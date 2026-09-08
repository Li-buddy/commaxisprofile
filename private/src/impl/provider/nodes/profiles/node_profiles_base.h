#pragma once

// ---------------------------------------------------------------------------
//
//! @file       node_profiles_base.h
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
//! This header file is part of the axisprofile library and defines a base
//! class for all profile nodes.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/nodes/node_base.h"
#include "provider/provider_helper.h"
#include "profiles/i_axisprofile_internal.h"
#include "manager/i_axisprofile_manager_internal.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestProviderNodeProfilesBase;


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

      //! @class NodeProfilesBase
      //! @brief This is the base class for all profile datalayer nodes.
      class NodeProfilesBase : public NodeBase
      {
      public: // methods

        //! Constructor
        //! @param[in]   mgr         pointer to the provider manager
        explicit NodeProfilesBase(ProviderMgr *mgr);

        //! Destructor
        virtual ~NodeProfilesBase() noexcept override = default;

        virtual void onMetadata(const std::string &address, const ResponseCallback &callback) override;

      protected: // methods

        //! @brief This method uses the given datalayer address string to get the associated axisprofile.
        //! @param[in]   address    Address of the node
        //! @param[in]   callback   This is the callback of the request which will only be called on error
        //! @return Pointer to the associated profile instance (success) or nullptr (error).
        IAxisProfileInternal* getProfileByAddress(const std::string& address, const ResponseCallback& callback);

        /// @brief This method uses the given datalayer address string to get the associated axisprofile.
        /// @param[in]   address      Address of the node
        /// @param[out]  axisprofile  Instance of axisprofile
        /// @param[in]   callback   This is the callback of the request which will only be called on error
        /// @return Return value indicating success or error.
        comm::datalayer::DlResult getObjectData(const std::string& address, IAxisProfileInternal*& axisprofile, const ResponseCallback& callback);

        //! @brief This method returns the profile manager of the axisprofile component.
        //! @param[in]   callback   This is the callback of the request which will only be called on error
        //! @return Pointer to the profile manager (success) or nullptr (error).
        IAxisProfileManagerInternal* getProfileManager(const ResponseCallback& callback);

        //! @brief This method manages onMetadata() request for profile nodes with wildcard address.
        //! @param[in]   address       Address of the node
        //! @param[in]   callback      This is the callback of the request which will only be called on error
        //! @param[in]   metaData      Action, that may be performed on the node
        //! @return Return value indicating success or error.
        AxisProfileResult onMetadataBase(const std::string &address, const ResponseCallback &callback, const MetaDataInfo& metaData);

        //! @brief This method implement onMetadata() request for profile nodes with basic data type.
        //! @param[in]   address       Address of the node
        //! @param[in]   callback      This is the callback of the request which will only be called on error
        //! @param[in]   variantType   Variant data type
        //! @return Return value indicating success or error.
        AxisProfileResult onMetadataBasicType(const std::string& address, const ResponseCallback& callback, comm::datalayer::VariantType variantType);
        
      private : // data

        friend class ::TestProviderNodeProfilesBase;   //!< Unittest of this class is a friend
      };
    }
  }
}
