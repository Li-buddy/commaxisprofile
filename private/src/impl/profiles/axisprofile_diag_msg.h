#pragma once

/// @file
///
/// This header file is part of the axisprofile library and implements
/// base class for diagnostic message handling.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "i_axisprofile_diag_msg_internal.h"
#include "data_sharing/i_sharing_handler.h"
// C-Interface
#include "comm/axisprofile/interfaces/i_axisprofile_diag_msg_c.h"

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
  /// @brief This enum represents the 'message class' of the diagnostic message.
  enum class MsgClass : std::uint16_t
  {
    UNDEFINE = 0x00,    ///< 0xxxx : No class
    COMMAND_ERR = 0x0C, ///< Cxxxx : Command error
    WARNING = 0x0E,     ///< Exxxx : Warning
    ERR_MSG = 0x0F,     ///< Fxxxx : Error message
  };

  /// @brief This enum represents the 'message class' of the diagnostic message.
  enum class ErrorLevel : std::uint16_t
  {
    NO_LEVEL = 0x00, ///< No level
    LEVEL_2 = 0x02,  ///< 2xxx
    LEVEL_3 = 0x03,  ///< 3xxx
    LEVEL_4 = 0x04,  ///< 4xxx
    LEVEL_6 = 0x06,  ///< 6xxx
    LEVEL_7 = 0x07,  ///< 7xxx
    LEVEL_8 = 0x08,  ///< 8xxx
  };

  /// @brief This structure represents the diagnostic message: Manufacturer status register (0x1002:00 / S-0-0390 / --)
  /// for communication profiles (CP): CP16/1 (SERCOS I), CP16/2 (SERCOS II) and CP12 (EtherCAT).
  struct DiagMsg
  {
                                                ///  Bits 31-30 | Indicate who is responsible for the specification of the diagnostic message
                                                ///  Bits 29-24 | Indicate the Sercos profile to which the diagnostic message belongs
    bool m_msgStatus = false;                   ///< Bit 20     | Shows the diagnostic message status, Fxxx error can be cleared or command completed set to 1
    MsgClass m_msgClass = MsgClass::UNDEFINE;   ///< Bits 19-16 | Indicate the diagnostic message class, and thus, its priority
    std::uint16_t m_statusCode = 0;             ///< Bits 15-0  | Indicate the status code of the diagnostic message
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the base class for diagnostic message handling.
  class AxisProfileDiagMsg : public IAxisProfileDiagMsgInternal
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   trace     Pointer to the profile trace
    /// @param[in]   at        Pointer to the Acknowledge Telegram (AT)
    explicit AxisProfileDiagMsg(ProfileTrace* const trace, AcknowledgeTelegram* const at);

    /// @brief Destructor
    virtual ~AxisProfileDiagMsg() noexcept override = default;

    const IAxisProfileDiagMsgC* getCInterface() const final;
    AxisProfileDeviceErrorType getErrorType() final;
    std::uint32_t getErrorCode() final;
    void resetErrCode() final;

    /// @brief This method extracts all information from the actual raw diagnostic message (current cycle).
    /// @return   Return value indicating success or error.
    AxisProfileResult analyzeDiagMsg();

  private: // methods

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    /// @brief  This method is called after new AT telegram data is received. It interprets the raw telegram data
    ///         to determine the current diagnostic message class.
    /// @return       Return value indicating success (>=0) or error (<0).
    AxisProfileResult determineMsgClassFromAT();

    /// @brief  This method is called after new AT telegram data is received. It interprets the raw telegram data
    ///         to determine the current diagnostic message status code.
    /// @return       Return value indicating success (>=0) or error (<0).
    AxisProfileResult determineStatusCodeFromAT();

    /// @brief  This method is called after new AT telegram data is received. It interprets the raw telegram data
    ///         to determine the current diagnostic message error type.
    /// @return       Return value indicating success (>=0) or error (<0).
    AxisProfileResult detemineErrorType();

  private: // data

    ProfileTrace* const m_profileTrace{};                                                      ///< Pointer to the profile trace
    AxisProfileDeviceErrorType m_actualErrorType = AxisProfileDeviceErrorType::ERROR_NO_INFO;  ///< Actual error type of the device (generalized)
    std::uint32_t m_actualErrorCode = UINT32_MAX;                                              ///< Acutal error code of the device
    IAxisProfileDiagMsgC m_cInterface{};                                                       ///< C-Interface for external usage
    const AcknowledgeTelegram* m_telegramAT {nullptr}; ///< Pointer to the Acknowledge Telegram (AT)
    DiagMsg m_diagMsg {};                              ///< Diagnostic message: Manufacturer status register (0x1002:00 / S-0-0390 / --)
    ErrorLevel m_secosErrorLevel {};                   ///< Error level
    IAxisProfileSharingHandler* m_sharingHandler {};   ///< mediator for sharing data
  };
}
