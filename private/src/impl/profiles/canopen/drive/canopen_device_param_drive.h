#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declare CanopenDeviceParamDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "profiles/axisprofile_param_helper.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/cmd_jobs/canopen_job_device_param.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenDeviceParam;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This struct contains the value which used on touch probe
  struct BootingDataForPrbLatchedTimes
  {
    std::uint16_t m_probe1PosLatchedTimes {};    ///< 0x60D5 (Touch probe 1 positive edge counter)
    std::uint16_t m_probe1NegLatchedTimes {};    ///< 0x60D6 (Touch probe 1 negative edge counter)
    std::uint16_t m_probe2PosLatchedTimes {};    ///< 0x60D7 (Touch probe 2 positive edge counter)
    std::uint16_t m_probe2NegLatchedTimes {};    ///< 0x60D8 (Touch probe 2 negative edge counter)
  };

  /// @brief This class implements the read functions of device param
  class CanopenDeviceParamDrive : public AxisProfileDeviceParamDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileBasicInfo Pointer to the Profile Basic Information
    /// @param[in] at               Pointer to the Acknowledge Telegram (AT)
    /// @param[in] mdt              Pointer to the Master Data Telegram (MDT)
    CanopenDeviceParamDrive(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    ~CanopenDeviceParamDrive() noexcept override = default;
    
    OpModeAvailInfo* getOpModeAvailInfo() final;
    bool checkFunctionSupported(const FunctionSupport function, FunctionSupportErrorInfo& errorInfo) final;
    AxisProfileResult checkStandstill(bool& standStill) final;
    AxisProfileResult getChecksumIdnList(std::vector<std::string>& idnStrListRet) const final;
    AxisProfileResult checkAxisHomed(bool& homed) final;

  protected: //methods

    bool checkBootIdnListModified() override final;
    AxisProfileResult getDeviceInformation() override final;

  private: // methods

    void loadBootingTasks() final;

    /// @brief Read the parameter of delay brake from device
    /// @return      Return succeed read or not
    AxisProfileResult readDelayBrakeParam();

    /// @brief Get the brake delay information
    /// @return      Return succeed read or not
    AxisProfileResult getDelayBrakeInfo();

    /// @brief Get the brake deceleration reaction
    /// @return      Return succeed read or not
    AxisProfileResult getBrakeDynLimit();

    /// @brief Determine operation mode if it is available.
    /// @return Return determination result.
    AxisProfileResult determineOpModeAvailable();

    /// @brief check device quick stop configuration(only for rexroth devices)
    /// @return      Return succeed read or not
    AxisProfileResult checkDeviceQuickStopCfg();

    /// @brief Read the parameter of probe latched times from device
    /// @return         Return succeed read or not
    AxisProfileResult readProbeLatchedParam();

    /// @brief Read the parameter of probe latched times from device
    /// @return         Return succeed read or not
    AxisProfileResult readEncoderTypes();

    /// @brief This method gets job object of device parameter access, which used for read/write parameter in background thread
    /// @return      Return value indicating success or error.
    AxisProfileResult getDeviceParamJobObj();

    /// @brief Read one CoE boot parameter from snapshot cache first, then drive.
    /// @param[in] objEntry CoE object entry.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameterValueFromSnapshot(const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen);

    AxisProfileResult readChecksumValueFromDevice(std::uint32_t& checkSum) override final;

    /// @brief This method sets the IDNs of the parameters that are used to generate the checksum.
    /// @note Important, the method can only be used in booting thread of axisprofile
    /// @param[in]  objList  IDN list of checksum parameter 
    /// @return     Return value indicating success or error. SercosDeviceParamDrive::setChecksumIDNList
    AxisProfileResult writeChecksumIdnListToDevice(const std::vector<ObjectDirEntry>& objList);

    /// @brief This method gets the IDNs of the parameters that are used to generate the checksum.
    /// @note Important, the method can only be used in booting thread of axisprofile
    /// @param[out]  objList  IDN list of checksum parameter
    /// @return     Return value indicating success or error.
    AxisProfileResult readChecksumIdnListFromDevice(std::vector<ObjectDirEntry>& objList);
;
  private: // data

    friend class ::TestCanopenDeviceParam;                                    ///< unit test of CanopenStatusControlDrive
    OpModeAvailInfoCanopen m_opModeAvailInfoCanopen{ false };                 ///< Operation mode available information at drive side
    IFieldbusAdapter* const m_fieldbusAdapter {};                             ///< Pointer to fieldbus adapter
    std::uint8_t  m_driveNumber {};                                           ///< Drive number
    BootingDataForPrbLatchedTimes m_bootingInfoPrb {};                        ///< Booting information of probe latched times will be read while axisprofile booting         
    std::vector<ObjectDirEntry> m_checksumIdnList;                            ///< checksum idn list
    IJobObj* m_JobObj {};                                                     ///< Job object of device parameter access
  };
}
