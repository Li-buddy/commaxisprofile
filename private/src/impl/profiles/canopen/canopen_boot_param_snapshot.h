#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the Canopen
/// boot parameter snapshot reader.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/boot_param_snapshot_base.h"
#include "profiles/canopen/canopen_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenBootParmSnapshot;
namespace comm::axisprofile
{
  class IAxisProfileSharingHandler;
}

namespace comm::axisprofile::canopen
{
  /// @brief This class reads the persisted Canopen boot parameter snapshot payload from the configuration file.
  class CanopenBootParamSnapshot : public BootParamSnapshotBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileBasicInfo Profile context.
    explicit CanopenBootParamSnapshot(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~CanopenBootParamSnapshot() noexcept override = default;

    /// @brief Get one CoE boot parameter by object entry. Try snapshot cache first, then fall back to reading from drive.
    /// @param[in] objEntry CoE object entry.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @param[in] forceReadFromDrive Flag to force reading the parameter from drive.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameter(const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromDrive = false);

    /// @brief Shared helper to read one CoE parameter via snapshot context from sharing handler.
    /// @param[in] sharingHandler Sharing handler to retrieve snapshot/device-param interfaces.
    /// @param[in] traceMsg Trace prefix message.
    /// @param[in] objEntry CoE object entry.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @return Return value indicating success or error.
    static AxisProfileResult getParameterValueFromSnapshot(const comm::axisprofile::IAxisProfileSharingHandler* sharingHandler,
                                 const char* traceMsg, const ObjectDirEntry& objEntry, std::uint8_t* bufPtr, std::size_t bufLen);

  private: // methods
    friend class ::TestCanopenBootParmSnapshot;

    const BootParamDescriptorBase* getBootParamDescriptor(const BootParamId& paramId) const override;
    std::string parameterToString(const BootParamId& paramId) const override;
    std::unique_ptr<ParamReqDataBase> generateReqData(const BootParamId& paramId) const override;

  };
}
