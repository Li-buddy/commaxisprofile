#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mockup/mock_profile_trace.h"
#include "profiles/category/drive/axisprofile_probe_drive.h"
#include "profiles/profile_defs.h"

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

/// @brief Test class for the profile probe base class.
class TestProfileProbe : public xTST_Test2
{
public:
  /// @brief Constructor
  explicit TestProfileProbe() = default;

  /// @brief Destructor
  virtual ~TestProfileProbe() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Probe");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile probe base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JS);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_ML);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  /// @brief Test the command C-Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testCInterface();

  /// @brief Test miscellaneous
  /// @return  S_OK, when tests were successful
  HRESULT testMisc();

private: // Data

  std::uint32_t m_deviceAddress = 0;             ///< Device address used in this test
  std::string m_profileName = "testProfile";     ///< Profile name used in this test

  /// @brief AxisProfileProbeUnittest
  /// Specialization of the test class just to overwrite the pure virtual methods and make it creatable. 
  class AxisProfileProbeUnittest : public comm::axisprofile::AxisProfileProbeDrive
  {
  public: //Methods

    explicit AxisProfileProbeUnittest(comm::axisprofile::ProfileBasicInfo& profileBasicInfo)
    : comm::axisprofile::AxisProfileProbeDrive(profileBasicInfo) {};

    virtual ~AxisProfileProbeUnittest() = default;
   
    comm::axisprofile::AxisProfileResult probeBaseConfig(comm::axisprofile::ProbeIndex /*probeIdx*/, comm::axisprofile::ProbeBaseConfig /*probeConfig*/, bool /*isAutoActivated*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult probeExtConfig(comm::axisprofile::ProbeIndex /*probeIdx*/, comm::axisprofile::ProbeExtConfig /*probeExtendConfig*/, bool /*isStateMonitActivated*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult probeStatusCtrl(comm::axisprofile::ProbeIndex /*probeIdx*/, bool /*enable*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult getProbeActualValuesRT(comm::axisprofile::ProbeIndex /*probeIdx*/, comm::axisprofile::ProbeActualValues* /*actualValueProbe*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult onlineChangeParams(comm::axisprofile::ProbeIndex /*probeIdx*/, comm::axisprofile::ProbeUpdateMeasurement /*updateValue*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult getProbeValues(comm::axisprofile::ProbeIndex /*probeIdx*/, comm::axisprofile::ProbeBaseConfig* /*probeConfig*/, comm::axisprofile::ProbeExtConfig* /*probeExtendConfig*/) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult abortAndResetProbe() override { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult isProbeSupported() override { return comm::axisprofile::AxisProfileResult::STS_OK; }
    void loadBootingTasks() override {}
  };
};
