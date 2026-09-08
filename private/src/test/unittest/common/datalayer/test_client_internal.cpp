
#include <random>
#include "test_client_internal.h"
#include "common/datalayer/client_internal.h"
#include "motion/test/result_helper.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestClientInternal);
REGISTER_TEST_TO_GROUP(TestClientInternal, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestClientInternal, 2_UnitTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestClientInternal::doInit()
{
  log(L"\nStart the datalayer system...");
  m_dlSystem.start();

  m_mockNodes.reset(new test::MockNodesHelper(m_dlSystem.factory()));
  log(L"\nCreate mockup nodes...");
  HRESULT res = m_mockNodes->init();
  RETURN_TEST_RESULT(res, L"\nGet an unexpected result!", S_OK);

  return res;
}

HRESULT TestClientInternal::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestClientInternal");
  log(L"\n----------------------------------------------------------------\n");

  res |= doFunctionTestCase(true);
  res |= doFunctionTestCase(false);
  res |= doTestErrorCase();

  return res;
}

HRESULT TestClientInternal::doEnd()
{
  HRESULT res = S_OK;

  log(L"\nDelete mockup nodes...");
  m_mockNodes->deInit();

  log(L"\nStop the datalayer system...");
  m_dlSystem.stop();
  return res;
}

HRESULT TestClientInternal::doFunctionTestCase(bool enableBkThread)
{
  HRESULT res = S_OK;

  log(L"\nTest wrapped datalayer client with %ls behavior...", enableBkThread ? L"background" : L"original");

  log(L"\nCreate a wrapped datalayer client...");
  comm::datalayer::ClientInternal client(m_dlSystem.factory()->createClient());

  log(L"\nCreate background thread...");
  m_bkThread.reset(new test::MockBackgroundThread2);
  m_bkThread->init();
  m_bkThread->appCallbackInNormalState(client.getBackgroundFun());
  m_bkThread->setAprState(StateIDType::RUNNING);
  client.enableBackgroundFun(enableBkThread);

  log(L"\nCreate callback of datalayer async write...");
  auto writeCb = [this] (comm::datalayer::DlResult status, const comm::datalayer::Variant* /*data*/)
    {
      this->log(L"\nwrite callback status: %ls", UTF8TOWIDE(status.toString()));
    };

  log(L"\nCreate callback of datalayer async read...");
  auto readCb = [this] (comm::datalayer::DlResult status, const comm::datalayer::Variant* data)
    {
      this->log(L"\nread callback status: %ls.", test::StringHelper::utf8ToWide(status.toString()).c_str());
      if (comm::datalayer::DlResult::DL_OK == status.value)
      {
        std::int32_t value = *data;
        this->log(L" value %d.", value);
      }
    };

  for (auto& it : MOCK_URLS)
  {
    // create a random value
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::int32_t> distribution(0, 65565);
    std::int32_t randomValue = distribution(gen);

    log(L"\nAsyncWrite value %d at %s...", randomValue, UTF8TOWIDE(it));
    comm::datalayer::Variant varWriteReq;
    varWriteReq.setValue(randomValue);
    auto dlRetValue = client.writeAsync(it, varWriteReq, writeCb);
    LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_OK);

    log(L"\nAsyncRead value at %s...", UTF8TOWIDE(it));
    comm::datalayer::Variant varReadReq;
    dlRetValue = client.readAsync(it, varReadReq, readCb);
    LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_OK);
  }
  log(L"\nTrigger a background execution...");
  m_bkThread->wakeup();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  log(L"\nRemove background thread...");
  m_bkThread.reset();

  log(L"\n");
  return res;
}

HRESULT TestClientInternal::doTestErrorCase()
{
  HRESULT res = S_OK;

  log(L"\nCreate a wrapped datalayer client with empty dl client...");
  comm::datalayer::ClientInternal client(nullptr);
  comm::datalayer::Variant dummyVar;
  log(L"\nCreate dummy callback...");
  auto dummyCb = [this] (comm::datalayer::DlResult /*status*/, const comm::datalayer::Variant* /*data*/)
    {
      return;
    };

  log(L"\nReadAsync when base client is empty...");
  auto dlRetValue = client.readAsync("", dummyVar, dummyCb);
  LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_INVALID_HANDLE);

  log(L"\nWriteAsync when base client is empty...");
  dlRetValue = client.writeAsync("", dummyVar, dummyCb);
  LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_INVALID_HANDLE);

  log(L"\nCreate a wrapped datalayer client with real dl client...");
  comm::datalayer::ClientInternal client2(m_dlSystem.factory()->createClient());

  log(L"\nCreate background thread...");
  m_bkThread.reset(new test::MockBackgroundThread2);
  m_bkThread->init();
  m_bkThread->appCallbackInNormalState(client2.getBackgroundFun());
  m_bkThread->setAprState(StateIDType::RUNNING);
  client2.enableBackgroundFun(true);

  log(L"\nCreate callback of datalayer async access...");
  auto dummyCb2 = [this] (comm::datalayer::DlResult status, const comm::datalayer::Variant* /*data*/)
    {
      this->log(L"\nCheck callback return value...");
      if (status != comm::datalayer::DlResult::DL_INVALID_ADDRESS)
      {
        this->error(L"\nshould be DL_INVALID_ADDRESS, but returns: %ls", UTF8TOWIDE(status.toString()));
      }
      else
      {
        this->log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>"); \
      }
    };

  log(L"\nAsync write at error node...");
  dlRetValue = client2.writeAsync("unknown-url", dummyVar, dummyCb2);
  m_bkThread->wakeup();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_OK);

  log(L"\nAsync read at error node...");
  dlRetValue = client2.readAsync("unknown-url", dummyVar, dummyCb2);
  m_bkThread->wakeup();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  LOG_TEST_RESULT(dlRetValue.value, L"\nGet an unexpected result!", comm::datalayer::DlResult::DL_OK);

  log(L"\nRemove background thread...");
  m_bkThread.reset();

  log(L"\n");
  return res;
}
