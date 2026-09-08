#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_system_cmd_statemachine.h
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
//! the commanding of the axisprofile state machine.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/node_base.h"


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

      //! @class NodeSystemCmdStateMachine
      //! @brief This is the state node class.
      class NodeSystemCmdStateMachine : public NodeBase
      {
      public: // methods
        //! Constructor
        explicit NodeSystemCmdStateMachine(ProviderMgr *mgr);

        //! Destructor
        virtual ~NodeSystemCmdStateMachine() noexcept override = default;

        void onCreate( const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback ) final;
        void onBrowse( const std::string &address, const ResponseCallback &callback ) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;
      };
    }
  }
}
