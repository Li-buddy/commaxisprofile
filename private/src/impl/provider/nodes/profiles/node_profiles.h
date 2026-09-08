#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_profiles_set_uri.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and defines the nodes for 
//! the actual value of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"
#include "comm/axisprofile/datatypes/profile_generated.h"


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

      //! @class NodeProfiles
      //! @brief This is the base node to create, browse and delete a profiles.
      class NodeProfiles : public NodeProfilesBase
      {
      public: // types

        //! @brief Internal structure that stores the content of the profile flatbuffer
        struct profileFbData
        {
          std::string m_profileName{};          //!< Name of the profile
          AxisProfileType m_profileType{};      //!< Type of the profile
        };

      public: // methods

        //! Constructor
        //! @param[in]   mgr         pointer to the provider manager
        explicit NodeProfiles(ProviderMgr *mgr);

        //! Destructor
        virtual ~NodeProfiles() noexcept override = default;

        void onCreate(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onRemove(const std::string &address, const ResponseCallback &callback) final;
        void onBrowse(const std::string &address, const ResponseCallback &callback) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;

        //! @brief This method extracts the received flatbuffer data.
        //! @param[in]    data            Variant to extract the data from.
        //! @param[out]   profileFbData   Structure of the basic profile data.
        //! @return Return value indicating success or error.
        static AxisProfileResult extractFlatbufferData(const comm::axisprofile::fbtypes::profile* data, profileFbData& profileFbData);

        //! @brief This method converts the internal data into a flatbuffer.
        //! @param[in]    fbb        Flatbuffers builder
        //! @param[in]    data       The basic profile data.
        //! @return Offset of the generated flatbuffer
        static flatbuffers::Offset<comm::axisprofile::fbtypes::profile> generateFlatbufferData(flatbuffers::FlatBufferBuilder &fbb, const profileFbData& data);
      };
    }
  }
}
