#include "test_axisprofile_factory.h"

#include <string.h>

#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/i_axisprofile_internal.h"
#include "profiles/axisprofile_factory.h"
#include "helper/result_helper.h"


using namespace comm::axisprofile;


REGISTER_TEST(TestAxisProfileFactory);
REGISTER_TEST_TO_GROUP(TestAxisProfileFactory, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileFactory, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileFactory, Profiles);


HRESULT TestAxisProfileFactory::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile factory");
  log(L"\n----------------------------------------------------------------");

  doFactoryTest();
  doCheckRequestedProfileTest();

  return S_OK;
}


HRESULT TestAxisProfileFactory::doFactoryTest()
{
  HRESULT result = S_OK;

  // create the profile factory
  AxisProfileFactory testProfileFactory;

  //Try to create Servodrive over Ethercat (SoE) axis profile..
  log(L"\nTry to create Servodrive over Ethercat (SoE) axis profile.. ");
  std::string name{ "testProfile" };
  IAxisProfileInternal* profile{};
  AxisProfileResult aprResult = AxisProfileFactory::createProfile(name, AxisProfileType::SERVODRIVE_OVER_ETHERCAT, &profile);
  if ( !profile || ( aprStatusFailed(aprResult) ) )
  {
    error(L"\n%s:%d %s", LITERALWIDECHAR(__FILE__), __LINE__, L"Could not create SoE axis profile!");
    errorSilent();
  }
  else
  {
    const char* devName = profile->getProfileName();
    log(L"\nProfile %s created.", UTF8TOWIDE(devName));

    delete profile;
    profile = nullptr;
    log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");
  }

  //Try to create Canopen over Ethercat (CoE) axis profile
  log(L"\nTry to create Canopen over Ethercat (CoE) axis profile.. ");
  std::string profileNameCOE{ "testProfile" };
  IAxisProfileInternal* profileCOE{};
  aprResult = AxisProfileFactory::createProfile(profileNameCOE, AxisProfileType::CANOPEN_OVER_ETHERCAT, &profileCOE);
  if ( !profileCOE || ( aprStatusFailed(aprResult) ) )
  {
    error(L"\n%s:%d %s", LITERALWIDECHAR(__FILE__), __LINE__, L"Could not create CoE axis profile!");
    errorSilent();
  }
  else
  {
    const char* devName = profileCOE->getProfileName();
    log(L"\nProfile %s created.", UTF8TOWIDE(devName));

    delete profileCOE;
    profileCOE = nullptr;
    log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");
  }

  //Try to create different axis profiles that are not supported ..
  log(L"\nTry to create different axis profiles that are not supported .. ");
  std::string nameFail{ "failProfile" };
  IAxisProfileInternal* undefinedProfile{};
  aprResult = AxisProfileFactory::createProfile(nameFail, AxisProfileType::UNDEFINED, &undefinedProfile);
  if ( undefinedProfile || ( aprStatusSucceeded(aprResult) ) )
  {
    delete undefinedProfile;
    undefinedProfile = nullptr;
    error(L"\nCould create invalid profile! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::SERCOS)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::ETHERCAT)));
  }

  log(L"\nTest getInputMappingValueID CANOPEN_OVER_ETHERCAT .. ");
  std::map<InputMappingValueID, MappingCfgInfo> InputMappingCfgInfo;
  aprResult = AxisProfileFactory::getInputMappingValueIDList(InputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::CANOPEN_OVER_ETHERCAT);
  if ( aprResult != AxisProfileResult::STS_OK )
  {
    error(L"Test getInputMappingValueID CANOPEN_OVER_ETHERCAT failed! ");
  }

  aprResult = AxisProfileFactory::getInputMappingValueIDList(InputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  if ( aprResult != AxisProfileResult::STS_OK )
  {
    error(L"Test getInputMappingValueID SERVODRIVE_OVER_ETHERCAT failed! ");
  }

  aprResult = AxisProfileFactory::getInputMappingValueIDList(InputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::UNDEFINED);
  if ( aprResult != AxisProfileResult::ERR_INVALID_ARGUMENT )
  {
    error(L"Test getInputMappingValueID UNDEFINED failed! ");
  }

  aprResult = AxisProfileFactory::getInputMappingValueIDList(InputMappingCfgInfo, AxisProfileCategory::CATEGORY_ENCODER, AxisProfileType::UNDEFINED);
  if (aprResult != AxisProfileResult::STS_OK)
  {
    error(L"Test getInputMappingValueID UNDEFINED failed! ");
  }

  aprResult = AxisProfileFactory::getInputMappingValueIDList(InputMappingCfgInfo, AxisProfileCategory::UNDEFINED, AxisProfileType::UNDEFINED);
  if (aprResult != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"Test getInputMappingValueID UNDEFINED failed! ");
  }

  std::map<OutputMappingValueID, MappingCfgInfo> OutputMappingCfgInfo;
  aprResult = AxisProfileFactory::getOutputMappingValueIDList(OutputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::CANOPEN_OVER_ETHERCAT);
  if ( aprResult != AxisProfileResult::STS_OK )
  {
    error(L"Test getOutputMappingValueID CANOPEN_OVER_ETHERCAT failed! ");
  }

  aprResult = AxisProfileFactory::getOutputMappingValueIDList(OutputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  if ( aprResult != AxisProfileResult::STS_OK )
  {
    error(L"Test getOutputMappingValueID SERVODRIVE_OVER_ETHERCAT failed! ");
  }

  aprResult = AxisProfileFactory::getOutputMappingValueIDList(OutputMappingCfgInfo, AxisProfileCategory::CATEGORY_DRIVE, AxisProfileType::UNDEFINED);
  if ( aprResult != AxisProfileResult::ERR_INVALID_ARGUMENT )
  {
    error(L"Test getOutputMappingValueID UNDEFINED failed! ");
  }

  aprResult = AxisProfileFactory::getOutputMappingValueIDList(OutputMappingCfgInfo, AxisProfileCategory::CATEGORY_ENCODER, AxisProfileType::UNDEFINED);
  if (aprResult != AxisProfileResult::STS_OK)
  {
    error(L"Test getOutputMappingValueID UNDEFINED failed! ");
  }

  aprResult = AxisProfileFactory::getOutputMappingValueIDList(OutputMappingCfgInfo, AxisProfileCategory::UNDEFINED, AxisProfileType::UNDEFINED);
  if (aprResult != AxisProfileResult::ERR_INVALID_ARGUMENT)
  {
    error(L"Test getOutputMappingValueID UNDEFINED failed! ");
  }

  log(L"\n");
  return result;
}

HRESULT TestAxisProfileFactory::doCheckRequestedProfileTest()
{
  HRESULT testResult = S_OK;
  //verify request information for UNDEFINE & SoE & ETHERCAT
  AxisProfileResult actualResult = AxisProfileFactory::checkRequestedProfile(AxisProfileType::UNDEFINED, DriveProfileType::SERCOS, NetworkTechnology::ETHERCAT);
  AxisProfileResult expectResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( actualResult != expectResult )
  {
    testResult = E_FAIL;
    error(L"\nChecked failed! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::SERCOS)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::ETHERCAT)));
  }

  //verify request information for UNDEFINE & SoE & UNDEFINED
  actualResult = AxisProfileFactory::checkRequestedProfile(AxisProfileType::UNDEFINED, DriveProfileType::SERCOS, NetworkTechnology::UNDEFINED);
  expectResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( actualResult != expectResult )
  {
    testResult = E_FAIL;
    error(L"\nChecked failed! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::SERCOS)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::UNDEFINED)));
  }

  //verify request information for UNDEFINE & CoE & ETHERCAT
  actualResult = AxisProfileFactory::checkRequestedProfile(AxisProfileType::UNDEFINED, DriveProfileType::CANOPEN, NetworkTechnology::ETHERCAT);
  expectResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( actualResult != expectResult )
  {
    testResult = E_FAIL;
    error(L"\nChecked failed! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::CANOPEN)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::ETHERCAT)));
  }

  //verify request information for UNDEFINE & CoE & UNDEFINED
  actualResult = AxisProfileFactory::checkRequestedProfile(AxisProfileType::UNDEFINED, DriveProfileType::CANOPEN, NetworkTechnology::UNDEFINED);
  expectResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( actualResult != expectResult )
  {
    testResult = E_FAIL;
    error(L"\nChecked failed! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::CANOPEN)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::UNDEFINED)));
  }

  //verify request information for UNDEFINE & UNDEFINED & UNDEFINED
  actualResult = AxisProfileFactory::checkRequestedProfile(AxisProfileType::UNDEFINED, DriveProfileType::UNDEFINED, NetworkTechnology::UNDEFINED);
  expectResult = AxisProfileResult::ERR_NOTIMPLEMENTED;
  if ( actualResult != expectResult )
  {
    testResult = E_FAIL;
    error(L"\nChecked failed! AxisProfileType: %ls DriveProfileType: %ls NetworkTechnology: %ls", UTF8TOWIDE(axisProfileTypeToString(AxisProfileType::UNDEFINED)),
          UTF8TOWIDE(driveProfileTypeToString(DriveProfileType::UNDEFINED)), UTF8TOWIDE(networkTechnologyToString(NetworkTechnology::UNDEFINED)));
  }

  return testResult;
}
