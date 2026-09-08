#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and defines the node for profile basic data configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2021-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"
#include "../node_profiles.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestProviderProfilesBasicData;

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

  /// @brief This is the node class to get basic data of a profile.
  class NodeProfilesBasicData : public NodeProfilesBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr   Pointer to the provider manager
    explicit NodeProfilesBasicData(ProviderMgr* mgr);

    /// Destructor
    virtual ~NodeProfilesBasicData() noexcept override = default;

    /// @brief This method response read operation(override).
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;        

    /// @brief This method response Metadata request(override).
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;        

  private: // methods

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[out]  data                Variant to to store the data in.
    /// @param[in]   profileFbData       Profile basic data
    /// @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult generateFlatbufferData(comm::datalayer::Variant& data, const NodeProfiles::profileFbData& profileBasicData);

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[in]    fbBuilder        Flatbuffers builder
    /// @param[in]    data       The basic profile data.
    /// @return Offset of the generated flatbuffer
    static flatbuffers::Offset<comm::axisprofile::fbtypes::profile> generateTypeFlatbufferData(flatbuffers::FlatBufferBuilder& fbBuilder, const NodeProfiles::profileFbData& data);

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            Variant to extract the data from.
    /// @param[out]   profileFbData   Structure of the basic profile data.
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, NodeProfiles::profileFbData& profileFbData, const ResponseCallback& callback);

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            fbs profile to extract the data from.
    /// @param[out]   profileFbData   Structure of the basic profile data.
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::profile* data, NodeProfiles::profileFbData& profileFbData);

  private: //Data

    friend class ::TestProviderProfilesBasicData;   ///< Unittest of this class is a friend
  };
}
