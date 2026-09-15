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
#include <atomic>
#include <map>
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
    explicit AxisProfileDeviceParam(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    virtual ~AxisProfileDeviceParam() noexcept override = default;

    const IAxisProfileBootingConfigC* getDeprecatedCInterface() const final;
    const IAxisProfileDeviceParamC* getCInterface() const final;
    AxisProfileResult booting() final;
    AxisProfileResult getDCSync0ShiftTime(double& sync0) final;
    AxisProfileResult getOutputEffectiveDelay(double& delay) final;
    bool isFuncInSignalStatusWord(SignalStatusWordFunction& func, FunctionSupportErrorInfo& errorInfo) final;
    AxisProfileResult getRateTorqueForce(double& rateTorqueForce, std::string& dynErrTxt) final;
    bool isNeedReadBootParams() const override final;
    std::uint32_t getCheckSumValue() const override final;
    AxisProfileResult checkAxisHomed(bool& homed) override;
    AxisProfileResult setAxisHomed(bool homed) final;

  protected: // methods

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    /// @brief This method read distribution clock config information of the slave
    /// @return      Return value indicating success or error.
    AxisProfileResult readDcConfigInfo();

    /// @brief Register the predefined functions supported by signal status word.
    /// @param[in] function  The predefined function.
    /// @param[in] validator The validator, which will be called at booting.
    void registerSignalStatusWordFuncs(const FunctionSupport& function, const SignalStatusWordValidator& validator);

    /// @brief Check and validate the signal status word configuration.
    /// @return      Return value indicating success or error.
    AxisProfileResult checkSignalStatusWord();

    /// @brief This method reset the control-based home flag to false during booting.
    /// @return Return value indicating success or error.
    AxisProfileResult initHomeState();

    /// @brief This method check whether need read drive booting parameters
    /// @return      Return value indicating whether need read or not.
    virtual bool checkNeedReadBootParams() = 0;

    /// @brief Get some device information during startup
    /// @return      Return value indicate succeed read or not
    virtual AxisProfileResult getDeviceInformation() = 0;

  protected: // data

    friend class ::TestProfileDeviceParam;                                      ///< Unit test of the booting config
    ProfileBasicInfo& m_profileBasicInfo;                                       ///< Profile basic information
    IAxisProfileSharingHandler* m_sharingHandler {};                            ///< mediator for sharing data
    std::vector<std::function<AxisProfileResult()>> m_initDeviceParam;          ///< vector contains all init methods

    bool m_dcSupport {false};                                                   ///< If the device support distribution clock
    double m_dcSync0ShiftTime {0.0};                                            ///< DC Sync 0 shift time
    double m_dcSync0CycleTime {0.0};                                            ///< DC Sync 0 cycle time, normally same as fieldbus cycle

    bool m_hasOutputEffectiveDelay {false};                                     ///< If the device has output delay
    double m_outputEffectiveDelay {0.0};                                        ///< Output latch delay of the device, in second    
    std::uint32_t m_checksum {};                                                ///< Parameter checksum
    bool m_needReadBootParams {true};                                           ///< whether read drive booting parameters
    std::atomic_bool m_homed {false};                                           ///< whether axis is homed             

  private:  // data

    IAxisProfileBootingConfigC m_cDeprecatedInterface {};                       ///< Deprecated C-Interface for external usage
    IAxisProfileDeviceParamC m_cInterface{};                                    ///< C-Interface for external usage
    std::vector<SignalStatusWordFunction> m_signalStatusWordFuncs{};            ///< The predefined functions supported by signal status word.
    bool m_firstBootup {true};                                                  ///< whether is the first time to boot up
  };
}
