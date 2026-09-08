#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "comm/datalayer/datalayer.h"
#include "profiles/profile_defs.h"
#include "profiles/scaling/i_scaling.h"
#include "stdafx.h"
#include "xTST_Test.h"
// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace test
{
  class MockDatalayerClient;
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestCanopenCommand
//! Test class for can commands of in the axisprofile.
class TestCanopenCommand : public xTST_Test2
{
public: // types

  //! @struct TestDataVariantU16
  //! @brief This struct contains the test data for a variant that is returned by the datalayer
  struct TestDataVariantU16
  {
    const wchar_t *m_name;          //!< name of the variant type
    std::uint32_t m_data;                //!< data to write in the variant
    bool m_addData;                 //!< add data to variant?
  };

  /// @brief Struct test mock objects for test
  struct TestCmdMockObj
  {
    comm::datalayer::IClient* m_client;                 ///< datalayer client
    comm::axisprofile::IScaling* m_scaling;             ///< scaling
    comm::axisprofile::AcknowledgeTelegram m_at;        ///< at
    comm::axisprofile::MasterDataTelegram m_mdt;        ///< mdt
    comm::axisprofile::ProfileBasicInfo m_profileInfo;  ///< profile basic information
  };

public: // methods

  //! Constructor
  TestCanopenCommand() = default;
  //! Destructor
  virtual ~TestCanopenCommand() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.Command");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen profile command class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JS);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_XQF);

  
  HRESULT doInit() override;
  HRESULT doTest() override;
  HRESULT doEnd() override;

private: // method

  //! @brief    Test miscellaneous
  //! @return   S_OK, if test was successful 
  HRESULT testMiscellaneous();

  //! @brief   This test validates the clear error functionality.
  //! @return   S_OK, if test was successful 
  HRESULT testSimpleCommands();

  //! @brief   Test rising and timeout error case of all commands
  //! @return   S_OK, if test was successful 
  HRESULT testRisingAndTimeoutError();
  
  //! @brief   This test validates the referencingAxis functionality.
  //! @return   S_OK, if test was successful 
  HRESULT testReferencingCommand();

  //! @brief   This method sets a test variant for the client response (async calls).
  //! @param[in]    client      data layer client to execute datalayer access
  //! @param[in]    data        test data for the variant
  void setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16 &data);

  /// @brief   This test validates the commands assigned op mode and op mode display functionality.
  /// @return   S_OK, if test was successful 
  HRESULT testCommandAssignedOpMode();

  /// @brief   Test setReferencePosition
  /// @return   S_OK, if test was successful 
  HRESULT testSetReferencingPosition();

  /// @brief   Test aux jobs
  /// @return   S_OK, if test was successful 
  HRESULT testAuxJobsHandling();

  /// @brief   Test command check if drive halted
  /// @return   S_OK, if test was successful 
  HRESULT testCheckIfDriveHalted();

private: // data

  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
  bool m_specialStepFlag = false;                    //!< Test case jumps into a special step
  TestCmdMockObj m_mockObjs {};                      //!< Mockup objects for test of the command
};
