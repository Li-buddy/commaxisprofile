#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declares the
/// configuration node for all profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "../node_profiles.h"
#include "comm/axisprofile/datatypes/profileConfig_generated.h"
#include "node_profiles_category.h"
#include "node_profiles_input_mapping.h"
#include "node_profiles_output_mapping.h"
#include "node_profiles_scalingStrategyCfg.h"
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

  /// @brief This is the configuration node class for all profiles.
  class NodeProfilesConfig : public NodeProfilesBase
  {
  public: // types

    /// @brief Internal structure that stores the content of the config data flatbuffer
    struct profileConfig
    {
      NodeProfiles::profileFbData m_profile {};                                     ///< Basic profile data (name, type, network technology)
      std::string m_deviceName {};                                                  ///< Name of the device
      std::uint32_t m_deviceAddress {};                                             ///< Address of the device
      std::string m_inputBuffer {};                                                 ///< Datalayer address of the input telegram
      std::string m_outputBuffer {};                                                ///< Datalayer address of the output telegram
      std::vector<InputMappingEntry>  m_inputValueMapping {};                       ///< Complete input mapping
      std::vector<OutputMappingEntry> m_outputValueMapping {};                      ///< Complete output mapping
      AxisProfileScalingStrategyCfg   m_scalingStrategyCfg;                         ///< Scaling strategy configuration
      std::uint8_t m_driveNumber {};                                                ///< Number of the drive
      AxisProfileCategory m_profileCategory {AxisProfileCategory::CATEGORY_DRIVE};  ///< category of profile
      bool m_checksumEnable {true};                                                 ///< whether checksum activated
      std::uint32_t m_outOfRangeErrLimit {1};                                            ///< maximum number of out-of-range values before error is reported
    };

  public: // methods

    /// Constructor
    /// @param[in]   mgr     pointer to the provider manager
    explicit NodeProfilesConfig(ProviderMgr* mgr);

    /// Destructor
    virtual ~NodeProfilesConfig() noexcept override = default;

    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
    void onMetadata(const std::string &address, const ResponseCallback &callback) final;
       
    /// @brief This method extracts the received flatbuffer data.
    /// @param[in]   data                Variant to extract the data from.
    /// @param[out]  profileConfigData   Extracted profile configuration data
    /// @param[in]   callback            This is the callback of the request which will only be called on error
    /// @return Return value indicating success or error.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, profileConfig& profileConfigData, const ResponseCallback &callback);

  private: // methods

    /// @brief This method converts the internal data into a flatbuffer.
    /// @param[out]  data                Variant to store the data in.
    /// @param[in]   profileConfigData   Profile configuration data
    /// @param[in]   callback            This is the callback of the request which will only be called on error
    /// @return Return value indicating success or error.
    AxisProfileResult generateFlatbufferData(comm::datalayer::Variant& data, const profileConfig& profileConfigData, const ResponseCallback &callback);
  };
}
