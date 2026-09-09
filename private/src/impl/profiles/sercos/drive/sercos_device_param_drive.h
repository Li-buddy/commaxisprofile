#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declare SercosDeviceParamDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "job_object/i_job_obj.h"
#include "profiles/axisprofile_param_helper.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/i_device_boot_param_snapshot.h"
#include "profiles/sercos/cmd_jobs/sercos_job_device_param.h"
#include "profiles/sercos/sercos_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosDeviceParam;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // Multi brake of bit control
  constexpr std::uint16_t SERCOS_MULTI_BRAKE_LIGHT_ERROR         = 0x0001;    ///< Sercos multi brake with light error.
  constexpr std::uint16_t SERCOS_MULTI_BRAKE_FIELDBUS_ERROR      = 0x0004;    ///< Sercos multi brake with fieldbus error.
  constexpr std::uint16_t SERCOS_MULTI_BRAKE_CRITICAL_ERROR      = 0x0008;    ///< Sercos multi brake with critical error.

  // Brake reaction bit control
  constexpr std::uint16_t BRAKE_REACTION_VALUE_2                 = 0x0002;    ///< The token of quick stop.
  constexpr std::uint16_t BRAKE_REACTION_VALUE_3                 = 0x0003;    ///< The token of return motion.
  constexpr std::uint16_t BRAKE_REACTION_VALUE_4                 = 0x0004;    ///< The token of emergency stop.
  constexpr std::uint16_t BRAKE_REACTION_VALUE_40                = 0x0040;    ///< The token of emergency stop.
  constexpr std::uint16_t VALUE_BIT_0_TO_BIT3                    = 0x000F;    ///< Get the token of bit0-bit3.
  constexpr std::uint16_t VALUE_BIT_4_TO_BIT7                    = 0x00F0;    ///< Get the token of bit4-bit7.

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief This enum class indicates whether the delay brake of every error level activated
  enum class MultiBrakeInfo
  {
    LIGHT_ERROR_ACTIVATED = 0,                           ///< Light error bit of P-0-0117 is activated
    LIGHT_ERROR_INACTIVATED,                             ///< Light error bit of P-0-0117 is inactivated
    FIELDBUS_ERROR_ACTIVATED,                            ///< Field bus error bit of P-0-0117 is activated
    FIELDBUS_ERROR_INACTIVATED,                          ///< Field bus error bit of P-0-0117 is inactivated
    CRITICAL_ERROR_ACTIVATED,                            ///< Critical error bit of P-0-0117 is activated
    CRITICAL_ERROR_INACTIVATED,                          ///< Critical error bit of P-0-0117 is inactivated
  };

  /// @brief This enum class indicates the deceleration reaction
  enum class ErrorBrakeReaction
  {
    QUICK_STOP = 0,                                      ///< Velocity command value reset with ramp and filter
    RETURN_MOTION,                                       ///< For systematic return motion in the case of error
    EMERGENCY_STOP_WITH_DECELERATION,                    ///< Velocity command value reset with ramp and filter
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This struct contains the value which used on multi brake
  struct BootingDataForMultiBrake
  {
    std::uint16_t m_multiBrakeDelaySupport{};                 ///< P-0-0117   (Multi brake support)
    std::uint16_t m_controlUnitReactionTimeLightError{};      ///< P-0-0117.1 (Control unit reaction time F2/3)
    std::uint16_t m_controlUnitReactionTimeCriticalError{};   ///< P-0-0117.2 (Control unit reaction time F4/6/7)
    std::uint16_t m_controlUnitReactionBestDeceleration{};    ///< P-0-0119   (Best possible deceleration)
    std::int32_t m_emergencyBrakeDeceleration{};              ///< S-0-0429   (Emergency brake deceleration)
    std::int32_t m_driveHaltAccelerationBipolar{};            ///< S-0-0372   (Drive halt acceleration bipolar)
    std::int32_t m_bipolarAccelerationLimitValue{};           ///< S-0-0138   (Bipolar acceleration limit value)
    std::int32_t m_bipolarJerkLimit{};                        ///< S-0-0349   (Bipolar jerk limit)
    std::int32_t m_returnAcceleration{};                      ///< P-0-0057   (Return acceleration)
    std::int32_t m_returnJerk{};                              ///< P-0-0058   (Return jerk)
  };

  /// @brief This class implements the read methods for sercos device param in drive category.
  class SercosDeviceParamDrive : public AxisProfileDeviceParamDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information
    /// @param[in]   at                 Pointer to the acknowledge data
    /// @param[in]   mdt                Pointer to the master data telegram
    explicit SercosDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    virtual ~SercosDeviceParamDrive() noexcept override = default;

    /// @brief Warp method of deviceParamBase for Sercos device param access with SoE
    /// @tparam T               Type of internal value to be read/write
    /// @param[in] info         Profile basic information
    /// @param[in] jobObj       Job object of device parameter read/write
    /// @param[in] type         Read or write
    /// @param[in] idn          IDN of the parameter in the drive
    /// @param[in,out] value    Value to be read/write
    /// @param[in] size         Size of the value
    /// @return Return value indicating success or error.
    template<typename VALUTE_TYPE>
    static AxisProfileResult deviceParamSoE(ProfileBasicInfo& info, IJobObj* jobObj, ReadWriteType type,
                                            std::uint16_t id, VALUTE_TYPE& value, std::size_t size)
    {
      return ParamHelper::deviceParamBase<std::uint16_t, VALUTE_TYPE, DatJobDeviceParamSoE>(info, jobObj, type, id, value, size);
    }

    OpModeAvailInfo* getOpModeAvailInfo() final;
    bool checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo) final;
    AxisProfileResult checkStandstill(bool& standStill) final;
    AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const final;
    AxisProfileResult checkAxisHomed(bool& homed) final;

  protected:
    bool checkBootIdnListModified() override final;
    AxisProfileResult getDeviceInformation() override final;

  private: // methods

    void loadBootingTasks() final;

    /// @brief Read the parameter of delay brake from device
    /// @return         Return succeed read or not
    AxisProfileResult readDelayBrakeParam();

    /// @brief Get the brake delay information
    /// @return         Return succeed read or not
    AxisProfileResult getDelayBrakeInfo();

    /// @brief Get the brake deceleration reaction
    /// @return         Return succeed read or not
    AxisProfileResult getBrakeDynLimit();

    /// @brief Mapping the brake deceleration reaction information
    /// @param[in, out] checkDecelerationLimitSupport If the selected error type supports the deceleration limit
    /// @return         Return succeed read or not
    AxisProfileResult mappingBrakeInfo(bool& checkDecelerationLimitSupport);

    /// @brief Read the parameter of operation mode from device
    /// @return         Return succeed read or not
    AxisProfileResult readOpModeParam();

    /// @brief Determine operation mode if it is available at drive side, and write result to "AxisProfileDeviceParam::m_opModeAvailableInfo"
    /// @return         Return succeed read or not
    AxisProfileResult determineOpModeAvailable();

    /// @brief This method reads encoders' types
    /// @return      Return value indicating success or error.
    AxisProfileResult readEncodersTypes();

    /// @brief This method reads drive limit
    /// @return      Return value indicating success or error.
    AxisProfileResult readDriveLimits();

    /// @brief This method reads drive position process mode.
    /// @return      Return value indicating success or error.
    AxisProfileResult readPosProcessMode();

    /// @brief This method gets job object of device parameter access, which used for read/write parameter in background thread
    /// @return      Return value indicating success or error.
    AxisProfileResult getDeviceParamJobObj();

    AxisProfileResult readChecksumValueFromDevice(std::uint32_t& checkSum) override final;

    /// @brief This method sets the IDNs of the parameters that are used to generate the checksum.
    /// @note Important, the method can only be used in booting thread of axisprofile
    /// @param[in]  idnList  IDN list of checksum parameter
    /// @return     Return value indicating success or error.
    AxisProfileResult writeChecksumIdnListToDevice(const std::vector<std::uint16_t>& idnList);

    /// @brief This method gets the IDNs of the parameters that are used to generate the checksum.
    /// @note Important, the method can only be used in booting thread of axisprofile
    /// @param[out]  idnList  IDN list of checksum parameter
    /// @return     Return value indicating success or error.
    AxisProfileResult readChecksumIdnListFromDevice(std::vector<std::uint16_t>& idnList);

    /// @brief get the parameter's value using the param snaphot.
    /// @param[in] idn SoE IDN.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @param[in] elementFlag SoE element flag of the parameter value/maxValue access.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameterValueFromSnaphot(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen,
                        comm::ethercat::master::fbs::ElementFlags elementFlag = comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);

  private: // data

    friend class ::TestSercosDeviceParam;                         ///< Unit test of the profile
    BootingDataForMultiBrake m_bootingDataMultiBrake {};          ///< Booting data of multi brake will be read while axisprofile booting  
    MultiBrakeInfo m_multibrakeInfo {};                           ///< The multi brake situation
    std::map<OperationMode, GenParamU16> m_bootingDataOpMode {};  ///< Booting data of operation mode will be read while axisprofile booting
    OpModeAvailInfoSercos m_opModeAvailInfoSercos {false};        ///< Operation mode available information at drive side
    IJobObj* m_JobObj {};                                         ///< Job object of device parameter access
    std::vector<std::uint16_t> m_checksumIdnList;                 ///< checksum idn list
  };
}
