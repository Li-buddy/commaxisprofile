#pragma once

/// @file          node_system_boot_timeout.h
///
/// This header file is part of the axisprofile library and defines the node information for the max booting timeout
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/node_base.h"


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
      
      /// @brief This is the max timeout node class.
      class NodeSystemBootTimeout : public NodeBase
      {
      public: // methods
        /// Constructor
        explicit NodeSystemBootTimeout(ProviderMgr* mgr);

        /// Destructor
        virtual ~NodeSystemBootTimeout() noexcept override = default;

        void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;
      };
    }
  }
}
