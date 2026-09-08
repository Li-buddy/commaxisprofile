#include "helper/result_helper.h"
#include "test_profile_param_access.h"
#include <memory>


using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestPorfileParamAccess);
REGISTER_TEST_TO_GROUP(TestPorfileParamAccess, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestPorfileParamAccess, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestPorfileParamAccess, Profiles);
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestPorfileParamAccess::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestPorfileParamAccess");
  log(L"\n----------------------------------------------------------------\n");

  res |= testErrorCase();

  return res;
}

HRESULT TestPorfileParamAccess::testErrorCase()
{
  AxisProfileResult result {AxisProfileResult::STS_OK}; // because test error case
  AsyncCommandStatemachine cmdState;
  RequestStatemachine requestState;
  ParamReqDataSoE readParamData {};
  ParamReqDataSoE writeParamData {};
  ProfileBasicInfo profileBasicInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profile, m_devAdd, m_devNum, &m_adapter, m_trace};

  log(L"\nCreate a new parameter access object...");
  std::unique_ptr<AxisProfileParamAccess> paramAccess;
  paramAccess.reset(new AxisProfileParamAccess(profileBasicInfo));

  log(L"\nTest error case in writeParamAsync...");
  m_adapter.m_returnValue = AxisProfileResult::ERR_DATALAYER; // any error
  result = paramAccess->writeParamAsync(writeParamData, cmdState);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", m_adapter.m_returnValue);

  log(L"\nTest error case in readParamAsync...");
  m_adapter.m_returnValue = AxisProfileResult::ERR_DATALAYER_BROWSE_NODE; // any error
  result = paramAccess->readParamAsync(readParamData, cmdState);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", m_adapter.m_returnValue);

  log(L"\nTest time out error in checkResponseAsync...");
  requestState.setCallbackTimeout(1); // return timeout
  requestState.reinitCallbackTick();
  requestState.tickCallbackAndCheckExceed();
  result = paramAccess->checkResponseAsync(requestState);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_TIMEOUT);

  log(L"\nTest datalayer error in checkResponseAsync...");
  requestState.setCallbackTimeout(async::TIMEOUT_CALLBACK);
  requestState.reinitCallbackTick();
  auto cbData = requestState.getCallbackData();
  cbData->m_requestFinishedFlag = true;
  cbData->m_dlResult = comm::datalayer::DlResult::DL_COMMUNICATION_ERROR;
  result = paramAccess->checkResponseAsync(requestState);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_DATALAYER);

  return S_OK;
}
