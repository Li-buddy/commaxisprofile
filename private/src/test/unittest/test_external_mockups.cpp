#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/mockup/mock_axisprofile_manager.h"
#include "comm/axisprofile/mockup/mock_axisprofile_system.h"
#include "test_external_mockups.h"
#include <helper/result_helper.h>

using namespace comm::axisprofile;


REGISTER_TEST(TestAxisProfileExternalMockups);
REGISTER_TEST_TO_GROUP(TestAxisProfileExternalMockups, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileExternalMockups, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileExternalMockups, General);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestAxisProfileExternalMockups::TestAxisProfileExternalMockups()
{
}

TestAxisProfileExternalMockups::~TestAxisProfileExternalMockups(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfileExternalMockups::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile external mockups.");
  log(L"\n----------------------------------------------------------------");

  HRESULT res = S_OK;

  res |= testSystemMockup();
  res |= testProfileManagerMockup();
  res |= testAxisProfileMockup();
  res |= testCommandMockup();
  res |= testCyclicDataMockup();
  res |= testStatusControlMockup();
  res |= testDiagMsgMockup();
  res |= testDeviceParamMockup();
  res |= testPropertyMockup();

  return res;
}


HRESULT TestAxisProfileExternalMockups::testSystemMockup()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test the mockup for the system interface.");

  MockAxisProfileSystem testClass{};

  log(L"\n\tCall requestAxisProfileState() and check result.");
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  AxisProfileResult aprResult = testClass.requestAxisProfileState("Running");
  if (aprResult != expectedResult)
  {
    error(L"Method should have succeeded. Result: 0x%X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\n\tCall getAxisProfileStateNames() and check result.");
  std::vector<std::string> stateNames{};
  aprResult = testClass.getAxisProfileStateNames(stateNames);
  if (aprResult != expectedResult)
  {
    error(L"Method should have succeeded. Result: 0x%X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\n\tCall getAxisProfileState() and check result.");
  std::string currenState = testClass.getAxisProfileState();
  if (currenState.empty())
  {
    error(L"Received empty string from method!");
    hResult = E_FAIL;
  }

  return hResult;
}

HRESULT TestAxisProfileExternalMockups::testProfileManagerMockup()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test the mockup for the axis profile manager.");

  log(L"\n\tCreate MockAxisProfileManager.");
  MockAxisProfileManager *axisProfileManagerMock = new MockAxisProfileManager;

  log(L"\n\tCreate some profiles.");
  axisProfileManagerMock->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile1");
  axisProfileManagerMock->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile2");
  axisProfileManagerMock->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "testProfile3");

  log(L"\n\tConnect profile.");
  std::shared_ptr<const IAxisProfileC> profile{};
  AxisProfileResult expectedResult = AxisProfileResult::STS_OK;
  AxisProfileResult aprResult = axisProfileManagerMock->connectAxisProfile("testProfile1", profile);
  if (aprResult != expectedResult)
  {
    error(L"Method should have succeeded. Result: 0x%X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\n\tConnect profile that is already in use.");
  expectedResult = AxisProfileResult::ERR_PROFILE_IN_USE;
  aprResult = axisProfileManagerMock->connectAxisProfile("testProfile1", profile);
  if (aprResult != expectedResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\n\tDelete one profile.");
  axisProfileManagerMock->deleteAxisProfile("testProfile3");

  log(L"\n\tDestroy manager (should delete all remaining profiles).");
  delete axisProfileManagerMock;
  axisProfileManagerMock = nullptr;

  return hResult;
}

HRESULT TestAxisProfileExternalMockups::testAxisProfileMockup()
{
  log(L"\n<b>Test the mockup for the axis profile.");
  std::uint32_t deviceAddress = 1;
  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  auto propertyIf = axisProfile.m_mockProfileShared->getPropertyInterface(axisProfile.m_mockProfileShared->handle);
  propertyIf->setDeviceAddress(propertyIf->handle, deviceAddress);

  const IAxisProfileCommandC *commmandInterface = axisProfile.m_mockProfileShared->getCommandInterface(axisProfile.m_mockProfileShared->handle);
  if ( commmandInterface == nullptr )
  {
    error(L"Command interface pointer is not valid.");
  }

  const IAxisProfileCyclicDataC *cyclicDataInterface = axisProfile.m_mockProfileShared->getCyclicDataInterface(axisProfile.m_mockProfileShared->handle);
  if ( cyclicDataInterface == nullptr )
  {
    error(L"Cyclic data interface pointer is not valid.");
  }

  const IAxisProfileStatusControlC *statusControlInterface = axisProfile.m_mockProfileShared->getStatusControlInterface(axisProfile.m_mockProfileShared->handle);
  if ( statusControlInterface == nullptr )
  {
    error(L"Status/Control interface pointer is not valid.");
  }

  const IAxisProfileDiagMsgC* diagMsgInterface = axisProfile.m_mockProfileShared->getDiagMsgInterface(axisProfile.m_mockProfileShared->handle);
  if (diagMsgInterface == nullptr)
  {
    error(L"Diagnostic Message interface pointer is not valid.");
  }

  const IAxisProfileBootingConfigC* bootingConfigInterface = axisProfile.m_mockProfileShared->getBootingConfigInterface(axisProfile.m_mockProfileShared->handle);
  if ( bootingConfigInterface == nullptr )
  {
    error(L"Booting config interface pointer is not valid.");
  }

  const IAxisProfileDeviceParamC* deviceParamInterface = axisProfile.m_mockProfileShared->getDeviceParamInterface(axisProfile.m_mockProfileShared->handle);
  if (deviceParamInterface == nullptr)
  {
    error(L"Device parameter interface pointer is not valid.");
  }

  AxisProfileResult deviceResult = propertyIf->getDeviceState(propertyIf->handle);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Device result is not ok. Error: 0x%x", deviceResult);
  }

  AxisProfileType profileType = propertyIf->getProfileType(propertyIf->handle);
  RETURN_TEST_RESULT(profileType, L"\nUnexpected return value.", AxisProfileType::UNDEFINED);

  std::uint32_t readAddress = propertyIf->getDeviceAddress(propertyIf->handle);
  LOG_TEST_RESULT_S(readAddress, L"\nIncorrect device address", deviceAddress);

  AxisProfileResult deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_propertyMockHandle.m_retValue = deviceResultExpected;
  deviceResult = propertyIf->getDeviceState(propertyIf->handle);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Device result is not as expected.");
  }

  AxisProfileDeviceScalingType scalingType = propertyIf->getDeviceScalingType(propertyIf->handle, AxisProfileDeviceValueType::POSITION);
  if (scalingType != AxisProfileDeviceScalingType::UNDEFINED)
  {
    error(L"ScalingType is not ok.");
  }

  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testCommandMockup()
{
  log(L"\n<b>Test the mockup for the axis profile command interface.");

  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfileCommandC *testAxisProfileCommandMock = axisProfile.m_mockProfileShared->getCommandInterface(axisProfile.m_mockProfileShared->handle);

  AxisProfileResult deviceResult = testAxisProfileCommandMock->clearDeviceError(testAxisProfileCommandMock->handle);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"ClearError result is not ok. Error: 0x%x", deviceResult);
  }

  AxisProfileResult deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->clearDeviceError(testAxisProfileCommandMock->handle);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  AxisProfilePositiveStop mode = AxisProfilePositiveStop::ENABLE;
  bool metrication = true;
  double standStillWindow = 30.0;
  double clampingTorque = 10.2f;
  bool standstill = true;


  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckPositiveStop(testAxisProfileCommandMock->handle, mode);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"setAndCheckPositiveStop result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckPositiveStop(testAxisProfileCommandMock->handle, mode);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckClampingTorque(testAxisProfileCommandMock->handle, clampingTorque, metrication);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"setAndCheckClampingTorque result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckClampingTorque(testAxisProfileCommandMock->handle, clampingTorque, metrication);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->getClampingTorque(testAxisProfileCommandMock->handle, &clampingTorque, metrication);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"getClampingTorque result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->getClampingTorque(testAxisProfileCommandMock->handle, &clampingTorque, metrication);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->checkStandStill(testAxisProfileCommandMock->handle, &standstill);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"checkStandStill result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->checkStandStill(testAxisProfileCommandMock->handle, &standstill);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckStandStillWindow(testAxisProfileCommandMock->handle, standStillWindow);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"setAndCheckStandStillWindow result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAndCheckStandStillWindow(testAxisProfileCommandMock->handle, standStillWindow);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->getStandStillWindow(testAxisProfileCommandMock->handle, &standStillWindow);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"getStandStillWindow result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->getStandStillWindow(testAxisProfileCommandMock->handle, &standStillWindow);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->activateCyclicStandStillCheck(testAxisProfileCommandMock->handle);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"activateCyclicStandStillCheck result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->activateCyclicStandStillCheck(testAxisProfileCommandMock->handle);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->deactivateCyclicStandStillCheck(testAxisProfileCommandMock->handle);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"deactivateCyclicStandStillCheck result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->deactivateCyclicStandStillCheck(testAxisProfileCommandMock->handle);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAbsolutePosition(testAxisProfileCommandMock->handle, 100.0, AxisProfileEncoder::ENCODER_1, true);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"setAbsolutePosition result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->setAbsolutePosition(testAxisProfileCommandMock->handle, 100.0, AxisProfileEncoder::ENCODER_1, true);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }


  double refPos{};
  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->referencingAxis(testAxisProfileCommandMock->handle, refPos, AxisProfileEncoder::ENCODER_1);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"referencingAxis result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->referencingAxis(testAxisProfileCommandMock->handle, refPos, AxisProfileEncoder::ENCODER_1);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->resetReferencingAxis(testAxisProfileCommandMock->handle);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"resetReferencingAxis result is not ok. Error: 0x%x", deviceResult);
  }

  deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_commandMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCommandMock->resetReferencingAxis(testAxisProfileCommandMock->handle);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work.", deviceResult);
  }    

  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testCyclicDataMockup()
{
  log(L"\n<b>Test the mockup for the axis profile cyclic data interface.");

  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfileCyclicDataC *testAxisProfileCyclicDataMock = axisProfile.m_mockProfileShared->getCyclicDataInterface(axisProfile.m_mockProfileShared->handle);

  CommandValuesDevice commandValues{10.0, 20.0, 10.0, 100, -100};

  AxisProfileResult deviceResult = testAxisProfileCyclicDataMock->setCommandValuesDevice(testAxisProfileCyclicDataMock->handle, commandValues);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Error during writing of commanded values. Error: 0x%x", deviceResult);
  }

  ActualValuesDevice actualValues{ 0, false, 0, 0, TorqueVelocityLimitStatus::NO_VIOLATE};

  deviceResult = testAxisProfileCyclicDataMock->getActualValuesDevice(testAxisProfileCyclicDataMock->handle, &actualValues);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Error during reading of actual values. Error: 0x%x", deviceResult);
  }
  if ( actualValues.m_actualPosition != commandValues.m_commandPosition || actualValues.m_actualVelocity != commandValues.m_commandVelocity
     || actualValues.m_actualTorqueForce != commandValues.m_commandTorqueForce)
  {
    error(L"Actual values are not equal to commanded values", deviceResult);
  }

  bool digitalInputValue = false;
  bool digitalInputValueExpected = true;
  DigitalInputValueBitType valueType = DigitalInputValueBitType::REFERENCE_SWITCH_BIT;
  axisProfile.m_axisProfileHandle.m_cyclicDataMockHandle.setDigitalInputValueBitMock(valueType, digitalInputValueExpected);
  deviceResult = testAxisProfileCyclicDataMock->getDigitalInputValueBit(testAxisProfileCyclicDataMock->handle, valueType, digitalInputValue);
  if (aprStatusFailed(deviceResult))
  {
    error(L"Error during reading of digital input value. Error: 0x%x", deviceResult);
  }
  if (digitalInputValue != digitalInputValueExpected)
  {
    error(L"Digital input value is not equal to expected value", deviceResult);
  }

  AxisProfileResult deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_cyclicDataMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileCyclicDataMock->setCommandValuesDevice(testAxisProfileCyclicDataMock->handle, commandValues);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work for commanded values.");
  }

  deviceResult = testAxisProfileCyclicDataMock->getActualValuesDevice(testAxisProfileCyclicDataMock->handle, &actualValues);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work for actual values.");
  }

  deviceResult = testAxisProfileCyclicDataMock->getDigitalInputValueBit(testAxisProfileCyclicDataMock->handle, valueType, digitalInputValue);
  if (deviceResult != deviceResultExpected)
  {
    error(L"Setting return value does not work for input value.");
  }

  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testStatusControlMockup()
{
  log(L"\n<b>Test the mockup for the axis profile status/control interface.");

  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfileStatusControlC *testAxisProfileStatusControlMock = axisProfile.m_mockProfileShared->getStatusControlInterface(axisProfile.m_mockProfileShared->handle);

  log(L"\n\tCheck methods for power mode.");
  AxisProfileResult deviceResult = testAxisProfileStatusControlMock->setPowerMode(testAxisProfileStatusControlMock->handle, AxisProfilePowerMode::POWER_ON);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Error during setting of power mode. Error: 0x%x", deviceResult);
  }
  if ( !testAxisProfileStatusControlMock->checkPowerMode(testAxisProfileStatusControlMock->handle, AxisProfilePowerMode::POWER_ON) )
  {
    error(L"Check of power mode returns false instead of true.");
  }
  if ( AxisProfilePowerMode::POWER_ON != testAxisProfileStatusControlMock->getPowerMode(testAxisProfileStatusControlMock->handle) )
  {
    error(L"getPowerMode does not return power on.");
  }
  
  log(L"\n\tCheck methods for operation mode.");
  deviceResult = testAxisProfileStatusControlMock->setOperationMode(testAxisProfileStatusControlMock->handle, AxisProfileOperationMode::POSITIONING);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Error during setting of operation mode. Error: 0x%x", deviceResult);
  }

  if ( !testAxisProfileStatusControlMock->checkOperationMode(testAxisProfileStatusControlMock->handle, AxisProfileOperationMode::POSITIONING) )
  {
    error(L"Check of operation mode returns false instead of true.");
  }

  axisProfile.m_axisProfileHandle.m_statusControlMockHandle.m_opModeSupported = true;
  axisProfile.m_axisProfileHandle.m_statusControlMockHandle.m_opModeErrorInfo = { OpModeSupportedErrorType::ERROR_NONE, "supported"};
  OpModeErrorInfo opModeErrorInfo{};
  if (!testAxisProfileStatusControlMock->checkOperationModeSupported(testAxisProfileStatusControlMock->handle, AxisProfileOperationMode::POSITIONING, opModeErrorInfo))
  {
    error(L"Check of operation mode supported returns false instead of true.");
  }

  if ( AxisProfileOperationMode::POSITIONING != testAxisProfileStatusControlMock->getOperationMode(testAxisProfileStatusControlMock->handle) )
  {
    error(L"getOperationMode does not return positioning.");
  }
  
  log(L"\n\tCheck methods for cyclic standstill check .");
  testAxisProfileStatusControlMock->cyclicStandStillCheck(testAxisProfileStatusControlMock->handle);

  log(L"\n\tCheck methods for status-/controlword.");
  deviceResult = testAxisProfileStatusControlMock->setControlWord(testAxisProfileStatusControlMock->handle, 10);
  if ( aprStatusFailed(deviceResult) )
  {
    error(L"Error during setting of controlword. Error: 0x%x", deviceResult);
  }
  if ( testAxisProfileStatusControlMock->checkError(testAxisProfileStatusControlMock->handle) )
  {
    error(L"Check for error returns true instead of false.");
  }
  if ( 0 != testAxisProfileStatusControlMock->getStatusWord(testAxisProfileStatusControlMock->handle) )
  {
    error(L"getStatusWord does not return 0.");
  }

  axisProfile.m_axisProfileHandle.m_statusControlMockHandle.m_deviceError = true;
  if (!testAxisProfileStatusControlMock->checkError(testAxisProfileStatusControlMock->handle) )
  {
    error(L"setDeviceError does not work.");
  }

  log(L"\n\tCheck modifieing of return values");
  AxisProfileResult deviceResultExpected = AxisProfileResult::ERR_NOK;
  axisProfile.m_axisProfileHandle.m_statusControlMockHandle.setReturnValue(deviceResultExpected);
  deviceResult = testAxisProfileStatusControlMock->setPowerMode(testAxisProfileStatusControlMock->handle, AxisProfilePowerMode::POWER_OFF);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work for commanded values.");
  }
  deviceResult = testAxisProfileStatusControlMock->setOperationMode(testAxisProfileStatusControlMock->handle, AxisProfileOperationMode::POSITIONING);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work for setOperationMode.");
  }
  deviceResult = testAxisProfileStatusControlMock->setControlWord(testAxisProfileStatusControlMock->handle, 10);
  if ( deviceResult != deviceResultExpected )
  {
    error(L"Setting return value does not work for setControlWord.");
  }
  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testDiagMsgMockup()
{
  log(L"\n<b>Test the mockup for the axis profile diagnostic message interface.");

  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfileDiagMsgC* testAxisProfileDiagMsgMock = axisProfile.m_mockProfileShared->getDiagMsgInterface(axisProfile.m_mockProfileShared->handle);

  log(L"\n\tCheck methods for cyclic standstill check .");
  testAxisProfileDiagMsgMock->getErrorType(testAxisProfileDiagMsgMock->handle);
  testAxisProfileDiagMsgMock->getErrorCode(testAxisProfileDiagMsgMock->handle);

  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testDeviceParamMockup()
{
  log(L"\n<b>Test the mockup for the axis profile booting config interface.");
  MockProfileInfo axisProfile{};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfileDeviceParamC *testAxisProfileDeviceParamMock = axisProfile.m_mockProfileShared->getDeviceParamInterface(axisProfile.m_mockProfileShared->handle);

  bool delayExpected = false;
  bool delaySupport = testAxisProfileDeviceParamMock->checkBrakeDelaySupport(testAxisProfileDeviceParamMock->handle);
  if (delayExpected != delaySupport)
  {
    error(L"delaySupport is not as expected.");
  }

  AxisProfileResult resultExpected = AxisProfileResult::STS_OK;
  axisProfile.m_axisProfileHandle.setReturnValue(resultExpected);

  double delayTime = 0;
  AxisProfileResult getBrakeDelayInfo = testAxisProfileDeviceParamMock->getErrorBrakeDelayInfo(testAxisProfileDeviceParamMock->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, delaySupport, delayTime);
  if (getBrakeDelayInfo != resultExpected)
  {
    error(L"getBrakedelayInfo is not as expected.");
  }

  double deceleration = 0;
  double jerk = 0;
  AxisProfileResult getBrakeDelayLimit = testAxisProfileDeviceParamMock->getErrorBrakeDynLimit(testAxisProfileDeviceParamMock->handle, AxisProfileDeviceErrorType::ERROR_LIGHT, deceleration, jerk);
  if (getBrakeDelayLimit != resultExpected)
  {
    error(L"getBrakedelayInfo is not as expected.");
  }

  return S_OK;
}

HRESULT TestAxisProfileExternalMockups::testPropertyMockup()
{
  log(L"\n</b>Test the mockup for the axis profile property interface.");

  MockProfileInfo axisProfile {};
  axisProfile.m_mockProfileShared.reset(new IAxisProfileC());
  MockAxisProfileManager::configureProfileCInterface(axisProfile);
  const IAxisProfilePropertyC* testAxisProfilePropertyMock = axisProfile.m_mockProfileShared->getPropertyInterface(axisProfile.m_mockProfileShared->handle);

  std::uint32_t deviceAddressExpected = 235;
  AxisProfileResult result = testAxisProfilePropertyMock->setDeviceAddress(testAxisProfilePropertyMock->handle, deviceAddressExpected);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  std::uint32_t deviceAddress = testAxisProfilePropertyMock->getDeviceAddress(testAxisProfilePropertyMock->handle);
  LOG_TEST_RESULT_S(deviceAddress, L"\nIncorrect device address", deviceAddressExpected);

  result = testAxisProfilePropertyMock->getDeviceState(testAxisProfilePropertyMock->handle);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  AxisProfileType profileType = testAxisProfilePropertyMock->getProfileType(testAxisProfilePropertyMock->handle);
  RETURN_TEST_RESULT(profileType, L"\nUnexpected return value.", AxisProfileType::UNDEFINED);

  AxisProfileDeviceScalingType scalingType = testAxisProfilePropertyMock->getDeviceScalingType(testAxisProfilePropertyMock->handle, AxisProfileDeviceValueType::POSITION);
  RETURN_TEST_RESULT(scalingType, L"\nUnexpected return value.", AxisProfileDeviceScalingType::UNDEFINED);

  return S_OK;
}
