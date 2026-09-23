#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Test Class
#include "profiles/category/drive/axisprofile_device_param_drive.h"
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
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief Test class for the profile command base class.
class TestProfileDeviceParam : public xTST_Test2
{
public:
  /// @brief Constructor
  explicit TestProfileDeviceParam() = default;

  /// @brief Destructor
  virtual ~TestProfileDeviceParam() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.DeviceParameter");
  TEST_DECLARE_DESCRIPTION("Unittest of the profile booting config base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_JS);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

private: // Methods

  /// @brief Test the command C-Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testCInterface() const;

  /// @brief Test the interface getPositionDataFormat.
  /// @return S_OK, when tests were successful
  HRESULT testForGetPostionDataFormat() const;
  
  /// @brief Test the interface readDcConfigInfo.
  /// @return S_OK, when tests were successful
  HRESULT testDcConfigRelatedMethods() const;

  /// @brief Test the interface isFuncInSignalStatusWord.
  /// @return S_OK, when tests were successful
  HRESULT testFuncInSignalStatusWord() const;

  /// @brief Test of miscellaneous methods.
  /// @return S_OK, when tests were successful
  HRESULT testMiscellaneous() const;

  /// @brief Test the isDriveSupportChecksum function
  /// @return S_OK, if test was successful
  HRESULT testIsDriveSupportChecksum();

private: // Data

  std::uint32_t m_deviceAddress = 0;                  ///< Device address used in this test
  std::string m_profileName = "testProfile";     ///< Profile name used in this test

private: //Types

  /// @brief AxisProfileDeviceParamUinttest
  /// Specialization of the test class just to overwrite the pure virtual methods and make it creatable. 
  class AxisProfileDeviceParamUinttest : public AxisProfileDeviceParamDrive
  {
  public: //methods

    explicit AxisProfileDeviceParamUinttest(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt)
      : AxisProfileDeviceParamDrive(profileBasicInfo, at, mdt) {};

    virtual ~AxisProfileDeviceParamUinttest() = default;

    OpModeAvailInfo* getOpModeAvailInfo() final { return nullptr; };
    bool checkFunctionSupported(const FunctionSupport /*function*/, FunctionSupportErrorInfo& /*errorInfo*/) final { return m_funcSupported; };
    AxisProfileResult readSignalStatusWordCfg(std::vector<std::uint32_t>& /*params*/, std::vector<std::uint16_t>& /*assignBits*/) final { return AxisProfileResult::STS_OK; };

    /// @brief This method can be used by tests to change the behavior of the supported functionality.
    /// @param[out] data  configuration data.
    virtual void setFuncSupportResult(bool supported) { m_funcSupported = supported; }

    void loadBootingTasks() override {}
    AxisProfileResult checkStandstill(bool& standStill) final
    {
      standStill = m_standstill;
      return AxisProfileResult::STS_OK;
    };
    
    AxisProfileResult getChecksumIdnList(std::vector<std::string>& /*idnStrListRet*/) const override final { return AxisProfileResult::STS_OK; }
  protected: //methods
    bool checkBootIdnListModified() override final {return true;}
    AxisProfileResult getDeviceInformation() override final { return AxisProfileResult::STS_OK; };

  private: //methods
    AxisProfileResult readChecksumValueFromDevice(std::uint32_t&/* checkSum*/) override final { return AxisProfileResult::STS_OK; };

  public: // data
    bool m_funcSupported {false};                                 ///< Functionality is supported or not.
    bool m_standstill {true};                                     ///< Indicates axis is standstill or not
    DriveProfileType m_profileType {DriveProfileType::UNDEFINED}; ///< Indicates drive profile type

  };
};
