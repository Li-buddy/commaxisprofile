#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_system_creator.h
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
//! class to create all system related nodes.
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


      //! @class NodeSystemCreator
      //! @brief This is the creator class of all system related nodes.
      class NodeSystemCreator
      {
      public: // methods

        //! Constructor
        NodeSystemCreator() = default;

        //! Destructor
        virtual ~NodeSystemCreator() noexcept = default;

        //! @brief add all nodes to the provider manager
        //! @param[in]   mgr   pointer to the provider manager
        void addNodes(ProviderMgr *mgr);
      };
    }
  }
}
