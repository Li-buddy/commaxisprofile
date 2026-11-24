#pragma once
/// @file  node_profiles_scalingNumerator.h
///
/// This header file is part of the axisprofile library and defines
/// the node for scaling numerator.
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
            
      /// @brief This is the node class to set/get the scaling numerator of a profile.
      class NodeProfilesScalingNumerator : public NodeProfilesBase
      {
      public: // methods

        /// @brief Constructor
        /// @param[in]   mgr   Pointer to the provider manager
        explicit NodeProfilesScalingNumerator(ProviderMgr* mgr);

        /// @brief Destructor
        virtual ~NodeProfilesScalingNumerator() noexcept override = default;

        void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

        void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

        void onMetadata(const std::string& address, const ResponseCallback& callback) final;
      };
    }
  }
}
