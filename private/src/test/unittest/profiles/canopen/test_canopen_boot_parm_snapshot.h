#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test for the Canopen boot parameter snapshot class.
class TestCanopenBootParmSnapshot : public xTST_Test2
{
public: // methods

  /// @brief Constructor
  TestCanopenBootParmSnapshot() = default;
  /// @brief Destructor
  virtual ~TestCanopenBootParmSnapshot() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.BootParmSnapshot");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen boot parameter snapshot class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief Main test function for running all tests.
  /// @return S_OK when tests were successful.
  HRESULT doTest() override;

private: // methods

  /// @brief Verify reading Canopen parameters from drive succeeds and persists values into snapshot.
  /// @return S_OK, when the test was successful
  HRESULT testGetParameterReadFromDriveAndPersist();

  /// @brief Verify reading Canopen parameter from configured snapshot cache succeeds.
  /// @return S_OK, when the test was successful
  HRESULT testGetParameterReadFromSnapshotCache();

  /// @brief Verify forcing drive read bypasses cache and refreshes persisted value.
  /// @return S_OK, when the test was successful
  HRESULT testGetParameterForceReadFromDrive();

  /// @brief Verify validation and error paths of getParameter and snapshot protocol checks.
  /// @return S_OK, when the test was successful
  HRESULT testGetParameterValidationAndErrorPaths();

  /// @brief Verify getParameterValueFromSnapshot null checks return ERR_EXEC_NULL.
  /// @return S_OK, when the test was successful
  HRESULT testGetParameterValueFromSnapshotNullChecks();

  /// @brief Verify generateReqData returns nullptr for non-Canopen BootParamId.
  /// @return S_OK, when the test was successful
  HRESULT testGenerateReqDataNullForInvalidBootParamId();
};
