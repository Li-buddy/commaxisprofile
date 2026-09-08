#pragma once

/// @file      mock_axisprofile_diag_msg.h
///
/// This file is part of the axisprofile library and implements a mockup for
/// the axis profile diagnostic message interface.
///
/// @copyright (C) 2018-2021 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_diag_msg_internal.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  /// @brief This class implements a mockup for the axis profile diagnosis message interface.
  class MockAxisProfileDiagMsg : public comm::axisprofile::IAxisProfileDiagMsgInternal
  {
  public: // methods

    /// @brief Constructor
    MockAxisProfileDiagMsg() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileDiagMsg() noexcept override = default;

    const IAxisProfileDiagMsgC* getCInterface() const override { return nullptr; }

    // --------------------------------------------- Diagnostic message -----------------------------------------------

    comm::axisprofile::AxisProfileDeviceErrorType getErrorType() override { return m_errorType; }
    std::uint32_t getErrorCode() override { return m_errorCode; }
    void resetErrCode() override { m_errorCode = UINT32_MAX; };
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};

  public: // data

    comm::axisprofile::AxisProfileDeviceErrorType m_errorType = comm::axisprofile::AxisProfileDeviceErrorType::ERROR_NO_INFO;   ///< Indicate a device error type is currently simulated
    std::uint32_t m_errorCode = 0;  ///< Indicate a device error code is currently simulated
  };
}
