#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_profiles_creator.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and provides a helper
//! class to create all profile related nodes.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace provider
    {
      class ProviderMgr;
    }
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


      //! @class NodeProfilesCreator
      //! @brief This is the creator class of all profile related nodes.
      class NodeProfilesCreator
      {
      public: // methods

        //! Constructor
        NodeProfilesCreator() = default;

        //! Destructor
        virtual ~NodeProfilesCreator() noexcept = default;

        //! @brief add all nodes to the provider manager
        //! @param[in]   mgr   pointer to the provider manager
        void addNodes(ProviderMgr *mgr);
      };
    }
  }
}
