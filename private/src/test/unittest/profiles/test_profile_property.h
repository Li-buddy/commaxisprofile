#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "mockup/mock_profile_trace.h"
#include "profiles/axisprofile_property.h"
#include "mockup/mock_axisprofile_category.h"


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

/// @brief TestProfileProperty
/// @brief Test class for the profile property base class.
class TestProfileProperty : public xTST_Test2
{
public:
  /// @brief Constructor
  explicit TestProfileProperty() = default;

  /// @brief Destructor
  virtual ~TestProfileProperty() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Property");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile property base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LQ);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LQ);

  HRESULT doTest();

private: // Methods

  /// @brief Test the command C-Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testCInterface();

  /// @brief   Test the axisprofile base class.
  /// @return  S_OK, when tests were successful
  HRESULT testClass();

public: //Data
  std::unique_ptr<::comm::axisprofile::AxisProfileProperty> m_testProperty {};  ///< pointer to test property

private: // Data

  std::uint32_t m_deviceAddress = 0;                  ///< Device address used in this test
  std::string m_profileName = "testProfile";          ///< Profile name used in this test
  comm::axisprofile::AxisProfileType m_testProfileType = comm::axisprofile::AxisProfileType::UNDEFINED;
  comm::axisprofile::DriveProfileType m_testDriveProfileType = comm::axisprofile::DriveProfileType::SERCOS;      ///< Drive profile type used in test
  comm::axisprofile::NetworkTechnology m_testNetworkTechnology = comm::axisprofile::NetworkTechnology::UNDEFINED;
};
