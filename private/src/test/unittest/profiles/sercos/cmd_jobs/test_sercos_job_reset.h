#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/signal_func/signal_func_handler.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/cmd_jobs/sercos_job_reset.h"
//Mockups
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "profiles/axisprofile_command_statemachine.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

using namespace comm::axisprofile;

class TestableSercosJobReset : public sercos::SercosJobReset
{
public:
  using sercos::SercosJobReset::SercosJobReset;
  using sercos::SercosJobReset::onJobAborting;
  using sercos::SercosJobReset::onJobActivating;
  using sercos::SercosJobReset::onJobEnterActivating;
  using sercos::SercosJobReset::onUpdateStatusInDevice;

  AsyncCommandStatemachine& asyncStateMachine()
  {
    return m_asyncSm;
  }

  JobDataDeviceStatus& deviceStatus()
  {
    return m_deviceStatus;
  }

  bool isRTCmd() const
  {
    return m_isSignalFuncCmd;
  }
};

class RecordingCyclicData : public test::MockAxisProfileCyclicData
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
  bool m_signalState {true};
  AxisProfileResult m_activeResult {AxisProfileResult::STS_OK};
  AxisProfileResult m_resetResult {AxisProfileResult::STS_OK};
  AxisProfileResult m_stateResult {AxisProfileResult::STS_OK};
};

class TestSignalFuncMgr : public SignalFuncHandler
{
public:
  explicit TestSignalFuncMgr(ProfileBasicInfo& profileBasicInfo)
    : SignalFuncHandler(profileBasicInfo)
  {
    auto resetCmdValidator = [] (const std::uint32_t& param, const std::uint16_t& bit) -> bool
      {
        return param == sercos::S_0_0099_RESET_CLASS_1_DIAGNOSTIC && bit == 0;
      };
    registerSignalControlWordFuncs(FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC, resetCmdValidator);
    registerSignalStatusWordFuncs(FunctionSupport::CLEAR_DEVERR_CMD_CYCLIC, resetCmdValidator);
  }

  void setSignalFuncSupported(bool supported)
  {
    m_supported = supported;
  }

protected:
  AxisProfileResult readDriveSignalStatusWordCfg(std::vector<std::uint32_t>& params,
                                            std::vector<std::uint16_t>& assignBits) override
  {
    params = {m_supported ? sercos::S_0_0099_RESET_CLASS_1_DIAGNOSTIC : sercos::S_0_0000_DUMMY};
    assignBits = {0};
    return AxisProfileResult::STS_OK;
  }

  AxisProfileResult readDriveSignalControlWordCfg(std::vector<std::uint32_t>& params,
                                              std::vector<std::uint16_t>& assignBits) override
  {
    params = {m_supported ? sercos::S_0_0099_RESET_CLASS_1_DIAGNOSTIC : sercos::S_0_0000_DUMMY};
    assignBits = {0};
    return AxisProfileResult::STS_OK;
  }

private:
  bool m_supported {true};
};

class SignalFuncDeviceParam : public test::MockAxisProfileDeviceParam
{
public:
  void setSignalFuncMgr(SignalFuncHandler* signalFuncMgr)
  {
    m_signalFuncHandler = signalFuncMgr;
  }

  SignalFuncHandler* getSignalFuncHandler() override
  {
    return m_signalFuncHandler;
  }

private:
  SignalFuncHandler* m_signalFuncHandler {};
};

inline const ProfileBasicInfo createProfileBasicInfo(const test::MockAxisProfileTrace& profileTrace)
{
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_profileName = "testProfile";
  profileBasicInfo.m_profileTrace = profileTrace;
  return profileBasicInfo;
}

inline void setSignalWordMapping(test::MockAxisProfileMappingHandler& mappingHandler, bool assigned)
{
  auto& profileAssignedInfo = mappingHandler.getProfileAssignedInfo();
  profileAssignedInfo.m_input.m_signalStatusWord = assigned;
  profileAssignedInfo.m_output.m_signalControlWord = assigned;
}

struct RTTestContext
{
  explicit RTTestContext(test::MockAxisProfileTrace& profileTrace)
    : profileBasicInfo(createProfileBasicInfo(profileTrace)), signalFuncMgr(profileBasicInfo)
  {}

  ProfileBasicInfo profileBasicInfo;
  TestSignalFuncMgr signalFuncMgr;
  SignalFuncDeviceParam deviceParam;
  RecordingCyclicData cyclicData;
};

inline AxisProfileResult prepareRTTestContext(IAxisProfileSharingHandler* sharingHandler,
                                              test::MockAxisProfileMappingHandler& mappingHandler,
                                              RTTestContext& testContext,
                                              bool signalFuncSupported)
{
  setSignalWordMapping(mappingHandler, true);
  testContext.signalFuncMgr.setSignalFuncSupported(signalFuncSupported);
  testContext.deviceParam.setSignalFuncMgr(&testContext.signalFuncMgr);
  sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandler);
  sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &testContext.deviceParam);
  sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &testContext.cyclicData);
  return testContext.signalFuncMgr.init(sharingHandler);
}

inline std::uint16_t commandStepValue(AsyncCommandStatemachine::CommandStep commandStep)
{
  return static_cast<std::uint16_t>(commandStep);
}

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test for the Sercos JobReset class
class TestSercosJobReset : public xTST_Test2
{

public:
  /// @brief Constructor
  TestSercosJobReset() = default;
  /// @brief Destructor
  virtual ~TestSercosJobReset()noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.cmdJobs.Reset");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile cmdJobs Reset class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LST);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief Init necessary motion system objects on test call.
  /// @return S_OK, when initialization successful.
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief Free memory of allocated objects in doInit() test function.
  /// @return S_OK, when deletion successful.
  HRESULT doEnd();

private:  //test methods

  /// @brief   This method test the function onJobEnterActivating
  /// @return  S_OK, when tests were successful
  HRESULT TestJobEnterActivating();

  /// @brief   This method test the realtime command activation sequence
  /// @return  S_OK, when tests were successful
  HRESULT TestRTCommandActivating();

  /// @brief   This method test the function onJobAborting
  /// @return  S_OK, when tests were successful
  HRESULT TestJobAborting();

  /// @brief   This method test the function onUpdateStatusInDevice
  /// @return  S_OK, when tests were successful
  HRESULT TestUpdateStatusInDevice();

private: //data
  test::MockAxisProfileTrace m_mockProfileTrace {};             ///< Mockup of the Profile trace interface used in test
  test::MockScaling m_mockScaling {};                           ///< Mockup of the scaling interface used in test
  test::MockAxisProfileMappingHandler m_mockMappingHandler {};  ///< Mockup of the Mapping handle interface used in test
  test::MockAxisProfileDeviceParam m_mockDeviceParam {};        ///< Mockup of the device parameter interface used in test
  test::MockAxisProfileCyclicData m_mockCyclicData {};          ///< Mockup of the cyclic data interface used in test
  test::MockAxisProfileStatusControl m_mockStatusControl {};    ///< Mockup of the status/control interface used in test
  std::unique_ptr<AxisProfileSharingHandler> m_sharingHandler;  ///< unique pointer to the sharing handler
};
