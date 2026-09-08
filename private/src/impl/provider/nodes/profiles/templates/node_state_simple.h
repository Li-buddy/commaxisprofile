#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines template for simple profile nodes.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the state node class for simple data types.
  template <typename RET_TYPE, typename DATA_OBJ>
  class NodeStateSimple : public NodeProfilesBase
  {
  public: // methods

    /// deduce function pointer type from template
    typedef std::function<RET_TYPE(const DATA_OBJ&)> FnPtrType;
    /// deduce function pointer type from template
    typedef std::function<RET_TYPE(DATA_OBJ&)> FnPtrTypeNonConst;

    /// stringifier method, when we have to return strings
    typedef std::function<const char* (const RET_TYPE&)> StringifierType;

    /// Constructor
    /// @param[in]   mgr             pointer to the provider manager
    /// @param[in]   pfn             function pointer for return data values
    /// @param[in]   metaDataInfo    metadata info of this node
    /// @param[in]   toString        function pointer to convert the data to string (can be nullptr)
    NodeStateSimple(ProviderMgr* mgr, const FnPtrType& pfn, const MetaDataInfo& metaDataInfo, const StringifierType& toString = nullptr): NodeProfilesBase(mgr), m_dataFnPtr(pfn), m_toString(toString)
    {
      this->m_metaDataInfo = metaDataInfo;
      this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
      this->m_metaDataInfo.nodeActions = node_actions::READ;
    }

    /// Constructor for non-const getter functions
    /// @param[in]   mgr             pointer to the provider manager
    /// @param[in]   pfn             function pointer for return data values
    /// @param[in]   metaDataInfo    metadata info of this node
    /// @param[in]   toString        function pointer to convert the data to string (can be nullptr)
    NodeStateSimple(const FnPtrTypeNonConst& pfn, ProviderMgr* mgr, const MetaDataInfo& metaDataInfo, const StringifierType& toString = nullptr): NodeProfilesBase(mgr), m_dataFnPtrNonConst(pfn), m_toString(toString)
    {
      this->m_metaDataInfo = metaDataInfo;
      this->m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
      this->m_metaDataInfo.nodeActions = node_actions::READ;
    }

    /// Destructor
    virtual ~NodeStateSimple() noexcept override = default;

    /// Read function of a node. Function will be called whenever a node should be read.
    /// @param[in]  address   Address of the node to read
    /// @param[in]  data      Data of the object
    /// @param[in]  callback  Callback to call when function is finished
    void onRead(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback) final;

    void onMetadata(const std::string& address, const ResponseCallback& callback) final;

  protected: // data
    FnPtrType m_dataFnPtr {};        ///< function pointer to getter of data interface class
    FnPtrTypeNonConst m_dataFnPtrNonConst {};        ///< function pointer to getter of data interface class for non-const access
    StringifierType m_toString {};   ///< function pointer to convert the data to string (can be nullptr)
  };


  // -------------------------------------------------------------------------------
  // -------------------------------Implementation----------------------------------
  // ------------------------------------------------------------------------------
  template <typename RET_TYPE, typename DATA_OBJ>
  void NodeStateSimple<RET_TYPE, DATA_OBJ>::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
  {
    comm::datalayer::Variant retVariant;
    comm::datalayer::DlResult dlResult;

    // get object data
    DATA_OBJ* objData;
    dlResult = NodeProfilesBase::getObjectData(address, objData, callback);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      return;
    }

    // return variant
    RET_TYPE ret = m_dataFnPtr ? m_dataFnPtr(*objData) : m_dataFnPtrNonConst(*objData);
    if (m_toString)
    {
      retVariant.setValue(m_toString(ret));
    }
    else
    {
      retVariant.setValue(ret);
    }
    callback(comm::datalayer::DlResult::DL_OK, &retVariant);
  }

  template <typename RET_TYPE, typename DATA_OBJ>
  void NodeStateSimple<RET_TYPE, DATA_OBJ>::onMetadata(const std::string& address, const ResponseCallback& callback)
  {
    comm::datalayer::VariantType variantTypeID;
    if (m_toString)
    {
      variantTypeID = comm::datalayer::getVariantTypeID<char*>();
    }
    else
    {
      // convert from RET_TYPE into VariantType
      variantTypeID = comm::datalayer::getVariantTypeID<RET_TYPE>();
      if (variantTypeID == comm::datalayer::VariantType::UNKNOWN)
      {
        // try workaround for const types for which no overloaded function getVariantTypeID exists
        variantTypeID = comm::datalayer::getVariantTypeID<typename std::remove_const<RET_TYPE>::type>();
        if (variantTypeID == comm::datalayer::VariantType::UNKNOWN)
        {
          if (std::is_pointer<RET_TYPE>::value)
          {
            // try workaround for const pointer types for which no overloaded function getVariantTypeID exists
            variantTypeID = comm::datalayer::getVariantTypeID<typename std::add_pointer<typename std::remove_const<typename std::remove_pointer<RET_TYPE>::type>::type>::type>();
          }
          else if (std::is_same<RET_TYPE, std::vector<std::uint64_t>>::value)
          {
            // specialized case: treat std::vector<std::uint64_t> like std::uint64_t*
            variantTypeID = comm::datalayer::VariantType::ARRAY_OF_FLOAT64;
          }
          else if (std::is_same<RET_TYPE, std::vector<std::string>>::value)
          {
            // specialized case: treat std::vector<std::uint64_t> like std::uint64_t*
            variantTypeID = comm::datalayer::VariantType::ARRAY_OF_STRING;
          }
        }
      }
    }

    std::string typeString;
    if (!createBasicTypeString(address, variantTypeID, typeString, callback))
    {
      return;
    }
    ProviderHelper::createMetaData(typeString, m_metaDataInfo, callback);
  }
}
