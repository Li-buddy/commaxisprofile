#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declare the operation of scaling factor configuration.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"
#include "comm/axisprofile/datatypes/scalingFactor_generated.h"

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

  /// @class NodeProfilesScalingFactorCfg
  /// @brief This is the node class of scaling factor.
  class NodeProfilesScalingFactorCfg : public NodeProfilesBase
  {
  public: // methods

    /// Constructor
    /// @param[in]   mgr   Pointer to the provider manager
    explicit NodeProfilesScalingFactorCfg(ProviderMgr* mgr);

    /// Destructor
    virtual ~NodeProfilesScalingFactorCfg() noexcept override = default;

    /// @brief This method response read operation(override).
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// @brief This method response Metadata request(override).
    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

    /// @brief Response for write request
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[in]    fbBuilder        Flatbuffers builder
    /// @param[in]    scalingFactor    The scaling factor data.
    /// @return Offset of the generated flatbuffer
    static flatbuffers::Offset<comm::axisprofile::fbtypes::ScalingFactor> generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbBuilder, const ScalingFactor& scalingFactor);

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            fbs profile to extract the data from.
    /// @param[out]   scalingFactor   Structure of the scaling factor data.
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::ScalingFactor* data, ScalingFactor& scalingFactor);

  private: // methods

    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]    data            Variant to extract the data from.
    /// @param[out]   scalingFactor   Structure of the scaling factor data.
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, ScalingFactor& scalingFactor, const ResponseCallback& callback);    

  };
}
