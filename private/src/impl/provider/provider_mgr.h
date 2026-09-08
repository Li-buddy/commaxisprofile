#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and implements the provider manager for datalayer nodes.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer.h"
#include "component/i_axisprofile_component.h"
#include "provider/nodes/i_node_base.h"
#include "provider/nodes/profiles/node_profiles_creator.h"
#include "provider/nodes/system/node_system_creator.h"
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::provider
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

  /// @brief This is the provider manager class.
  class ProviderMgr
  {
  public: // methods

    /// Constructor
    ProviderMgr();
    /// Destructor
    virtual ~ProviderMgr() noexcept;

    /// @brief This method returns the class name.
    ///
    /// @return                      Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(ProviderMgr)); }

    /// @brief This method initializes the providerMgr.
    /// @param[in]     cmpAxisProfile    Pointer to the axisprofile component.
    virtual void init(comm::axisprofile::IAxisProfileComponent *cmpAxisProfile);

    /// @brief This method adds a new node to the providers.
    /// @param[in]     address       Address of this new node
    /// @param[in]     newNode       Pointer to the new node.
    virtual void addNode(const std::string &address, INodeBase* newNode);

    /// @brief Get the axisprofile component
    /// @return     the axisprofile component
    virtual comm::axisprofile::IAxisProfileComponent* getAxisProfileCmp() const { return m_cmpAxisProfile; }

    /// @brief Run all provider nodes (in fact, register them to the data layer)
    /// @return  true, if successful
    virtual bool run();

    /// @brief Stop all provider nodes (in fact, unregister them from the data layer)
    /// @return  true, if successful
    virtual bool stop();

  private: // methods

    /// @brief register a flatbuffer type to a node in the datalayer
    /// @param[in]   bfbsPath             relative path to the bfbs file
    /// @param[in]   nodeAddress          address to the node
    /// @return false on error
    virtual bool registerTypes(const std::string &bfbsPath, const std::string &nodeAddress);

    /// @brief Register the motion scopes for this provider.
    virtual void registerScopes();
    /// @brief Unregister the motion scopes for this provider.
    virtual void unregisterScopes();

  private: // data

    provider::NodeSystemCreator m_nodeSystemCreator;       ///< Creator class for all system nodes
    provider::NodeProfilesCreator m_nodeProfilesCreator;   ///< Creator class for all profile nodes

    /// @brief struct for every known node
    struct NodeEntry
    {
      std::string m_address;   ///< address of this node
      INodeBase* m_node;       ///< the node itself
      /// @brief constructor
      /// @param[in]   address       node address in datalayer tree
      /// @param[in]   node          pointer to the node
      NodeEntry(const std::string &address, INodeBase* const node)
      : m_address(address), m_node(node)
      {
      };                
    };
        
    std::vector<NodeEntry> m_allNodes;                               ///< vector of known nodes
    comm::axisprofile::IAxisProfileComponent* m_cmpAxisProfile{ };   ///< pointer to the axisprofile component
    comm::datalayer::IProvider* m_dlProvider{ };                     ///< pointer to the data layer provider

    /// @brief wait until all registered nodes are online
    /// @param[in]   dlFactory    pointer to the datalayer factory
    /// @return true on success
    bool waitForAllNodes( comm::datalayer::IDataLayerFactory *dlFactory );

    /// @brief check, if all registered nodes are online
    /// @param[in]   client               datalayer client
    /// @param[out]  everyThingOnline     result: are all nodes online?
    /// @return false on error
    bool checkForAllNodes( comm::datalayer::IClient *client, bool &everyThingOnline );
  };
}
