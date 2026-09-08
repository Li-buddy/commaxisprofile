#pragma once
/// @file
///
/// This header file is part of the axisprofile library and defines the node information for device name node.
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
#include "profiles/value_mapping/i_mapping_handler.h"

// Generated
#include "comm/axisprofile/datatypes/profileConfig_generated.h"

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

      /// @class NodeProfilesDeviceName
      /// @brief This is the datalayer node to configure the axisprofile output mapping.
      class NodeProfilesDeviceName : public NodeProfilesBase
      {
      public: // methods

        /// @param[in]   mgr         pointer to the provider manager
        explicit NodeProfilesDeviceName(ProviderMgr* mgr);
        
        virtual ~NodeProfilesDeviceName() noexcept override = default;

        void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
        void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;
      };
    }
  }
}
