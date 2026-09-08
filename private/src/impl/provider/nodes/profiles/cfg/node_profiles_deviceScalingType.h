#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines
/// the node for device scaling type configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2022-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../node_profiles.h"
#include "comm/axisprofile/datatypes/profileDeviceScalingType_generated.h"
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
            
  /// @brief This is the node class to set/get the device scaling type.
  class NodeProfilesDeviceScalingType : public NodeProfilesBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   mgr   Pointer to the provider manager
    explicit NodeProfilesDeviceScalingType(ProviderMgr* mgr);

    ///@brief Destructor
    virtual ~NodeProfilesDeviceScalingType() noexcept override = default;

    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            fbtypes::profileTypeData* to extract the data from.
    /// @param[out]   profileDeviceScalingType     device scaling type
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::profileDeviceScalingType* data, comm::axisprofile::AxisProfileDeviceScalingType& profileDeviceScalingType);

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[in]  fbb                flatbuffer to store the data in.
    /// @param[in]  scalingType        Profile type
    /// @return Return flatbuffer offset data.
    static flatbuffers::Offset<comm::axisprofile::fbtypes::profileDeviceScalingType>  generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, comm::axisprofile::AxisProfileDeviceScalingType scalingType);

  private: // methods        

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data          Variant to extract the data from.
    /// @param[out]   profileDeviceScalingType   Structure of the basic profile data.
    /// @param[in]    callback      This is the callback of the request which will only be called on error
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, comm::axisprofile::AxisProfileDeviceScalingType& profileDeviceScalingType, const ResponseCallback& callback);
  };
}
