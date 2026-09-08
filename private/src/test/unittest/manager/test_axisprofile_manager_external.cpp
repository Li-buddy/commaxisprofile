#include <memory>
#include "test_axisprofile_manager_external.h"

// Mockups
#include "mockup/mock_axisprofile.h"

// Test Class
#include "manager/axisprofile_manager.h"
#include "helper/result_helper.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileManagerExternal);
REGISTER_TEST_TO_GROUP(TestAxisProfileManagerExternal, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManagerExternal, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileManagerExternal, Manager);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestAxisProfileManagerExternal::TestAxisProfileManagerExternal()
{
}

TestAxisProfileManagerExternal::~TestAxisProfileManagerExternal(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfileManagerExternal::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the external axisprofile manager interface");
  log(L"\n----------------------------------------------------------------");

  HRESULT hresult = S_OK;

  log(L"\n");
  hresult |= testCreateProfile();
  hresult |= testDeleteProfile();
  hresult |= testConnectProfile();
  hresult |= testMiscellaneous();

  return hresult;
}

HRESULT TestAxisProfileManagerExternal::testCreateProfile()
{
  HRESULT result = S_OK;
  AxisProfileManager axisProfileManager{};
  log(L"\n<b>Test of the interface method 'createAxisProfile()'</b>");

  log(L"\nCall createAxisProfile() with empty name...");
  comm::axisprofile::AxisProfileResult expectedResult = comm::axisprofile::AxisProfileResult::ERR_INVALID_ARGUMENT;
  comm::axisprofile::AxisProfileResult aprResult = axisProfileManager.createAxisProfile(comm::axisprofile::AxisProfileType::UNDEFINED, "");
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nAdd profile with name %ls...", UTF8TOWIDE(m_profileName));
  axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfile = new test::MockAxisProfile(m_profileName, comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT);

  log(L"\nCall createAxisProfile() with name which already exists...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_ALREADY_EXISTS;
  aprResult = axisProfileManager.createAxisProfile(comm::axisprofile::AxisProfileType::UNDEFINED, m_profileName);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall createAxisProfile() with undefined profile type...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_NOTIMPLEMENTED;
  aprResult = axisProfileManager.createAxisProfile(comm::axisprofile::AxisProfileType::UNDEFINED, m_profileName1);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall createAxisProfile() when everything is OK...");
  expectedResult = comm::axisprofile::AxisProfileResult::STS_OK;
  aprResult = axisProfileManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName1);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCheck if profile exists...");
  auto it = axisProfileManager.m_mapAxisProfiles.find(m_profileName1);
  if (it == axisProfileManager.m_mapAxisProfiles.end())
  {
    error(L"\nCould not find previously created profile!");
    result = E_FAIL;
  }

  log(L"\nDelete existing profiles.");
  delete axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfile;
  axisProfileManager.m_mapAxisProfiles.erase(m_profileName);
  delete axisProfileManager.m_mapAxisProfiles[m_profileName1].m_axisProfile;
  axisProfileManager.m_mapAxisProfiles.erase(m_profileName1);

  log(L"\nAdd the maximum amount of elements (%d) to the axisprofile map in the manager.", MAX_NUMBER_OF_PROFILES);
  for (std::uint32_t i = 0; i < MAX_NUMBER_OF_PROFILES; i++)
  {
    std::string profileName = m_profileName + std::to_string(i);
    axisProfileManager.m_mapAxisProfiles[profileName];
  }

  log(L"\nCall createAxisProfile() and try to create more profiles than allowed...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_MAX_NUMBER_PROFILES;
  aprResult = axisProfileManager.createAxisProfile(comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestAxisProfileManagerExternal::testDeleteProfile()
{
  HRESULT result = S_OK;
  AxisProfileManager axisProfileManager{};
  log(L"\n<b>Test of the interface method 'deleteAxisProfile()'</b>");

  log(L"\nCall deleteAxisProfile() with empty name...");
  comm::axisprofile::AxisProfileResult expectedResult = comm::axisprofile::AxisProfileResult::ERR_INVALID_ARGUMENT;
  comm::axisprofile::AxisProfileResult aprResult = axisProfileManager.deleteAxisProfile("");
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall deleteAxisProfile() with profile name that does not exist...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  aprResult = axisProfileManager.deleteAxisProfile(m_profileName);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nAdd profile with name %ls...", UTF8TOWIDE(m_profileName));
  axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfileShared.reset(new IAxisProfileC());
  std::shared_ptr<const IAxisProfileC> user = axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfileShared;

  log(L"\nCall deleteAxisProfile() when profile is already used...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_IN_USE;
  aprResult = axisProfileManager.deleteAxisProfile(m_profileName);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall deleteAxisProfile() when everything is OK...");
  user = nullptr;
  expectedResult = comm::axisprofile::AxisProfileResult::STS_OK;
  aprResult = axisProfileManager.deleteAxisProfile(m_profileName);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCheck if profile has been deleted...");
  auto it = axisProfileManager.m_mapAxisProfiles.find(m_profileName);
  if (it != axisProfileManager.m_mapAxisProfiles.end())
  {
    error(L"\nDeleted profile is still present!");
    result = E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestAxisProfileManagerExternal::testConnectProfile()
{
  HRESULT result = S_OK;
  AxisProfileManager axisProfileManager{};
  log(L"\n<b>Test of the interface method 'connectAxisProfile()'</b>");

  log(L"\nCall connectAxisProfile() with empty name...");
  std::shared_ptr<const IAxisProfileC> user;
  comm::axisprofile::AxisProfileResult expectedResult = comm::axisprofile::AxisProfileResult::ERR_INVALID_ARGUMENT;
  comm::axisprofile::AxisProfileResult aprResult = axisProfileManager.connectAxisProfile("", user);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall connectAxisProfile() with no profile...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_CONFIG;
  aprResult = axisProfileManager.connectAxisProfile(m_profileName, user);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }
  
  log(L"\nAdd profile with name %ls...", UTF8TOWIDE(m_profileName));
  axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfileShared.reset(new IAxisProfileC());
  user = axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfileShared;

  log(L"\nCall connectAxisProfile() with profile name that does not exist...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_NOT_FOUND;
  aprResult = axisProfileManager.connectAxisProfile(m_profileName1, user);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall connectAxisProfile() when profile is already used...");
  expectedResult = comm::axisprofile::AxisProfileResult::ERR_PROFILE_IN_USE;
  aprResult = axisProfileManager.connectAxisProfile(m_profileName, user);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have failed. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCall connectAxisProfile() when everything is OK...");
  user = nullptr;
  expectedResult = comm::axisprofile::AxisProfileResult::STS_OK;
  aprResult = axisProfileManager.connectAxisProfile(m_profileName, user);
  if (aprResult != expectedResult)
  {
    error(L"\nMethod should have succeeded. Expected: 0x%X, Actual: 0x%X", expectedResult, result);
    result = E_FAIL;
  }

  log(L"\nCheck if profile is marked as 'connected' in the AxisProfileManager...");
  auto it = axisProfileManager.m_mapAxisProfiles.find(m_profileName);
  if (it == axisProfileManager.m_mapAxisProfiles.end())
  {
    error(L"\nProfile could not be found!");
    result = E_FAIL;
  }
  else if (it->second.m_axisProfileShared.use_count() != 2)
  {
    error(L"\nProfile has wrong use count! Expected: 2, Actual: %d", it->second.m_axisProfileShared.use_count());
    result = E_FAIL;
  }

  log(L"\n");
  return result;
}

HRESULT TestAxisProfileManagerExternal::testMiscellaneous()
{
  HRESULT result = S_OK;
  AxisProfileManager axisProfileManager{};
  log(L"\n<b>Test of miscellaneous external interface methods</b>");

  log(L"\nCall profileExists() when profile does not exist...");
  bool profileExists = axisProfileManager.profileExists(m_profileName);
  if (profileExists)
  {
    error(L"\nMethod returned 'true' but should have returned 'false'!");
    result = E_FAIL;
  }

  log(L"\nAdd profile with name %ls...", UTF8TOWIDE(m_profileName));
  axisProfileManager.m_mapAxisProfiles[m_profileName].m_axisProfile = new test::MockAxisProfile(m_profileName, comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT);

  log(L"\nCall profileExists() when profile exists...");
  profileExists = axisProfileManager.profileExists(m_profileName);
  if (!profileExists)
  {
    error(L"\nMethod returned 'false' but should have returned 'true'!");
    result = E_FAIL;
  }

  log(L"\nMethods to register/unregister external checking function for profiles...");
  auto expectedResult = comm::axisprofile::AxisProfileResult::STS_OK;
  auto checkProfile = [](const std::string& /*profileName*/) {return CheckResponse::OK_USE_PROFILE;};
  auto aprResult = axisProfileManager.registerCheckCallback(checkProfile, "testBundle");
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", aprResult);

  expectedResult = comm::axisprofile::AxisProfileResult::STS_OK;
  aprResult = axisProfileManager.unregisterAllCheckCallbacks("testBundle");
  LOG_TEST_RESULT(expectedResult, L"Method should have succeeded.", aprResult);

  log(L"\n");
  return result;
}
