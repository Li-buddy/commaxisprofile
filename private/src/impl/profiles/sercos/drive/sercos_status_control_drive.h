#pragma once

/// @file
///
/// This header file is part of the axisprofile library and declare SercosStatusControlDrive.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/drive/axisprofile_status_control_drive.h"
#include "profiles/sercos/sercos_defs.h"
#include "profiles/profile_defs.h"
// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSercosStatusControl;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------


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

  /// @class SercosStatusControlDrive
  /// This class implements the status- and controlword handling for sercos.
  class SercosStatusControlDrive : public AxisProfileStatusControlDrive
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   profileBasicInfo Reference to the Profile Basic Information       
    /// @param[in]   at               Pointer to the Acknowledge Telegram (AT)
    /// @param[in]   mdt              Pointer to the Master Data Telegram (MDT)
    SercosStatusControlDrive(ProfileBasicInfo& profileBasicInfo, const comm::axisprofile::AcknowledgeTelegram* at, comm::axisprofile::MasterDataTelegram* mdt);

    /// @brief Destructor
    virtual ~SercosStatusControlDrive() noexcept override = default;

    // -------------------------------------- Statusword ----------------------------------------

    std::uint16_t getStatusWord() const final;
    bool checkError() const final;
    bool cyclicStandStillCheck() const final;
    bool checkOperationModeSupported(const AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo) final;
    AxisProfileOperationMode getOperationMode() const final;
    void confirmOperationModeFromDevice(AxisProfileOperationMode operationMode) final;
    AxisProfileResult analyzeStatusWord() final;
    bool isDeviceFollowsControl() const final;
    bool deviceHasWarning() const final {return m_sercosStatusWord.m_warningInC2D;}
    ReadyToOperate checkReadyToOperate() final;

    // -------------------------------------- Controlword ---------------------------------------

    std::uint16_t getControlWord() const final;
    AxisProfileResult setControlWord(std::uint16_t controlWord) final;
    AxisProfileResult setOperationMode(const AxisProfileOperationMode opMode) final;
    AxisProfileResult setPowerMode(const AxisProfilePowerMode powerMode) final;

    // -------------------------------------- Drive Halt ---------------------------------------

    AxisProfileResult setDriveHaltMode(const bool haltEnable) final;
    AxisProfileResult getDriveHaltMode(bool& isHaltEnabled) const final;

  private: // method

    void loadBootingTasks() final;

    /// @brief  This method is called after new AT telegram data is received. It interprets the raw telegram data
    ///         to determine the current operation mode, maps it to the general base class operation mode and sets it.
    ///
    /// @return       Return value indicating success (>=0) or error (<0).  
    AxisProfileResult determineOpModeFromAT();

    /// @brief  This method is called after new AT telegram data is received. It interprets the raw telegram data
    ///         to determine the current state of the 'ready to operate' bits. It also maps it to the general base class power mode and sets it.
    ///
    /// @return       Return value indicating success (>=0) or error (<0).  
    AxisProfileResult determineReadyToOperateFromAT();

    /// @brief  This method converts a given general operation mode into the correct sercos operation mode.
    /// @param[in]      generalOpMode       Generalized operation mode
    /// @param[out]     sercosOpMode        Specific sercos operation mode
    ///
    /// @return         Return value indicating success (>=0) or error (<0).
    AxisProfileResult determineSercosOpMode(const AxisProfileOperationMode generalOpMode, OperationMode* sercosOpMode);

    /// @brief  This method determines operation mode if supported and generates the error information.
    /// @param[in]      opModeAvailable         Operation mode configured in drive
    /// @param[in]      opModeInputAssigned     Operation mode assigned in input profile variable
    /// @param[in]      opModeOutputAssigned    Operation mode assigned in output profile variable
    /// @param[out]     errorInfo               Error information of operation mode
    ///
    /// @return         Return value indicating supported or not supported.
    bool determineOpModeSupported(const bool opModeAvailable, const bool opModeInputAssigned, const bool opModeOutputAssigned, OpModeErrorInfo& errorInfo);

    /// @brief  This method is used to update operation mode supported information.
    /// @return       Return value indicating success (>=0) or error (<0).
    AxisProfileResult updateOpModeSupportedInfo();

  private: // data

    friend class ::TestSercosStatusControl;

    const comm::axisprofile::AcknowledgeTelegram* m_telegramAT{};   ///< Pointer to the Acknowledge Telegram (AT)
    comm::axisprofile::MasterDataTelegram* m_telegramMDT{};         ///< Pointer to the Master Data Telegram (MDT)

    StatusWord  m_sercosStatusWord;                       ///< Current statusword (S-0-0135)
    ControlWord m_sercosControlWord;                      ///< Current controlword (S-0-0134)
    OpModeAvailInfoSercos* m_opModeAvailInfo { };         ///< Check if operation mode is available at drive side
    AxisProfileOperationMode  m_checkedOperationMode {AxisProfileOperationMode::UNDEFINED};  ///< The checked operation mode after setting operation mode.
  };
}
