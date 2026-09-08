#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "manager/axisprofile_manager.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    class IAxisProfileManagerInternal;
  }
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------


/// @class TestAxisProfileManager
/// Test class for the axisprofile manager
class TestAxisProfileManager : public xTST_Test2
{
public:
  /// ctor
  TestAxisProfileManager() noexcept = default;

  /// Destructor
  virtual ~TestAxisProfileManager() noexcept = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Manager");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile manager.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_DS);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods
  /// @brief   this test validates the initialization of the axisprofile manager
  /// @return  S_OK, when tests were successful
  HRESULT testManagerFieldbusMgrInit();

  /// @brief   this test validates the initialization of the axisprofile manager
  /// @return  S_OK, when tests were successful
  HRESULT testManagerInit();

  /// @brief   this test validates the initialization of the axisprofile manager memory
  /// @return  S_OK, when tests were successful
  HRESULT testMemoryInit();

  /// @brief   this test validates the initialization of the axisprofile manager mapping
  /// @return  S_OK, when tests were successful
  HRESULT testMappingInit();

  /// @brief   this test validates the initialization of the axisprofile manager mapping
  /// @return  S_OK, when tests were successful
  HRESULT testFieldMgrInit();

  /// @brief   this test will collect all necessary master and slave device information.
  /// @return  S_OK, when tests were successful
  HRESULT testForCollectAllDeviceInfo();

  /// @brief   this test validates the creation and destruction of axisprofiles via the axisprofile manager
  /// @return  S_OK, when tests were successful
  HRESULT testAxisProfiles();

  /// @brief   this test validates the processing of the cyclic at data
  /// @return  S_OK, when tests were successful
  HRESULT testProcessDataAt();

  /// @brief   this test validates the processing of the cyclic mdt data
  /// @return  S_OK, when tests were successful
  HRESULT testProcessDataMdt();

  /// @brief   this test checks errorcases for the simple class methods
  /// @return  S_OK, when tests were successful
  HRESULT testMethodErrorCases();

  /// @brief   this method adds a profile to the given manager
  /// @param[in]  testManager       pointer to the manager, the profile is added
  /// @param[in]  deviceAddress     adress of the connected device
  /// @param[in]  profileName       name of the profile to add
  /// @param[in]  inputAddress      adress of input memory in dl-rt
  /// @param[in]  outputAddress     adress of output memory in dl-rt
  /// @return  S_OK, when tests were successful
  HRESULT addProfileToManager(comm::axisprofile::IAxisProfileManagerInternal* testManager, std::uint32_t deviceAddress, const char* profileName, const char* inputAddress, const char* outputAdress);

  /// @brief Small helper method to print the result of the current test case.
  /// @param[in]   result   Result code of the test case.
  inline void printTestCaseResult(HRESULT result);

  /// @brief  processAuxJobs test case.
  /// @param[in]   result   Result code of the test case.
  HRESULT testProcessAuxJobs();

  /// @brief Test of method validateMappingCfg
  HRESULT testValidateMappingCfg();

  /// @brief  Test case of executing external checking for profiles
  /// @param[in]   result   Result code of the test case.
  HRESULT testExecExternalChecking();

private: // data

  std::string m_profileName = "testProfile";   ///< Profile name used in test
  bool m_abortBoot{};                          ///< Variable used to abort the boot procedure
};

