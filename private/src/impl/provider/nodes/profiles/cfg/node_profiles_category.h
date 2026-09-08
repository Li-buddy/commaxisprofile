#pragma once
/// @file
///
/// @brief This header file is part of the axisprofile library and defines the node information for profile category node.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/datatypes/profileCategory_generated.h"
#include "provider/nodes/profiles/node_profiles_base.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the datalayer node to configure the profile category.
  class NodeProfilesCategory : public NodeProfilesBase
  {
  public: // methods

    /// @param[in] mgr Pointer to the provider manager
    explicit NodeProfilesCategory(ProviderMgr* mgr);
    /// @brief Destructor
    virtual ~NodeProfilesCategory() noexcept override = default;

    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[in]  fbb                flatbuffer to to store the data in.
    /// @param[in]  profileCategory        Profile category
    /// @return Return flatbuffer offset data.
    static flatbuffers::Offset<comm::axisprofile::fbtypes::ProfileCategory>  generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, comm::axisprofile::AxisProfileCategory profileCategory);

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            Variant to extract the data from.
    /// @param[out]   profileCategory Category of the profile.
    /// @param[in]    callback        This is the callback of the request which will only be called on error
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, comm::axisprofile::AxisProfileCategory& profileCategory, const ResponseCallback& callback);

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data             fbtypes::profileTypeData* to extract the data from.
    /// @param[out]   profileCategory  category of the profile.
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::ProfileCategory* data, comm::axisprofile::AxisProfileCategory& profileCategory);
  };
}
