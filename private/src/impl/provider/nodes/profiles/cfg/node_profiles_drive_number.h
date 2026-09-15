#pragma once

/// @file
///
/// This is a drive number node profiles header file
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
// Name Spaces
// ---------------------------------------------------------------------------


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

      //! @class NodeProfilesDriveNumber
      //! @brief This is the node class to set/get the drive number of a profile.
      class NodeProfilesDriveNumber : public NodeProfilesBase
      {
      public: // methods

        /// Constructor
        /// @param[in]   mgr   Pointer to the provider manager
        explicit NodeProfilesDriveNumber(ProviderMgr* mgr);

        /// Destructor
        virtual ~NodeProfilesDriveNumber() noexcept override = default;

        void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;
        void onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

        /// @brief Response for metadata request
        /// @param[in]   address    address describe the uri
        /// @param[in]   callback   execute after the response
        void onMetadata(const std::string& address, const ResponseCallback& callback) final;
      };
    }
  }
}
