#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements
/// base class of all axis profiles.
/// 
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "category/i_category_adapter.h"
#include "common/mutex_helper.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "i_axisprofile_internal.h"
#include "i_axisprofile_property_internal.h"
#include "job_mgr/job_mgr.h"
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileSharingHandler;
  class IUserPDOMgr;
  class IScaling;

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

  /// @brief This is the basic implementation for the axis profile.
  class AxisProfile : public IAxisProfileInternal
  {
  public: // methods

    /// @brief constructor
    /// @param[in]   profileName         Name of the profile
    /// @param[in]   axisProfileType     The axis profile type
    /// @param[in]   driveProfileType    IEC 61800-7 drive profile type
    /// @param[in]   networkTechnology   Network technology that should be used by the profile
    AxisProfile(const std::string& profileName, const AxisProfileType axisProfileType, const DriveProfileType driveProfileType, const NetworkTechnology networkTechnology);

    /// @brief destructor
    ~AxisProfile() noexcept override;

    // ------------------------------------- External Interface --------------------------------------

    const IAxisProfileCommandC* getCommandInterfaceC() const final;
    const IAxisProfileStatusControlC* getStatusControlInterfaceC() const final;
    const IAxisProfileDiagMsgC* getDiagMsgInterfaceC() const final;
    const IAxisProfileCyclicDataC* getCyclicDataInterfaceC() const final;
    const IAxisProfileBootingConfigC* getBootingConfigInterfaceC() const final;
    const IAxisProfileDeviceParamC* getDeviceParamInterfaceC() const final;
    const IAxisProfileProbeC* getProbeInterfaceC() const final;
    const IAxisProfileCmdC* getCmdInterfaceC() const final;
    const IAxisProfilePropertyC* getPropertyInterfaceC() const final;
    AxisProfileResult init() override;
    AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) final;
    std::uint32_t getDeviceAddress() const final;
    const char* getDeviceName() const final;
    AxisProfileResult setDeviceName(const char* cDeviceName) final;
    AxisProfileType getProfileType() const final;
    AxisProfileDeviceScalingType getDeviceScalingType(AxisProfileDeviceValueType valueType) const final;
    AxisProfileResult setProfileScalingStrategyCfg(const AxisProfileScalingStrategyCfg& ) override { return AxisProfileResult::STS_OK; }
    AxisProfileScalingStrategyCfg getProfileScalingStrategyCfg() const override { return AxisProfileScalingStrategyCfg{}; }
    AxisProfileResult setDriveNumber(const std::uint8_t driveNumber) final {m_profileBasicInfo.m_driveNumber = driveNumber; return AxisProfileResult::STS_OK;}
    std::uint8_t getDriveNumber() const final{ return m_profileBasicInfo.m_driveNumber; }

    // ------------------------------------- Internal Interface --------------------------------------

    const IAxisProfileC* createCInterface() final;
    IAxisProfileCommandInternal* getCommandInterface() final;
    IAxisProfileStatusControlInternal* getStatusControlInterface() final;
    IAxisProfileDiagMsgInternal* getDiagMsgInterface() final;
    IAxisProfileCyclicDataInternal* getCyclicDataInterface() final;
    IAxisProfileDeviceParamInternal* getDeviceParamInterface() final;
    IAxisProfileProbeInternal* getProbeInterface() final;
    IAxisProfileCmdInternal* getCmdInterface() final;
    IAxisProfilePropertyInternal* getPropertyInterface() final;
    IAxisProfileMappingHandler* getMappingHandler() const final;
    IFieldbusAdapter* getFieldbusAdapter() const final;
    IJobMgr* getJobMgr() final;
    IScaling* getScaling() const final;
    const char* getProfileName() final;
    DriveProfileType getDriveProfileType() const final;
    NetworkTechnology getNetworkTechnology() const final;
    const std::vector<std::function<AxisProfileResult()>> &getInitMethods() final{return m_initMethods;};
    virtual AxisProfileResult runAllAuxJobs() final;
    AxisProfileResult setProfileCategory(AxisProfileCategory typeMode) final;
    AxisProfileCategory getProfileCategory() final;
    void runJobsInBkThread() override;
    void resetRtInputData() final;
    AxisProfileResult bootingReset() override; // may be overridden by derived profile if necessary
    void setCycleTimeToScaling(double cycleTime) final;
    void resetFPUFlag() final;
    void setProfileActivated(bool activated) final;
    bool checkProfileActivated() const final;

  protected: // methods

    /// @brief This method gets job list to be register to job manager.
    /// @note  The derived profile gives the job list to be register.
    /// @return The job list to registered
    virtual std::vector<std::unique_ptr<IJobObj>> onGetJobList() = 0;

    /// @brief This method load the init method for the profile.
    /// @return Return value indicating success or error.
    void loadInitMethod();

    /// @brief This method check for FPU errors and returns an AxisProfileResult.
    void checkForFPUError();

  private: // methods

    /// @brief This method register all jobs of axisprofile
    /// @return Return value indicating success or error
    AxisProfileResult registerAllJobs();

  protected: // data
      
    std::string m_strDeviceName{};              ///< Name of the configured device
    ProfileBasicInfo m_profileBasicInfo;        ///< Profile basic information
    
    IAxisProfileDiagMsgInternal* m_diagMsgInterface{};               ///< Pointer to the internal diagnostic message interface
    IAxisProfileMappingHandler* m_mappingHandler{};                  ///< Pointer to the mapping handler for the profile
    IAxisProfileSharingHandler* m_sharingHandler{};                  ///< Pointer to the data sharing handler 
    IAuxJobHandler* m_auxJobHandler {};                              ///< Pointer to the aux job handler
    IAxisProfileParamAccess* m_paramAccess {};                       ///< Pointer to the axisprofile parameter access
    ICategoryAdapter* m_profileCategory {};                          ///< Pointer to the category of current profile
    IAxisProfileCmdInternal* m_cmdIf {};                             ///< Pointer to the command interface
    IAxisProfilePropertyInternal* m_propertyInterface{};             ///< Pointer to the property interface
    IJobMgr* m_jobMgr {};                                            ///< Pointer to the job manager

    const DriveProfileType m_driveProfileType;                       ///< IEC 61800-7 drive profile type
    const NetworkTechnology m_networkTechnology;                     ///< The network technology that is used for this profile
    std::vector<std::function<AxisProfileResult()>> m_initMethods;   ///< vector contains all init methods
    MutexHelper m_mutexHelper {};                                    ///< mutex helper for easy use of mutex

    AcknowledgeTelegram m_telegramAT {};    ///< Container for the Acknowledge Telegram (AT)
    MasterDataTelegram m_telegramMDT {};    ///< Container for the Master Data Telegram (MDT)
    bool m_profileActivated{true};       ///< This property holds whether the profile is activated.
  }; 
}
