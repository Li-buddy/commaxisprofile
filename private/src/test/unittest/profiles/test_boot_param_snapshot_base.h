#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test for the boot parameter snapshot base class.
class TestBootParamSnapshotBase : public xTST_Test2
{
public: // methods

  /// @brief Constructor
  TestBootParamSnapshotBase() = default;
  /// @brief Destructor
  virtual ~TestBootParamSnapshotBase() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.BootParmSnapshotBase");
  TEST_DECLARE_DESCRIPTION("Unittest of the boot parameter snapshot base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief Main test function for running all tests.
  /// @return S_OK when tests were successful.
  HRESULT doTest() override;

private: // methods

  /// @brief Verify drive reads update snapshot contents but do not implicitly enable cache reuse.
  /// @return S_OK, when the test was successful
  HRESULT testDriveReadDoesNotImplicitlyEnableCache();

  /// @brief Verify read request generation failures are reported as invalid arguments.
  /// @return S_OK, when the test was successful
  HRESULT testGenerateReqDataFailure();

  /// @brief Verify unknown descriptor value types are rejected after a successful drive read.
  /// @return S_OK, when the test was successful
  HRESULT testUnsupportedDescriptorType();

  /// @brief Verify byte-array reads use the variable-length path and persist only the actual read length.
  /// @return S_OK, when the test was successful
  HRESULT testByteArrayReadUsesActualReadLength();
};
