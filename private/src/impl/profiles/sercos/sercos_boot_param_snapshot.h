#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the Sercos
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
#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "profiles/boot_param_snapshot_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  class IAxisProfileSharingHandler;
}

namespace comm::axisprofile::sercos
{
  /// @brief This class stores persisted Sercos boot parameters and reads raw boot
  ///        parameters from the drive on demand.
  class SercosBootParamSnapshot : public BootParamSnapshotBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileBasicInfo Profile context.
    explicit SercosBootParamSnapshot(ProfileBasicInfo& profileBasicInfo);

    /// @brief Destructor
    ~SercosBootParamSnapshot() noexcept override = default;

    /// @brief get one SoE boot parameter by IDN, firstly get from cache, if not hit cache, read from deviece.
    /// @param[in] idn SoE IDN.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @param[in] forceReadFromeDrive Flag to indicate whether to force reading the parameter from drive, default is false.
    /// @param[in] elementFlag SoE element flag of the parameter value/maxValue access.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameter(std::uint16_t idn,  std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromeDrive = false,
                     comm::ethercat::master::fbs::ElementFlags elementFlag = comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);

    /// @brief Shared helper to read one SoE parameter via snapshot context from sharing handler.
    /// @param[in] sharingHandler Sharing handler to retrieve snapshot/device-param interfaces.
    /// @param[in] traceMsg Trace prefix message.
    /// @param[in] idn SoE IDN.
    /// @param[out] bufPtr Raw target buffer.
    /// @param[in] bufLen Raw target buffer size.
    /// @param[in] elementFlag SoE element flag of the parameter value/maxValue access.
    /// @return Return value indicating success or error.
    static AxisProfileResult getParameterValueFromSnapshot(const comm::axisprofile::IAxisProfileSharingHandler* sharingHandler,
                                                           const char* traceMsg, std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen,
                                                           comm::ethercat::master::fbs::ElementFlags elementFlag = comm::ethercat::master::fbs::ElementFlags::ElementFlags_value);

  private: // methods

    const BootParamDescriptorBase* getBootParamDescriptor(const BootParamId& paramId) const override;
    std::string parameterToString(const BootParamId& paramId) const override;
    std::unique_ptr<ParamReqDataBase> generateReqData(const BootParamId& paramId) const override;

  private: // data

    comm::ethercat::master::fbs::ElementFlags m_elementFlag {comm::ethercat::master::fbs::ElementFlags::ElementFlags_value};
  };
}
