#pragma once
/// @file  node_profiles_scalingStrategyCfg.h
///
/// This header file is part of the axisprofile library and defines
/// the node for scaling strategy configuration.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"
#include "../node_profiles.h"
#include "node_profiles_scalingStrategy.h"
#include "node_profiles_deviceScalingType.h"
#include "node_profiles_scalingFactor.h"
#include "comm/axisprofile/datatypes/profileScalingCfg_generated.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace provider
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

      /// @brief This is the node class to get/set scaling strategy data of a profile.
      class NodeProfilesScalingStrategyCfg : public NodeProfilesBase
      {
      public: // methods

        ///@brief Constructor
        /// @param[in]   mgr   Pointer to the provider manager
        explicit NodeProfilesScalingStrategyCfg(ProviderMgr* mgr);

        ///@brief Destructor
        virtual ~NodeProfilesScalingStrategyCfg() noexcept override = default;

        /// @brief Response for read request
        void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

        /// @brief Response for write request
        void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

        /// @brief Response for metadata request
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;

      private: // methods        

        /// @brief This method extracts the received flatbuffer data.
        /// @param[in]    data          Variant to extract the data from.
        /// @param[out]   profileStrategyCfg   Structure of the scaling strategy config data.
        /// @param[in]    callback      This is the callback of the request which will only be called on error
        /// @return Return value indicating success or error.
        static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, comm::axisprofile::AxisProfileScalingStrategyCfg& profileStrategyCfg, const ResponseCallback& callback);

      public:
        /// @brief This method extracts the received flatbuffer data.
        /// @param[in]    data            fbtypes::profileTypeData* to extract the data from.
        /// @param[out]   profileStrategyCfg     Structure of the scaling strategy config data.
        /// @return Return value indicating success or error.
        static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::profileScalingCfg* data, comm::axisprofile::AxisProfileScalingStrategyCfg& profileStrategyCfg);

        /// @brief This method converts the internal data into a flatbuffer.
        /// @param[in]  fbb                    flatbuffer to to store the data in.
        /// @param[in]  profileStrategyCfg     Structure of the scaling strategy config data.
        /// @return Return flatbuffer offset data.
        static flatbuffers::Offset<comm::axisprofile::fbtypes::profileScalingCfg>  generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const comm::axisprofile::AxisProfileScalingStrategyCfg& profileStrategyCfg);
      };
    }
  }
}
