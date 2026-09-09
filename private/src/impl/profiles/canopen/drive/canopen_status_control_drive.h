///
/// @brief This file is part of the axisprofile library and implements the status- and controlword handling for CiA402.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/canopen/canopen_defs.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "profiles/axisprofile_command_statemachine.h"
#include "profiles/category/drive/axisprofile_status_control_drive.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenStatusControl;

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

  /// @brief set Operation Mode
  enum class SetOpModeStep : std::uint16_t
  {
    SET_MODE = 0, ///< step set mode
    CHECK_MODE,   ///< step check mode
    STEP_MAX      ///< step max
  };

  /// @brief The state of setting and checking operation mode.
  struct SetAndCheckOpModeState
  {
    SetOpModeStep stepSetOpMode {SetOpModeStep::SET_MODE};            ///< The step of setting operation mode.
    AsyncCommandStatemachine stateReadingOpModeReq {};                ///< The state of reading request for current operation mode.
    AsyncCommandStatemachine stateWritingOpModeReq {};                ///< The state of writing request for current operation mode.
    std::uint8_t tmpOpMode{0};                                        ///< The temporary value for reading operation mode.
    AxisProfileOperationMode checkedOpMode {AxisProfileOperationMode::UNDEFINED};  ///< The checked operation mode after setting operation mode.
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class implements the status- and controlword handling for sercos.
  class CanopenStatusControlDrive : public AxisProfileStatusControlDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo   Pointer to the Profile Basic Information
    /// @param[in]   at                 Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt                Pointer to the Master Data Telegram (MDT)
    CanopenStatusControlDrive(ProfileBasicInfo& profileBasicInfo,
                              const AcknowledgeTelegram* at,
                              MasterDataTelegram* mdt);

    /// @brief Destructor
    ~CanopenStatusControlDrive() noexcept override = default;

    // -------------------------------------- Statusword ----------------------------------------
    std::uint16_t getStatusWord() const final;
    bool checkError() const final;
    bool cyclicStandStillCheck() const final;
    bool checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo) final;
    AxisProfileOperationMode getOperationMode() const final;
    AxisProfileResult analyzeStatusWord() final;
    bool isDeviceFollowsControl() const final;
    bool deviceHasWarning() const final {return m_canStatusWord.m_warning;}
    ReadyToOperate checkReadyToOperate() final;

    // -------------------------------------- Controlword ---------------------------------------
    std::uint16_t getControlWord() const final;
    AxisProfileResult setControlWord(std::uint16_t controlWord) final;
    AxisProfileResult setOperationMode(const AxisProfileOperationMode opMode) final;
    AxisProfileResult setPowerMode(const AxisProfilePowerMode powerMode) final;

    // -------------------------------------- Drive Halt ---------------------------------------
    AxisProfileResult setDriveHaltMode(const bool haltEnable) final;
    AxisProfileResult getDriveHaltMode(bool& isHaltEnabled) const final;

  private: // methods

    void loadBootingTasks() final;

    /// @brief power on procedure to execute
    /// @return    Returns value indicating success or failure.
    AxisProfileResult powerOnProcedure();

    /// @brief power off procedure to execute
    /// @return    Returns value indicating success or failure.
    AxisProfileResult powerOffProcedure();

    /// @brief Check the current operation mode, called after setting operation mode.
    /// @return    Returns the enumeration value indicating success or failure.
    AxisProfileResult checkOperationMode();

    /// @brief  This method checks operation mode available condition.
    /// @param[in] supported        The operation mode is supported in drive
    /// @param[in] assignedInput    The input PDOs are assigned for this mode
    /// @param[in] assignedOutput   The output PDOs are assigned for this mode
    /// @param[out]errorInfo        Error information of operation mode
    /// @return Return value indicating supported or not supported.
    bool checkOpModeAvailable(AvailabilityInfo supported, bool assignedInput, bool assignedOutput, OpModeErrorInfo& errorInfo);

    /// @brief  This method converts a given general operation mode into the correct canopen operation mode.
    /// @param[in]      generalOpMode       Generalized operation mode
    /// @param[out]     mode                Specific canopen operation mode
    /// @return    Returns the enumeration value indicating success or failure.
    AxisProfileResult determineCanopenOpMode(const AxisProfileOperationMode generalOpMode, std::uint8_t& mode);

  private: // data

    friend class ::TestCanopenStatusControl;                            // unit test of CanopenStatusControlDrive        
    IFieldbusAdapter* const m_fieldbusAdapter {};                       ///< Pointer to fieldbus adapter
    const AcknowledgeTelegram* m_telegramAT {};                         ///< Pointer to the Acknowledge Telegram (AT)
    MasterDataTelegram* m_telegramMDT {};                               ///< Pointer to the Master Data Telegram (MDT)
    StatusWord m_canStatusWord;                                         ///< Current statusword
    ControlWord m_canControlWord;                                       ///< Current controlword
    std::int16_t m_switchTimeOut {CANOPEN_SWITCH_TIMEOUT_THRESHOLD};    ///< timeout
    SetAndCheckOpModeState m_setAndCheckOpModeState {};                 ///< The state of setting and checking operation mode.

    FsaState m_fsaState {FsaState::NOT_READY_TO_SWITCH_ON};             ///< The FSA state of CiA402 drive
    FsaState m_fsaStateOld {FsaState::NOT_READY_TO_SWITCH_ON};          ///< The FSA state of CiA402 drive of last cycle
  };
}
