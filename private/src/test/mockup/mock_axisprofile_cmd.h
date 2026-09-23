#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup of internal interface of command.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mockup/mock_job_obj.h"
#include "profiles/i_axisprofile_cmd_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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

  /// @brief This class implements a mockup for the axis profile command interface.
  class MockAxisProfileCmd : public ::comm::axisprofile::IAxisProfileCmdInternal
  {
  public: // methods

    /// @brief Constructor
    MockAxisProfileCmd() = default;

    /// @brief Destructor
    virtual ~MockAxisProfileCmd() noexcept override = default;
    ::comm::axisprofile::CmdState getCmdState(::comm::axisprofile::CmdType /*cmdType*/) override { return m_cmdState; }
    ::comm::axisprofile::AxisProfileResult resetCmd(::comm::axisprofile::CmdType /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult abortCmd(::comm::axisprofile::CmdType /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::CmdStatusInDevice getCmdStatusInDevice(::comm::axisprofile::CmdType /*cmdType*/) override { return m_cmdStatusInDevice; }
    ::comm::axisprofile::AxisProfileResult clearDeviceError(::comm::axisprofile::CmdType& /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult clearDeviceError2() override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult setAbsolutePosition(double /*newPos*/, ::comm::axisprofile::AxisProfileEncoder /*encoder*/, ::comm::axisprofile::CmdType& /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult referencingAxis(double /*newPos*/, ::comm::axisprofile::AxisProfileEncoder /*encoder*/, ::comm::axisprofile::CmdType& /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult fixedStop(double /*clampingTorque*/, ::comm::axisprofile::CmdType& /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult setNewReferenceWithoutMovement(double /*newPos*/, ::comm::axisprofile::AxisProfileEncoder /*encoder*/, ::comm::axisprofile::RefMode /*refMode*/, ::comm::axisprofile::CmdType& /*cmdType*/) override { return m_returnValue; }
    ::comm::axisprofile::AxisProfileResult switchOperationModeWithCheck(::comm::axisprofile::AxisProfileOperationMode /*opMode*/) override { return m_returnValue; }
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/) override {}
    const IAxisProfileCmdC* getCInterface() const override { return m_cmdCInterface; }

  public: // data

    IAxisProfileCmdC* m_cmdCInterface = nullptr; ///< Pointer to the command C-Interface
    ::comm::axisprofile::CmdState m_cmdState = ::comm::axisprofile::CmdState::STATE_UNKNOWN;                ///< State of the command
    ::comm::axisprofile::CmdStatusInDevice m_cmdStatusInDevice = ::comm::axisprofile::CmdStatusInDevice::DEVICE_NOT_SET; ///< Status of command in device
    ::comm::axisprofile::AxisProfileResult m_returnValue = ::comm::axisprofile::AxisProfileResult::STS_OK;  ///< Result of member functions
  };
}
