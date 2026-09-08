#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_profiles_input_mapping.h
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
//! This header file is part of the axisprofile library and defines the node
//! for the input mapping configuration.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"
#include "profiles/value_mapping/mapping_handler.h"

// Generated
#include "comm/axisprofile/datatypes/valueMapping_generated.h"


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

      //! @class NodeProfilesInputMapping
      //! @brief This is the datalayer node to configure the axisprofile input mapping.
      class NodeProfilesInputMapping : public NodeProfilesBase
      {
      public: // methods

        //! Constructor
        //! @param[in]   mgr         pointer to the provider manager
        explicit NodeProfilesInputMapping(ProviderMgr *mgr);

        //! Destructor
        virtual ~NodeProfilesInputMapping() noexcept override = default;

        void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;        

        //! @brief This method extracts the received flatbuffer data.
        //! @param[in]    data       Variant to extract the data from.
        //! @param[out]   mapping    Vector of all extracted input mappings.
        //! @return Return value indicating success or error.
        static AxisProfileResult extractFlatbufferData(const fbtypes::valueMapping* data, std::vector< comm::axisprofile::InputMappingEntry > &mapping);

        //! @brief This method converts the internal data into a flatbuffer.
        //! @param[in]    fbb        Flatbuffers builder
        //! @param[in]    data       Vector containing the whole input mappings.
        //! @return Offset of the generated flatbuffer
        static flatbuffers::Offset<fbtypes::valueMapping> generateFlatbufferData(flatbuffers::FlatBufferBuilder &fbb, const std::vector<comm::axisprofile::InputMappingEntry>& data);

      };
    }
  }
}
