#include "test_profile_trace.h"
#include <cstring> // use memset under linux

// Test Class
#include "profiles/profile_trace.h"

REGISTER_TEST(TestProfileTrace);
REGISTER_TEST_TO_GROUP(TestProfileTrace, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileTrace, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileTrace, Profiles);

using namespace comm::axisprofile;


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT TestProfileTrace::doInit()
{
  return S_OK;
}

HRESULT TestProfileTrace::doEnd()
{
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProfileTrace::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test for the profile trace helper class. ");
  log(L"\n----------------------------------------------------------------");

  HRESULT res = testErrors();
  if (FAILED(res))
  {
    error(L"\nTest case failed! Aborting test.");
    return E_FAIL;
  }

  res = testDifferentTraceMessages();
  if (FAILED(res))
  {
    error(L"\nTest case failed! Aborting test.");
    return E_FAIL;
  }

  res = testReturnCodeTraceMessages();
  if (FAILED(res))
  {
    error(L"\nTest case failed! Aborting test.");
    return E_FAIL;
  }

  log(L"\n");

  return res;
}


HRESULT TestProfileTrace::testErrors()
{
  ProfileTrace testClass;

  log(L"\n\n<b>Test of all error cases.</b>");

  log(L"\nTestCase 1: Call all overloads of createTraceMsg() with empty string variables");

  log(L"\nCheck if resulting trace messages are empty.");
  std::string dummyString1{}, dummyString2{}, dummyString3{};
  const char* traceMsgPtr = testClass.createTraceMsg(dummyString1.c_str());
  if (strlen(traceMsgPtr) != 0)
  {
    error(L"\nTrace message is not empty: %ls", test::StringHelper::utf8ToWide(traceMsgPtr).c_str());
    return E_FAIL;
  }

  traceMsgPtr = testClass.createTraceMsg(AxisProfileResult::STS_OK, dummyString1.c_str(), dummyString2.c_str(), dummyString3.c_str());
  if (strlen(traceMsgPtr) != 0)
  {
    error(L"\nTrace message is not empty: %ls", test::StringHelper::utf8ToWide(traceMsgPtr).c_str());
    return E_FAIL;
  }

  log(L"\nSet basic trace data for further testing.");
  testClass.setConstantProfileData(m_testProfileName.c_str(), AxisProfileType::SERVODRIVE_OVER_ETHERCAT);

  log(L"\nTestCase 2: Enter more characters into the message buffer than allowed (>%d)", common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS);

  std::string stringWith100Characters = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
  std::string stringWith300Characters = stringWith100Characters + stringWith100Characters + stringWith100Characters;

  log(L"\nCall all overloads of createTraceMsg() with too many characters.");
  log(L"\nThis should succeed but message has to be truncated after %d characters!)", common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS);

  log(L"\nCheck length of resulting trace messages.");
  traceMsgPtr = testClass.createTraceMsg(stringWith300Characters.c_str());
  if (strlen(traceMsgPtr) != (common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS - 1))
  {
    error(L"\nWrong message size! Expected: %d Actual: %d", (common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS - 1), strlen(traceMsgPtr));
    return E_FAIL;
  }

  traceMsgPtr = testClass.createTraceMsg(AxisProfileResult::ERR_WRONG_DEVICE_STATE, stringWith300Characters.c_str());
  if (strlen(traceMsgPtr) != (common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS - 1))
  {
    error(L"\nWrong message size! Expected: %d Actual: %d", (common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS - 1), strlen(traceMsgPtr));
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestProfileTrace::testDifferentTraceMessages()
{
  ProfileTrace testClass;

  log(L"\n\n<b>Check generated trace messages under different conditions.</b>");

  log(L"\nTestCase 1: Do not set any data");
  log(L"\nCreate trace message.");
  const char* traceMsgResult = testClass.createTraceMsg();

  log(L"\nCheck if resulting trace message is empty.");
  if (strlen(traceMsgResult) != 0)
  {
    error(L"\nTrace message is not empty: %ls", test::StringHelper::utf8ToWide(traceMsgResult).c_str());
    return E_FAIL;
  }
  log(L" <DARK_GREEN><b>Success!</b></DARK_GREEN>");

  log(L"\nTestCase 2: Initialize profile trace data");
  const std::uint32_t deviceAddress = 0;
  char expectedResult[common::log::trace::MAXIMUM_STRING_LENGTH_128_CHARS];
  std::memset(expectedResult, 0, sizeof(expectedResult));
  snprintf(expectedResult, sizeof(expectedResult), "Name: %s; Type: SoE; Addr: %u", m_testProfileName.c_str(), deviceAddress);
  testClass.setConstantProfileData(m_testProfileName.c_str(), AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  testClass.updateDeviceAddress(deviceAddress);

  log(L"\nCall all overloads of createTraceMsg().");

  log(L"\nCheck resulting trace messages.");
  traceMsgResult = testClass.createTraceMsg();
  if (strncmp(traceMsgResult, expectedResult, sizeof(expectedResult)) != 0)
  {
    error(L"\nWrong trace message! Expected: %ls | Actual: %ls", test::StringHelper::utf8ToWide(expectedResult).c_str(),
                                                                 test::StringHelper::utf8ToWide(traceMsgResult).c_str());
    return E_FAIL;
  }

  traceMsgResult = testClass.createTraceMsg(AxisProfileResult::STS_OK);
  if (strncmp(traceMsgResult, expectedResult, sizeof(expectedResult)) != 0)
  {
    error(L"\nWrong trace message! Expected: %ls | Actual: %ls", test::StringHelper::utf8ToWide(expectedResult).c_str(),
      test::StringHelper::utf8ToWide(traceMsgResult).c_str());
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestProfileTrace::testReturnCodeTraceMessages()
{
  ProfileTrace testClass;

  log(L"\n\n<b>Use all possible ProfileResult return codes to generate trace messages.</b>");

  for (int i = 0; i < 4; i++)
  {
    // Use different additional information parameters to generate all different messages
    std::string dummyInfoA, dummyInfoB, dummyInfoC;
    switch (i)
    {
    case 0:
      log(L"\n\nPass one additional information parameter to the trace class (addInfo1).");
      dummyInfoA = "dummyInfoA";
      break;
    case 1:
      log(L"\n\nPass two additional information parameters to the trace class (addInfo1 + addInfo2).");
      dummyInfoA = "dummyInfoA";
      dummyInfoB = "dummyInfoB";
      break;
    case 2:
      log(L"\n\nPass two additional information parameters to the trace class (addInfo1 + addInfo3).");
      dummyInfoA = "dummyInfoA";
      dummyInfoC = "dummyInfoC";
      break;
    case 3:
      log(L"\n\nPass two additional information parameters to the trace class (addInfo2 + addInfo3).");
      dummyInfoB = "dummyInfoB";
      dummyInfoC = "dummyInfoC";
      break;
    default:
      break;
    }

    const char* defaultText = "Unspecified error";
    log(L"\nGenerate all possible profile status codes and use them to create a trace message:");
    for (int j = 0; j < 0xFFFF; j++)
    {
      auto cmdResult = static_cast<AxisProfileResult>(APR_STATUS_MAKE_SUCCESS(j));
      const char* traceMsgResult = testClass.createTraceMsg(cmdResult, dummyInfoA.c_str(), dummyInfoB.c_str(), dummyInfoC.c_str());
      if ((strlen(traceMsgResult) != 0) && strncmp(traceMsgResult, defaultText, strlen(defaultText)) != 0)
      {
        log(L"\nProfileResult: 0x%04X | TraceText: %ls", cmdResult, test::StringHelper::utf8ToWide(traceMsgResult).c_str());
      }
    }

    log(L"\nGenerate all possible profile error codes and use them to create a trace message:");
    for (int j = 0; j < 0xFFFF; j++)
    {
      auto cmdResult = static_cast<AxisProfileResult>(APR_STATUS_MAKE_ERROR(j));
      const char* traceMsgResult = testClass.createTraceMsg(cmdResult, dummyInfoA.c_str(), dummyInfoB.c_str(), dummyInfoC.c_str());
      if ((strlen(traceMsgResult) != 0) && strncmp(traceMsgResult, defaultText, strlen(defaultText)) != 0)
      {
        log(L"\nProfileResult: 0x%04X | TraceText: %ls", cmdResult, test::StringHelper::utf8ToWide(traceMsgResult).c_str());
      }
    }
  }

  return S_OK;
}
