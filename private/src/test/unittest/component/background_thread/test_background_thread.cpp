
#include "test_background_thread.h"

#include "component/background_thread/background_thread.h"

#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/external_bundles/mock_system_state.h"
#include "helper/result_helper.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestBackgroundThread);
REGISTER_TEST_TO_GROUP(TestBackgroundThread, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestBackgroundThread, 2_UnitTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestBackgroundThread::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestBackgroundThread");
  log(L"\n----------------------------------------------------------------\n");

  res |= doSimpleTest();
  res |= doCallbackTest();

  return res;
}

HRESULT TestBackgroundThread::doSimpleTest()
{
  HRESULT res = S_OK;

  log(L"\n<GREY>Create mockup axisprofile component...</GREY>");
  test::MockAxisProfileCmp aprCmp;

  log(L"\n<b>Test base methods...</b>");
  log(L"\n<GREY>  Create background thread...</GREY>");
  m_bgThread.reset(new BackgroundThread(&aprCmp));
  log(L"\n<GREY>  Init() background thread...</GREY>");
  m_bgThread->init();

  log(L"\n<GREY>  set cycle time...</GREY>");
  auto aprRetValue = m_bgThread->setCycleTime(1.0);
  LOG_TEST_RESULT(aprRetValue, L"Unexpected return value", AxisProfileResult::STS_OK);

  log(L"\n<GREY>  Remove background thread...</GREY>");
  std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
  m_bgThread.reset(nullptr);

  return res;
}

HRESULT TestBackgroundThread::doCallbackTest()
{
  HRESULT res = S_OK;
  AxisProfileResult aprResult {AxisProfileResult::STS_OK};

  log(L"\n<GREY>Create mockup axisprofile component...</GREY>");
  test::MockAxisProfileCmp aprCmp;
  const auto aprSm = static_cast<test::MockStateMachine*>(aprCmp.getStateMachine());

  log(L"\n<b>Test base methods...</b>");

  log(L"\n<GREY>  Create background thread...</GREY>");
  m_bgThread.reset(new BackgroundThread(&aprCmp));
  log(L"\n<GREY>  Init() background thread...</GREY>");
  m_bgThread->init();

  log(L"\n<GREY>  Error case, Try to append normal state callback in BOOTING state...</GREY>");
  aprSm->m_stateID = StateIDType::BOOTING;
  aprResult = m_bgThread->appCallbackInNormalState(callbackFun);
  LOG_TEST_RESULT(aprResult, L"\nUnexpected return value", AxisProfileResult::ERR_INVALID_STATE);

  log(L"\n<GREY>  Error case, Try to append running state callback in RUNNING state...</GREY>");
  aprSm->m_stateID = StateIDType::RUNNING;
  aprResult = m_bgThread->appCallbackInRunningState(callbackFun);
  LOG_TEST_RESULT(aprResult, L"\nUnexpected return value", AxisProfileResult::ERR_INVALID_STATE);

  log(L"\n<GREY>  Try to append normal state callback in service state...</GREY>");
  aprSm->m_stateID = StateIDType::CONFIG;
  aprResult = m_bgThread->appCallbackInNormalState(callbackFun);
  LOG_TEST_RESULT(aprResult, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n<GREY>  Try to append running state callback in CONFIG state...</GREY>");
  aprSm->m_stateID = StateIDType::CONFIG;
  aprResult = m_bgThread->appCallbackInRunningState(callbackFun);
  LOG_TEST_RESULT(aprResult, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n<GREY>  Wake up one tick to normal execution callback...</GREY>");
  aprSm->m_stateID = StateIDType::BOOTING;
  m_bgThread->wakeup();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  log(L"\n<GREY>  Abort execution...</GREY>");
  aprSm->m_stateID = StateIDType::ERROR_STATE;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  log(L"\n<GREY>  Create a new background thread...</GREY>");
  m_bgThread.reset(new BackgroundThread(&aprCmp));
  log(L"\n<GREY>  Init() background thread...</GREY>");
  m_bgThread->init();

  log(L"\n<GREY>  Try to append running state callback in service state...</GREY>");
  aprSm->m_stateID = StateIDType::CONFIG;
  aprResult = m_bgThread->appCallbackInRunningState(callbackFun);
  LOG_TEST_RESULT(aprResult, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n<GREY>  Wake up one tick to running execution callback...</GREY>");
  aprSm->m_stateID = StateIDType::RUNNING;
  m_bgThread->wakeup();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  log(L"\n<GREY>  Abort execution...</GREY>");
  aprSm->m_stateID = StateIDType::CONFIG;
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  log(L"\n<GREY>  Remove background thread...</GREY>");
  std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
  m_bgThread.reset(nullptr);

  return res;
}

void TestBackgroundThread::callbackFun()
{
  std::uint16_t cnt {10};
  while (cnt)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cnt--;
  }
}
