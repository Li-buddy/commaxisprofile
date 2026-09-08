#include "test_profile_diag_msg.h"
#include "comm/axisprofile/interfaces/i_axisprofile_diag_msg_c.h"
#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "mockup/mock_axisprofile_property.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestProfileDiagMsg);
REGISTER_TEST_TO_GROUP(TestProfileDiagMsg, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileDiagMsg, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileDiagMsg, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProfileDiagMsg::TestProfileDiagMsg()
{
}


TestProfileDiagMsg::~TestProfileDiagMsg(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProfileDiagMsg::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the base class for profile diagnostic message.");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testCInterface();
  hResult |= testDiagMsg();

  return hResult;
}


HRESULT TestProfileDiagMsg::testCInterface()
{
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace profileTraceMock{};
  AcknowledgeTelegram atData;
  AxisProfileDiagMsg testClass(&profileTraceMock, &atData);
  log(L"\n<b>Test of the diagnostic message C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfileDiagMsgC* diagMsgItf = testClass.getCInterface();
  if (!diagMsgItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }


  log(L"\nCall AXISPROFILE_getErrorType() with nullptr handle...");
  diagMsgItf->getErrorType(nullptr);

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DIAGMSG, &testClass);

  log(L"\nCall AXISPROFILE_getErrorType() for unknown device series...");
  diagMsgItf->getErrorType(diagMsgItf->handle);

  log(L"\nCall AXISPROFILE_getErrorType() for ctrlX device series...");
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);
  diagMsgItf->getErrorType(diagMsgItf->handle);

  log(L"\nCall AXISPROFILE_getErrorCode() with nullptr handle...");
  auto errCode = diagMsgItf->getErrorCode(nullptr);
  RETURN_TEST_RESULT_ANY(errCode == UINT32_MAX, L"\nUnexpected return value");

  log(L"\nCall AXISPROFILE_getErrorType()...");
  errCode = diagMsgItf->getErrorCode(diagMsgItf->handle);
  RETURN_TEST_RESULT_ANY(errCode == UINT32_MAX, L"\nUnexpected return value");

  log(L"\n");
  return hResult;
}


HRESULT TestProfileDiagMsg::testDiagMsg()
{
  log(L"\n<b>Test interpretation of the diagnostic message.");

  TestCaseDiagMsg testcases[] =
  {
    { L"DiagMsg is zero",     0x00000000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_NO_INFO},
    { L"Command error",       0x000C0901, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_NONE},
    { L"Warning",             0x000E2025, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_NONE},
    { L"Error Message F2xxx", 0x000F2000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_LIGHT},
    { L"Error Message F3xxx", 0x000F3000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_LIGHT},
    { L"Error Message F4xxx", 0x000F4000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_FIELDBUS},
    { L"Error Message F6xxx", 0x000F6000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_CRITICAL},
    { L"Error Message F7xxx", 0x000F7000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_CRITICAL},
    { L"Error Message F8xxx", 0x000F8000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_FATAL},
    { L"Error not defined",   0x000F9000, AxisProfileResult::STS_OK, AxisProfileDeviceErrorType::ERROR_NOT_DEFINED},
  };

  HRESULT hresult = S_OK;
  for ( auto &elem : testcases )
  {
    hresult |= runTestcaseDiagMsg(&elem);
  }
  return hresult;
}

HRESULT TestProfileDiagMsg::runTestcaseDiagMsg(TestCaseDiagMsg* testcase)
{
  log(L"\n\tRun testcase: %ls", testcase->m_name);

  HRESULT hresult = S_OK;
  ProfileTrace traceHelper;
  AcknowledgeTelegram atData;
  atData.m_diagMsg.m_word = testcase->m_diagMsg;

  AxisProfileDiagMsg testDiagMsgInstance(&traceHelper, &atData);

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler shareHandler {&profileTraceMockup};
  test::MockAxisProfileProperty property {};
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, &property);
  shareHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DIAGMSG, &testDiagMsgInstance);
  property.setDeviceSeries(DeviceSeries::CTRLXDRIVE);

  AxisProfileResult result = testDiagMsgInstance.analyzeDiagMsg();
  if ( testcase->m_expectedResult != result )
  {
    error(L"Analyze of diagnostic message does not return expected status. Expected: 0x%X, Actual: 0x%X", testcase->m_expectedResult, result);
    hresult = E_FAIL;
  }

  AxisProfileDeviceErrorType actualErrorType = testDiagMsgInstance.getErrorType();
  if (actualErrorType != testcase->m_errorType )
  {
    error(L"Value of error type is not as expected. Expected: %d, Actual: 0x%X", testcase->m_errorType, actualErrorType);
    hresult = E_FAIL;
  }

  std::uint32_t actualErrorCode = testDiagMsgInstance.getErrorCode();
  RETURN_TEST_RESULT_ANY(actualErrorCode == testcase->m_diagMsg, L"\nUnexpected error code");

  if ( hresult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  return hresult;
}
