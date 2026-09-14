#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test for the Sercos boot parameter snapshot class.
class TestSercosBootParmSnapshot : public xTST_Test2
{
public: // methods

  /// Constructor
  TestSercosBootParmSnapshot() = default;
  /// Destructor
  virtual ~TestSercosBootParmSnapshot() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.BootParmSnapshot");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos boot parameter snapshot class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_WSY);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  /// @brief Main test function for running all tests.
  /// @return S_OK when tests were successful.
  HRESULT doTest() override;

private: // methods

  /// @brief Verify reading parameters from drive succeeds and persists values into the snapshot.
  /// @return  S_OK, when the test was successful
  HRESULT testGetParameterReadFromDriveAndPersist();

  /// @brief Verify reading parameters from configured snapshot cache succeeds.
  /// @return  S_OK, when the test was successful
  HRESULT testGetParameterReadFromSnapshotCache();

  /// @brief Verify forcing drive read bypasses cache and refreshes persisted value.
  /// @return  S_OK, when the test was successful
  HRESULT testGetParameterForceReadFromDrive();

  /// @brief Verify validation and error paths of getParameter and snapshot protocol checks.
  /// @return  S_OK, when the test was successful
  HRESULT testGetParameterValidationAndErrorPaths();

  /// @brief Verify SoE element flag parameter is forwarded into fieldbus read requests.
  /// @return  S_OK, when the test was successful
  HRESULT testGetParameterElementFlagForwarding();

  /// @brief Verify default IndraDrive boot parameter list matches descriptor table filtering.
  /// @return  S_OK, when the test was successful
  HRESULT testGetSoeIndraDriveBootParamList();
};
