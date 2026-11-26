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
/// @brief This class implements the signal function handler for probe signals.
class ProbeSignalFuncHandler : public SignalFuncHandler
{
public:
  explicit ProbeSignalFuncHandler(ProfileBasicInfo& profileBasicInfo)
    : SignalFuncHandler(profileBasicInfo)
  {
    prepareSignalWordInfo(profileBasicInfo.m_profileCategory);
  }

  void registDriveSignalWordFuncs() override
  {
    auto probe1Validator = [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
      {
        return param == sercos::S_0_0405_ENABLE_PROBE_1 && bit == 0;
      };
    auto probe2Validator = [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
      {
        return param == sercos::S_0_0406_ENABLE_PROBE_2 && bit == 0;
      };

    registerSignalStatusWordFuncs(FunctionSupport::PROBE1_ENABLE_CYCLIC, probe1Validator);
    registerSignalControlWordFuncs(FunctionSupport::PROBE1_ENABLE_CYCLIC, probe1Validator);
    registerSignalStatusWordFuncs(FunctionSupport::PROBE2_ENABLE_CYCLIC, probe2Validator);
    registerSignalControlWordFuncs(FunctionSupport::PROBE2_ENABLE_CYCLIC, probe2Validator);
  }

  void registEncoderSignalWordFuncs() override
  {

  }

  void setProbeSignalsSupported(bool supported)
  {
    m_supported = supported;
  }

protected:
  AxisProfileResult readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params,
                                                 std::vector<std::uint16_t>& assignBits) override
  {
    setSignalWordCfg(params, assignBits);
    return AxisProfileResult::STS_OK;
  }

  AxisProfileResult readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params,
                                                  std::vector<std::uint16_t>& assignBits) override
  {
    setSignalWordCfg(params, assignBits);
    return AxisProfileResult::STS_OK;
  }

private:
  void setSignalWordCfg(std::vector<std::uint32_t>& params, std::vector<std::uint16_t>& assignBits) const
  {
    if (m_supported)
    {
      params = {sercos::S_0_0405_ENABLE_PROBE_1, sercos::S_0_0406_ENABLE_PROBE_2};
      assignBits = {0, 0};
    }
    else
    {
      params = {sercos::S_0_0000_DUMMY};
      assignBits = {0};
    }
  }

  bool m_supported {true};
};

/// @brief This class implements a mock device parameter class for testing the probe signal function handler.
class ProbeSignalFuncDeviceParam : public test::MockAxisProfileDeviceParam
{
public:
  void setSignalFuncHandler(SignalFuncHandler* handler)
  {
    m_signalFuncHandler = handler;
  }

  SignalFuncHandler* getSignalFuncHandler() override
  {
    return m_signalFuncHandler;
  }

private:
  SignalFuncHandler* m_signalFuncHandler {};
};

/// @brief This class implements a mock cyclic data class for testing the probe signal function handler.
class RecordingProbeCyclicData : public test::MockAxisProfileCyclicData
{
public:
  AxisProfileResult activeSignalFunc(std::uint8_t bitOffset) override
  {
    ++m_activeCallCount;
    m_lastActiveBitOffset = bitOffset;
    return m_activeResult;
  }

  AxisProfileResult resetSignalFuncBit(std::uint8_t bitOffset) override
  {
    ++m_resetCallCount;
    m_lastResetBitOffset = bitOffset;
    return m_resetResult;
  }

  AxisProfileResult getSignalFuncState(std::uint8_t bitOffset, bool& value) override
  {
    ++m_stateCallCount;
    m_lastStateBitOffset = bitOffset;
    value = m_signalState;
    return m_stateResult;
  }

  std::uint16_t m_activeCallCount {};
  std::uint16_t m_resetCallCount {};
  std::uint16_t m_stateCallCount {};
  std::uint8_t m_lastActiveBitOffset {};
  std::uint8_t m_lastResetBitOffset {};
  std::uint8_t m_lastStateBitOffset {};
  bool m_signalState {false};
  AxisProfileResult m_activeResult {AxisProfileResult::STS_OK};
  AxisProfileResult m_resetResult {AxisProfileResult::STS_OK};
  AxisProfileResult m_stateResult {AxisProfileResult::STS_OK};
};

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
    const wchar_t* m_name;          ///< name of the variant type
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

  /// @brief   This method test the signal function path of probeStatusCtrl()
  /// @return       S_OK, if test was successful 
  HRESULT TestProbeStatusCtrlSignalFunction();

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
  void setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16& data);

private: //data

  const std::string m_profileName = "testProfile";   ///< Name of the test axis profile
};
