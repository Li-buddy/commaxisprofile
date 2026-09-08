#pragma once
/// @file
///
/// This header file is part of the axisprofile library and check the offset between control and device of a specific profile. 
///
/// @copyright (C) 2018-2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

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

      // ---------------------------------------------------------------------------
      // Forward references
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of classes and structures
      // ---------------------------------------------------------------------------

      /// @class NodeProfilesDeviceOffset
      /// @brief This is the device offset node class for all profiles.
      class NodeProfilesDeviceOffset : public NodeProfilesBase
      {
        public: // methods

        /// Constructor
        /// @param[in]   mgr     pointer to the provider manager
        explicit NodeProfilesDeviceOffset(ProviderMgr* mgr);

        /// Destructor
        virtual ~NodeProfilesDeviceOffset() noexcept override = default;

        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;
      };
    }
  }
}
