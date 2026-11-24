#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

#include "mockup/external_bundles/mock_datalayer_client.h"
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

/// @brief Test for the sercos probe class
class TestSercosProbe : public xTST_Test2
{
public: // types
  struct TestDataVariantU16
  {
    const wchar_t *m_name;          ///< name of the variant type
    std::uint16_t m_data;                ///< data to write in the variant
    bool m_addData;                 ///< add data to variant?
  };
  
public:
  /// @brief Constructor
  TestSercosProbe() = default;
  /// @brief Destructor
  virtual ~TestSercosProbe() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.Probe");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile Probe class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JS);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_ML);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private:  //test methods

  /// @brief   This method test the function probeBaseConfig()
  /// @return       S_OK, if test was successful 
  HRESULT TestProbeBaseConfig();

  /// @brief   This method test the function probeExtConfig()
  /// @return       S_OK, if test was successful 
  HRESULT TestProbeExtConfig();

  /// @brief   This method test the function probeStatusCtrl()
  /// @return       S_OK, if test was successful 
  HRESULT TestProbeStatusCtrl();

  /// @brief   This method test the function onlineChangeParams()
  /// @return       S_OK, if test was successful 
  HRESULT TestOnlineChangeParams();

  /// @brief   This method test the function getProbeValues()
  /// @return       S_OK, if test was successful 
  HRESULT TestGetProbeValues();

  /// @brief   This method test the function getProbeValuesRT()
  /// @return       S_OK, if test was successful 
  HRESULT TestGetProbeValuesRT();

  /// @brief   This method test the functions failed situations
  /// @return       S_OK, if test was successful 
  HRESULT TestProbeCmdFailed();

  /// @brief   This method test the functions of signal convert
  /// @return       S_OK, if test was successful 
  HRESULT TestMeasuredSignalConvert();

  /// @brief   This method test the functions of is probe supported
  /// @return       S_OK, if test was successful 
  HRESULT TestisProbeSupported();
  
  /// @brief   This method is the simple tests.
  /// @return       S_OK, if test was successful 
  HRESULT SimpleTests();

  //! @brief   This method sets a test variant for the client response (async calls).
  //! @param[in]    client      data layer client to execute datalayer access
  //! @param[in]    data        test data for the variant
  void setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16 &data);

private: //data

  const std::string m_profileName = "testProfile";   ///< Name of the test axis profile
};
