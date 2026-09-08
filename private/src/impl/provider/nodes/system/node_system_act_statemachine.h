#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_system_act_statemachine.h
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

      //! @class NodeSystemActStateMachine
      //! @brief This is the act state node class.
      class NodeSystemActStateMachine : public NodeBase
      {
      public: // methods
        //! Constructor
        explicit NodeSystemActStateMachine(ProviderMgr *mgr);

        //! Destructor
        virtual ~NodeSystemActStateMachine() noexcept override = default;

        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;
      };
    }
  }
}
