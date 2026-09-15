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
//! This header file is part of the axisprofile library and defines the node
//! to set a configuration URI for a specific profile.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/profiles/node_profiles_base.h"


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

      //! Operation which should be executed when calling onWrite() of class NodeProfilesSetURI.
      enum class NodeOperation
      {
        INBUFFER = 0,   //!< Save a single motion object
        OUTBUFFER       //!< Load a single motion object
      };

      // ---------------------------------------------------------------------------
      // Forward references
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of classes and structures
      // ---------------------------------------------------------------------------

      //! @class NodeProfilesSetURI
      //! @brief This is the generic node to configure a profile URI.
      class NodeProfilesSetURI : public NodeProfilesBase
      {
      public: // methods

        //! Constructor
        //! @param[in]   mgr         pointer to the provider manager
        //! @param[in]   operation   node operation which will be executed
        NodeProfilesSetURI(ProviderMgr *mgr, const NodeOperation operation);

        //! Destructor
        virtual ~NodeProfilesSetURI() noexcept override = default;

        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onWrite(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;

        /// @brief Response for metadata request
        /// @param[in]   address    address describe the uri
        /// @param[in]   callback   execute after the response
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;

      private: // data

        const NodeOperation m_operation;   //!< Operation which will be executed in onWrite()
      };
    }
  }
}
