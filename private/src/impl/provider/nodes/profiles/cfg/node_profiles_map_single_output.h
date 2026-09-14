#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_profiles_map_single_output.h
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
//! to configure the mapping of a single output value.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    enum class OutputMappingValueID;
  }
}


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

      //! @class NodeProfilesMapSingleOutput
      //! @brief This is the datalayer node to configure the mapping of a single output value.
      class NodeProfilesMapSingleOutput : public NodeProfilesBase
      {
      public: // methods

        //! Constructor
        //! @param[in]   mgr         pointer to the provider manager
        explicit NodeProfilesMapSingleOutput(ProviderMgr* mgr);

        //! Destructor
        virtual ~NodeProfilesMapSingleOutput() noexcept override = default;

        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;
        void onBrowse(const std::string& address, const ResponseCallback& callback) final;

      private: // data

        std::string m_mappingValue;                    //!< Value of the mapping item
      };
    }
  }
}
