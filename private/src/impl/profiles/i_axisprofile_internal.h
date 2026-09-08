#pragma once

/// @file
///
/// This header file is part of the axisprofile library and define the
/// internal interface for all axisprofiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "aux_job/i_aux_job_handler.h" // TODO: be removed when new interface ready
#include "axisprofile_internal_defs.h"
#include "bundle/i_axisprofile.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/interfaces/i_axisprofile_c.h"
#include "i_axisprofile_device_param_internal.h"
#include "i_axisprofile_cmd_internal.h"
#include "i_axisprofile_command_internal.h"
#include "i_axisprofile_cyclic_data_internal.h"
#include "i_axisprofile_diag_msg_internal.h"
#include "i_axisprofile_param_access.h"
#include "i_axisprofile_probe_internal.h"
#include "i_axisprofile_property_internal.h"
#include "i_axisprofile_status_control_internal.h"
#include "job_mgr/i_job_mgr.h"
#include "scaling/i_scaling.h"
#include "value_mapping/i_mapping_handler.h"
#include "category/i_category_adapter.h"
#include <functional>
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
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
  // Forward references
  // ---------------------------------------------------------------------------
  class IFieldbusAdapter;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the internal interface for all axisprofiles.
  class IAxisProfileInternal : public IAxisProfile
  {
  public: // methods

    /// @brief destructor
    virtual ~IAxisProfileInternal() noexcept override = default;

    /// @brief This method creates a new axisprofile C-Interface instance. Usually it is only used by the AxisProfileManager when creating a new profile.
    ///        IMPORTANT: This method is not realtime-capable!
    /// @return  Pointer to the axisprofile C-Interface.
    virtual const IAxisProfileC* createCInterface() = 0;

    /// @brief This method gets the interface for axisprofile commands.
    /// @return  Pointer to the command interface.
    virtual IAxisProfileCommandInternal* getCommandInterface() = 0;

    /// @brief This method gets the interface for status- and controlword management.
    /// @return  Pointer to the status- and controlword interface.
    virtual IAxisProfileStatusControlInternal* getStatusControlInterface() = 0;

    /// @brief This method gets the interface for diagnostic message management.
    /// @return  Pointer to the diagnostic message interface.
    virtual IAxisProfileDiagMsgInternal* getDiagMsgInterface() = 0;

    /// @brief This method gets the interface for cyclic data management.
    /// @return  Pointer to the cyclic data interface.
    virtual IAxisProfileCyclicDataInternal* getCyclicDataInterface() = 0;

    /// @brief This method gets the interface for device par mater parameters.
    /// @return  Pointer to the device parameter interface.
    virtual IAxisProfileDeviceParamInternal* getDeviceParamInterface() = 0;

    /// @brief This method gets the interface for command.
    /// @return  Pointer to the command interface.
    virtual IAxisProfileCmdInternal* getCmdInterface() = 0;

    /// @brief This method gets the interface for peoperty.
    /// @return  Pointer to the property interface.
    virtual IAxisProfilePropertyInternal* getPropertyInterface() = 0;

    /// @brief This method initializes the configuration of the axis profile when profile is created
    /// @return  Return value indicating success or error.
    virtual AxisProfileResult init() = 0;

    /// @brief This method return the mapping handler of this profile.
    /// @return  Pointer to the mapping handler class.
    virtual IAxisProfileMappingHandler* getMappingHandler() const = 0;

    /// @brief This method return the fieldbus adapter of this profile.
    /// @return  Pointer to the fieldbus adapter class.
    virtual IFieldbusAdapter* getFieldbusAdapter() const = 0;

    /// @brief This method return the scaling class of this profile.
    /// @return  Pointer to the scaling class.
    virtual IScaling* getScaling() const = 0;

    /// @brief This method returns the name of the profile.
    /// @return The profile name
    virtual const char* getProfileName() = 0;

    /// @brief This method returns the IEC 61800-7 drive profile type.
    /// @return Drive profile type
    virtual DriveProfileType getDriveProfileType() const = 0;

    /// @brief This method returns the network technology which is used by the profile.
    /// @return Network technology
    virtual NetworkTechnology getNetworkTechnology() const = 0;

    /// @brief  This method copies the at data from the given position in the telegram to the axis profile.
    /// @param[in,out]  dataByteArray   Pointer to position in telegram where the at data of the device are located.
    ///
    /// @return Return value indicating success or error.
    virtual AxisProfileResult processDataAt(std::uint8_t* dataByteArray) = 0;

    /// @brief  This method copies the mdt data from the axis profile to the given position in the telegram.
    /// @param[in,out]  dataByteArray   Pointer to position in telegram where the mdt data of the device are located.
    ///
    /// @return Return value indicating success or error.
    virtual AxisProfileResult processDataMdt(std::uint8_t* dataByteArray) = 0;

    /// @brief  This method return a vector which contains all the init methods
    /// @return return a vector which contains all the methods for initiation in correct order
    virtual const std::vector<std::function<AxisProfileResult()>> &getInitMethods() = 0;

    /// @brief  This method is called in RT-Thread.
    /// @return Return STS_OK when success.
    virtual AxisProfileResult runAllAuxJobs() = 0;

    /// @brief This method executes profile jobs that in back thread
    virtual void runJobsInBkThread() = 0;

    /// @brief This method gets the interface for probe parameters.
    /// @return Pointer to the probe interface.
    virtual IAxisProfileProbeInternal* getProbeInterface() = 0;

    /// @brief This method writes the drive number to the profile. 
    /// @param[in]    driveNumber  drive number.
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult setDriveNumber(const std::uint8_t driveNumber) = 0;

    /// @brief This method return the drive number to the profile. 
    /// @return       Return the drive number.
    virtual std::uint8_t getDriveNumber() const = 0;

    /// @brief  This method set the category to profile and manage related update.
    ///         note: category can't cross different protocol
    /// @param[in]  category Profile category
    /// @return Return value indicating success or error
    virtual AxisProfileResult setProfileCategory(AxisProfileCategory category) = 0;

    /// @brief Reset all input realtime data. The method should be called in case of mapping is changed.
    virtual void resetRtInputData() = 0;

    /// @brief This method reset the axisprofile during booting.
    /// @return Return STS_OK when success.
    virtual AxisProfileResult bootingReset() = 0;

    /// @brief This method set system cycle time to scaling strategy
    /// @param[in]    cycleTime  cycle time to be set
    virtual void setCycleTimeToScaling(double cycleTime) = 0;

    /// @brief Reset FPU error flag member inside profile.
    /// @return Return value indicating success (>=0) or error (<0).
    virtual void resetFPUFlag() = 0;

    /// @brief This method gets the interface for job manager.
    /// @return Pointer to the job manager.
    virtual IJobMgr* getJobMgr() = 0;

    /// @brief Set this profile to be activated or not.
    /// @param[in]  activated  The profile will be activated or not.
    virtual void setProfileActivated(bool activated) = 0;

    /// @brief Check profile whether the profile is activated.
    /// @return  Whether the profile is activated.
    virtual bool checkProfileActivated() const = 0;
  };
}
