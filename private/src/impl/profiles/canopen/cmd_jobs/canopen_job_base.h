#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines base job of CANOpen.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_job_base.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/canopen_param_access.h"
#include "profiles/canopen/drive/canopen_device_param_drive.h"
#include "profiles/i_axisprofile_status_control_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of base job object of CANOpen.
  class CanopenJobBase : public AxisprofileJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit CanopenJobBase(ProfileBasicInfo& profileInfo,
                            IAxisProfileSharingHandler* shareHandle = nullptr)
      :AxisprofileJobBase(profileInfo, shareHandle)
    {
      m_deviceParamIf = m_shareHandle->getDeviceParamIF();
      m_statusControlIf = m_shareHandle->getStatusControlIF();
      m_paramAccessIf = static_cast<CanopenParamAccess*>(m_shareHandle->getParamAccessIF());
    }

    /// @brief Destructor
    virtual ~CanopenJobBase() noexcept override = default;
    JobIDType getID() override {return "CanopenJobSetAbsPos";}

  protected: // methods

    AxisProfileResult onJobEnterActivating() override
    {
      m_asyncSm.resetCommandState();
      m_asyncSm.switchToCmdStartStep();
      m_smStatus.resetRequestState();
      return JobObj::onJobEnterActivating();
    }

    AxisProfileResult onJobEnterAborting() override
    {
      m_asyncSm.resetCommandState();
      m_asyncSm.switchToCmdStartStep();
      m_smStatus.resetRequestState();
      return JobObj::onJobEnterAborting();
    }

    /// @brief This method write value to device
    /// @param[in] objDir     The object directory entry of the parameter to be written
    /// @param[in] value      The value to be set to the object entry
    /// @return Return value indicating success or error.
    AxisProfileResult writeToDeviceU16(const ObjectDirEntry& objDir, std::uint16_t value)
    {
      m_u16Value = value;
      return m_paramAccessIf->writeAndCheckParamAsync(objDir.index, objDir.subIndex,
                                                      &m_u16Value, sizeof(m_u16Value), m_smStatus);
    }

    /// @brief This method write new reference position and offset to device.
    ///        The new position and offset has be writes to device before homing.
    /// @return Return value indicating success or error.
    AxisProfileResult exeParamListAccess()
    {
      return m_paramAccessIf->accessAndCheckParamListAsync(m_paramList, m_smStatus);
    }

  protected: // data

    IAxisProfileDeviceParam* m_deviceParamIf {};      ///< device parameter interface
    IAxisProfileStatusControl* m_statusControlIf {};  ///< status control interface
    CanopenParamAccess* m_paramAccessIf {};           ///< CANOpen parameter access

    std::uint16_t m_u16Value {};                      ///< U16 value to be written
    RequestStatemachine m_smStatus {};                ///< Status request state machine
    std::list<CanopenParamStruct> m_paramList {};     ///< parameter list to be access asynchronously
  };
}
