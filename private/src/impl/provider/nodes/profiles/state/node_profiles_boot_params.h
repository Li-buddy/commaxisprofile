#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and defines the
/// node for persisted boot parameter snapshots.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_device_boot_param_snapshot.h"
#include "provider/nodes/profiles/node_profiles_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
{
  /// @brief This is the node class to get/set persisted boot parameter snapshots of a profile.
  class NodeProfilesBootParams : public NodeProfilesBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] mgr Pointer to the provider manager
    explicit NodeProfilesBootParams(ProviderMgr* mgr);

    /// @brief Destructor
    ~NodeProfilesBootParams() noexcept override = default;

    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

    /// @brief conver the boot parameter item to a readable string value
    /// @param[in] item  to be converted
    /// @return the readable string value of the boot parameter item
    static std::string toReadableValue(const BootParamItem& item);
  };
}
