#include "helper/result_helper.h"
#include "test_axisprofile.h"

using namespace comm::axisprofile;


REGISTER_TEST(TestAxisProfile);
REGISTER_TEST_TO_GROUP(TestAxisProfile, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfile, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfile, Profiles);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfile::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile base class.");
  log(L"\n----------------------------------------------------------------");

  log(L"\n");
  hResult |= testCInterface();
  hResult |= testClass();

  return hResult;
}

HRESULT TestAxisProfile::testCInterface() const
{
  HRESULT hResult = S_OK;;
  AxisProfileUnittest testClassCInterface(m_profileName, m_testProfileType, m_testDriveProfileType, m_testNetworkTechnology);
  log(L"\n<b>Test of the command C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfileC* profileItf = testClassCInterface.createCInterface();
  if (!profileItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getCommandInterfaceC() with nullptr handle...");
  profileItf->getCommandInterface(nullptr);

  log(L"\nCall AXISPROFILE_getCommandInterfaceC()...");
  profileItf->getCommandInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getStatusControlInterfaceC() with nullptr handle...");
  profileItf->getStatusControlInterface(nullptr);

  log(L"\nCall AXISPROFILE_getStatusControlInterfaceC()...");
  profileItf->getStatusControlInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getDiagMsgInterfaceC() with nullptr handle...");
  profileItf->getDiagMsgInterface(nullptr);

  log(L"\nCall AXISPROFILE_getDiagMsgInterfaceC()...");
  profileItf->getDiagMsgInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getCyclicDataInterfaceC() with nullptr handle...");
  profileItf->getCyclicDataInterface(nullptr);

  log(L"\nCall AXISPROFILE_getBootingConfigInterfaceC() with nullptr handle...");
  profileItf->getBootingConfigInterface(nullptr);

  log(L"\nCall AXISPROFILE_getBootingConfigInterfaceC()...");
  profileItf->getBootingConfigInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getDeviceParamInterfaceC() with nullptr handle...");
  profileItf->getDeviceParamInterface(nullptr);

  log(L"\nCall AXISPROFILE_getgetDeviceParamInterfaceC()...");
  profileItf->getDeviceParamInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getCyclicDataInterfaceC()...");
  profileItf->getCyclicDataInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getProbeInterfaceC() with nullptr handle...");
  profileItf->getProbeInterface(nullptr);

  log(L"\nCall AXISPROFILE_getProbeInterfaceC()...");
  profileItf->getProbeInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getCmdInterfaceC() with nullptr handle...");
  auto itf = profileItf->getCmdInterface(nullptr);
  RETURN_TEST_RESULT_PTR(itf, L"\nUnexpected return value", nullptr);

  log(L"\nCall AXISPROFILE_getProbeInterfaceC()...");
  itf = profileItf->getCmdInterface(profileItf->handle);

  log(L"\nCall AXISPROFILE_getPropertyInterfaceC() with nullptr handle...");
  profileItf->getPropertyInterface(nullptr);

  log(L"\nCall AXISPROFILE_getPropertyInterfaceC()...");
  profileItf->getPropertyInterface(profileItf->handle);

  delete profileItf;
  profileItf = nullptr;

  log(L"\n");
  return hResult;
}

HRESULT TestAxisProfile::testClass()
{
  HRESULT result = S_OK;
  AxisProfileUnittest testClass(m_profileName, m_testProfileType, m_testDriveProfileType, m_testNetworkTechnology);
  testClass.setDeviceAddress(1);
  log(L"\n<b>Test of the axisprofile base class</b>");

  log(L"\nGet profile name.");
  std::string readName = testClass.getProfileName();
  if (readName != m_profileName)
  {
    error(L"\nReceived wrong profile name! Expected: %ls Actual: %ls", UTF8TOWIDE(m_profileName.c_str()), UTF8TOWIDE(readName.c_str()));
    result = E_FAIL;
  }

  log(L"\nCall getMappingHandler().");
  const IAxisProfileMappingHandler* mappingHandler = testClass.getMappingHandler();
  if (!mappingHandler)
  {
    error(L"\nThe mapping handler is a null-pointer!");
    result = E_FAIL;
  }

  log(L"\nCall getProfileType() and check the result.");
  AxisProfileType receivedType = testClass.getProfileType();
  if (receivedType != m_testProfileType)
  {
    error(L"\nWrong profile type! Expected: %ls Actual: %ls", test::StringHelper::utf8ToWide(axisProfileTypeToString(m_testProfileType)).c_str(), test::StringHelper::utf8ToWide(axisProfileTypeToString(receivedType)).c_str());
    result = E_FAIL;
  }

  std::uint32_t deviceAddress = 235;
  log(L"\nCall setDeviceAddress() with valid address %d.", deviceAddress);
  AxisProfileResult aprResult = testClass.setDeviceAddress(deviceAddress);
  if (aprStatusFailed(aprResult))
  {
    error(L"\nMethod failed but should have succeeded! Result: 0x%08X", aprResult);
    result = E_FAIL;
  }

  log(L"\nCall getDeviceAddress() and check the result.");
  std::uint32_t readAddress = testClass.getDeviceAddress();
  LOG_TEST_RESULT_S(readAddress, L"\nIncorrect device address", deviceAddress);

  log(L"\nCall getDeviceAddress() and check the result.");
  std::string strDeviceName{"testDevice"};
  aprResult = testClass.setDeviceName(strDeviceName.c_str());
  if ( aprStatusFailed(aprResult) )
  {
    error(L"\nMethod failed but should have succeeded! Result: 0x%08X", aprResult);
    result = E_FAIL;
  }

  log(L"\nCall getDeviceName() and check the result.");
  std::string strReadDeviceName = testClass.getDeviceName();
  if ( strDeviceName != strReadDeviceName )
  {
    error(L"\nIncorrect device address! Expected: %d Actual: %d", strDeviceName, strReadDeviceName);
    result = E_FAIL;
  }

  std::uint8_t driveNumber = 2;
  log(L"\nCall setDriveNumber() with valid drive number %d.", driveNumber);
  aprResult = testClass.setDriveNumber(driveNumber);
  if (aprStatusFailed(aprResult))
  {
    error(L"\nMethod failed but should have succeeded! Result: 0x%08X", aprResult);
    result = E_FAIL;
  }

  log(L"\nCall getDriveNumber() and check the result.");
  std::uint8_t readDriveNumber = testClass.getDriveNumber();
  if (driveNumber != readDriveNumber)
  {
    error(L"\nIncorrect drive number! Expected: %d Actual: %d", driveNumber, readDriveNumber);
    result = E_FAIL;
  }

  log(L"\nCall getDriveProfileType() and check the result.");
  DriveProfileType profileType = testClass.getDriveProfileType();
  LOG_TEST_RESULT(profileType, L"\nIncorrect drive profile type! Expected: %d Actual: %d", DriveProfileType::SERCOS);

  log(L"\nCall getNetworkTechnology() and check the result.");
  NetworkTechnology net = testClass.getNetworkTechnology();
  LOG_TEST_RESULT(net, L"\nIncorrect network technology! Expected: %d Actual: %d", NetworkTechnology::UNDEFINED);

  log(L"\nCall setProfileCategory() and check the result.");
  aprResult = testClass.setProfileCategory(AxisProfileCategory::CATEGORY_DRIVE);
  LOG_TEST_RESULT(aprResult, L"\nIncorrect result! Expected: %d Actual: %d", AxisProfileResult::STS_OK);

  log(L"\nCall setProfileCategory() and check the result.");
  aprResult = testClass.setProfileCategory(AxisProfileCategory::CATEGORY_ENCODER);
  LOG_TEST_RESULT(aprResult, L"\nIncorrect result! Expected: %d Actual: %d", AxisProfileResult::STS_OK);

  // real test will be implemented in next PR
  log(L"\nDummy test at empty method runJobsInBkThread.");
  testClass.runJobsInBkThread();

  log(L"\nDummy test at internal interfaces.");
  testClass.getCommandInterface();
  testClass.getProbeInterface();
  testClass.getScaling();
  testClass.getJobMgr();
  testClass.getProfileCategory();

  log(L"\n");
  return result;
}
