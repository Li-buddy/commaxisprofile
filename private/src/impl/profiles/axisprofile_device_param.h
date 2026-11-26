#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class for device parameter handling.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2021-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_booting_config_c.h"
#include "comm/axisprofile/interfaces/i_axisprofile_device_param_c.h"
#include "data_sharing/i_sharing_handler.h"
#include "helper/requset_statemachine.h"
#include "i_axisprofile_device_param_internal.h"
#include "axisprofile_param_helper.h"
#include "sercos/cmd_jobs/sercos_job_device_param.h"
#include "canopen/cmd_jobs/canopen_job_device_param.h"
#include <atomic>
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestProfileDeviceParam;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class ProfileTrace;

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------    

  /// @brief This is the base class for booting config handling.
  class AxisProfileDeviceParam : public IAxisProfileDeviceParamInternal
  {
  public: //methods

    /// @brief Constructor      
    /// @param[in]   profileBasicInfo     Pointer to the Profile Basic Information
    /// @param[in]   at                   Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt                  Pointer to the Master Data Telegram (MDT)
    explicit AxisProfileDeviceParam(ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at, MasterDataTelegram* mdt);

    /// @brief Destructor
    virtual ~AxisProfileDeviceParam() noexcept override;

    const IAxisProfileBootingConfigC* getDeprecatedCInterface() const final;
    const IAxisProfileDeviceParamC* getCInterface() const final;
    AxisProfileResult booting() override;
    AxisProfileResult getDCSync0ShiftTime(double& sync0) final;
    AxisProfileResult getOutputEffectiveDelay(double& delay) final;
    AxisProfileResult getRateTorqueForce(double& rateTorqueForce, std::string& dynErrTxt) final;
    AxisProfileResult checkAxisHomed(bool& homed) override;
    AxisProfileResult setAxisHomed(bool homed) final;

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
      return ParamHelper::template deviceParamBase<std::uint16_t, VALUTE_TYPE, sercos::DatJobDeviceParamSoE>(info, jobObj, type, id, value, size);
    }

    /// @brief Warp method of deviceParamBase for Sercos device param access with CoE
    /// @tparam T               Type of internal value to be read/write
    /// @param[in] info         Profile basic information
    /// @param[in] jobObj       Job object of device parameter read/write
    /// @param[in] type         Read or write
    /// @param[in] idn          IDN of the parameter in the drive
    /// @param[in,out] value    Value to be read/write
    /// @param[in] size         Size of the value
    /// @return Return value indicating success or error.
    template<typename VALUTE_TYPE>
    static AxisProfileResult deviceParamCoE(ProfileBasicInfo& info, IJobObj* jobObj, ReadWriteType type,
                                            ObjectDirEntry id, VALUTE_TYPE& value, std::size_t size)
    {
      return ParamHelper::template deviceParamBase<ObjectDirEntry, VALUTE_TYPE, canopen::DatJobDeviceParamCoE>(info, jobObj, type, id, value, size);
    }

  protected: // methods

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    SignalFuncHandler* getSignalFuncHandler() final;

    /// @brief This method read distribution clock config information of the slave
    /// @return      Return value indicating success or error.
    AxisProfileResult readDcConfigInfo();
    
    /// @brief This method reset the control-based home flag to false during booting.
    /// @return Return value indicating success or error.
    AxisProfileResult initHomeState();

    /// @brief This method prepares the signal function handler for the drive.
    /// @return return the result of preparing the signal function handler for the drive.
    AxisProfileResult prepareSignalFuncHandler();

    /// @brief This method update the homed state of the axis based on cyclic data, if cyclic data available.
    /// @return Return value indicating success or error.
    bool updateHomedByCyclicData();

  protected: // data

    friend class ::TestProfileDeviceParam;                                      ///< Unit test of the booting config
    ProfileBasicInfo& m_profileBasicInfo;                                       ///< Profile basic information
    const comm::axisprofile::AcknowledgeTelegram* m_telegramAT {nullptr};       ///< Pointer to the Acknowledge Telegram (AT)
    comm::axisprofile::MasterDataTelegram* m_telegramMDT {nullptr};             ///< Pointer to the Master Data Telegram (MDT)    
    IAxisProfileSharingHandler* m_sharingHandler {};                            ///< mediator for sharing data
    std::vector<std::function<AxisProfileResult()>> m_initDeviceParam;          ///< vector contains all init methods

    bool m_dcSupport {false};                                                   ///< If the device support distribution clock
    double m_dcSync0ShiftTime {0.0};                                            ///< DC Sync 0 shift time
    double m_dcSync0CycleTime {0.0};                                            ///< DC Sync 0 cycle time, normally same as fieldbus cycle

    bool m_hasOutputEffectiveDelay {false};                                     ///< If the device has output delay
    double m_outputEffectiveDelay {0.0};                                        ///< Output latch delay of the device, in second    
    std::atomic_bool m_homed {false};                                           ///< whether axis is homed
    SignalFuncHandler* m_signalFuncHandler {nullptr};                           ///< Signal function handler

  private:  // data

    IAxisProfileBootingConfigC m_cDeprecatedInterface {};                       ///< Deprecated C-Interface for external usage
    IAxisProfileDeviceParamC m_cInterface{};                                    ///< C-Interface for external usage    
  };
}
