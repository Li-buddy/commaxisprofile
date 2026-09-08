#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"

// Test Class
#include "profiles/category/drive/axisprofile_cyclic_data_drive.h"
#include "profiles/category/category_adapter_factory.h"
#include "profiles/value_mapping/mapping_handler.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestProfileCyclicData
//! Test class for the profile cyclic data base class.
class TestProfileCyclicData : public xTST_Test2
{
public:
  //! Constructor
  TestProfileCyclicData();
  //! Destructor
  virtual ~TestProfileCyclicData() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.CyclicData");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile cyclic data base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  //! @brief Test the command C-Interface.
  //! @return  S_OK, when tests were successful
  HRESULT testCInterface() const;

  /// @brief Test the check method for telegram assign 
  /// @return S_OK, when tests were successful
  HRESULT testCheckIfTelegramItemAssigned() const;

  /// @brief Test for the check method of master status
  /// @return S_OK, when tests were successful
  HRESULT testForcheckMasterStatus() const;

  /// @brief Test for the check method of slave status
  /// @return S_OK, when tests were successful
  HRESULT testForcheckSlaveStatus() const;

  /// @brief Test for the check method of actual value availability
  /// @return S_OK, when tests were successful
  HRESULT testForCheckActualValueAvailability() const;

private: // Data

  std::string m_profileName = "testProfile";     //!< Profile name used in this test

private: // Types

  //! @class AxisProfileCyclicDataUnittest
  //! Specialization of the test class just to overwrite the pure virtual methods and make it creatable.
  class AxisProfileCyclicDataUnittest : public comm::axisprofile::AxisProfileCyclicDataDrive
  {
  public: // Methods
    comm::axisprofile::IScaling* pMockScaling = nullptr;
    explicit AxisProfileCyclicDataUnittest(const comm::axisprofile::ProfileBasicInfo& profileBasicInfo,
                                           comm::axisprofile::AcknowledgeTelegram* const at,
                                           comm::axisprofile::MasterDataTelegram* const mdt)
      : AxisProfileCyclicDataDrive(profileBasicInfo, at, mdt)
    {

    }
    virtual ~AxisProfileCyclicDataUnittest() = default;

    bool checkIfTelegramItemAssigned(comm::axisprofile::AxisProfileValueTypeID valueTypeID, comm::axisprofile::DataAccessErrorInfo& errorInfo)
    {
      return AxisProfileCyclicDataDrive::checkIfTelegramItemAssigned(valueTypeID, errorInfo);
    }
    bool checkFieldbusStatus(comm::axisprofile::DataAccessErrorInfo& errorInfo)
    {
      return AxisProfileCyclicDataDrive::checkFieldbusStatus(errorInfo);
    }
    bool checkForSlaveStatus(comm::axisprofile::DataAccessErrorInfo& errorInfo)
    {
      return AxisProfileCyclicDataDrive::checkForSlaveStatus(errorInfo);
    }
    void setHandler(comm::axisprofile::IAxisProfileSharingHandler* pSharingHandler)
    {
      AxisProfileCyclicDataDrive::setSharingHandler(pSharingHandler);
    }

    AxisProfileResult getActualValuesDevice(ActualValuesDevice* const /*actualValues*/)
    {
      return AxisProfileResult::STS_OK;
    }
        
    AxisProfileResult setCommandValuesDevice(CommandValuesDevice /*commandValues*/)
    {
      return AxisProfileResult::STS_OK;
    }

    void loadBootingTasks() override {};

    AxisProfileResult booting() override {return AxisProfileResult::STS_OK;}
  };
};
