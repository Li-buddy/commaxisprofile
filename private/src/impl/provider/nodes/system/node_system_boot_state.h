#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_system_boot_state.h
//!
//! @author     DC-AE/ENG6-CN
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and defines the nodes for 
//! the boot state of the axisprofile.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/node_base.h"
#include "configuration/i_boot_mgr.h"
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
      
      //! @class NodeSystemBootState
      //! @brief This is the act state node class.
      class NodeSystemBootState : public NodeBase
      {
      public: // methods
        //! Constructor
        explicit NodeSystemBootState(ProviderMgr* mgr);

        //! Destructor
        virtual ~NodeSystemBootState() noexcept override = default;

        void onRead(const std::string &address, const comm::datalayer::Variant* data, const ResponseCallback &callback) final;
        void onMetadata(const std::string &address, const ResponseCallback &callback) final;
        
      protected:
       
      private:

      //! @brief This methods creates flatbuffer
      //! @param[in]   address    address of the datalayer node
      //! @param[out]  outFb      output variant
      //! @param[in]   inFbVar    input variant
      //! @param[in]   bootMgr    pointer to boot manager
      //! @return Return value indicating success or error.      
       comm::datalayer::DlResult createFB( const std::string &address, comm::datalayer::Variant &outFb, const comm::datalayer::Variant* inFbVar, const IBootMgr* bootMgr);
      };
    }
  }
}
