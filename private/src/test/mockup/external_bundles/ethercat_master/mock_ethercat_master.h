#pragma once

/// @file
///
/// This source file is part of the axisprofile library and declare mock EtherCAT master.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer.h"
#include "mock_ethercat_master_general_nodes.h"
#include "mock_ethercat_master_nodes.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @class MockEthercatMaster
  /// Helper class to simulate the ethercat provider in test
  class MockEthercatMaster 
  {
  public:

    /// @brief Constructor
    /// @param[in]   test               Pointer to the test class for logging
    /// @param[in]   datalayerFactory   Pointer to the datalayer factory
    MockEthercatMaster(const xTST_Test* test, comm::datalayer::IDataLayerFactory* datalayerFactory);
    /// @brief Destructor
    virtual ~MockEthercatMaster() noexcept;

    /// @brief  Get the datalayer ethercatmaster node as a string.
    /// @param[in]   path    String where the path will be written to.
    ///
    /// @return         Return value indicating success (>=0) or error (<0).
    std::string getBufferName(const std::string &path);

    /// @brief This method initializes the class.
    /// @return Return value indicating success or error.
    HRESULT init();

    /// @brief This method deinitialize the class.
    /// @return Return value indicating success or error.
    HRESULT deInit();

    /// This method is just used in case of testing without real hardware. Therefore it always sets the statusword of the device in AT telegram to
    /// 'in operation'.
    /// @param[in]    numberOfAxes    Number of axes used in the test, which is needed to iterate over the output MDT telegram.
    ///
    /// @return         Return value indicating success (>=0) or error (<0).
    HRESULT copyMdtToAt(std::uint8_t numberOfAxes);

    /// This method is just used in case of testing without real hardware. 
    /// @param[in]    numberOfAxes    Number of axes used in the test, which is needed to iterate over the input AT telegram.
    ///
    /// @return         Return value indicating success (>=0) or error (<0).
    HRESULT copyAtToMdt(std::uint8_t numberOfAxes);    

    /// @brief  This method will wait until the datalayer realtime memory is available.
    /// @param[in]   timeout    Timeout value
    ///
    /// @return         Return value indicating success (>=0) or error (<0).
    HRESULT waitUntilRtAvailable(std::uint32_t timeout);

    /// @brief This method is used to simulate device errors
    /// @param[in]  error    true to set error bit, false to reset error bit
    void setSoEStatuswordErrorBit(bool error);

    /// @brief This method is used to simulate device SERCOS command change bit by toggling the appropriate bit in the statusword.
    void setSoEStatuswordCmdChangeBit(bool toggle);

    /// @brief This method is used to simulate status and control words of CiA402
    void runCoEStatusControl();

    /// @brief This method is used to set the status word of the device.
    /// @paramin] statusword  new status word to be set
    void setStatusWord(std::uint16_t statusword);

  public: // data

    const xTST_Test* m_testClass{};                                        ///< Pointer to the test class for logging
    comm::axisprofile::WordStruct m_statusword {};                         ///< Current device statusword
    comm::axisprofile::WordStruct m_controlword {};                        ///< Current device controlword
    comm::datalayer::IProvider* m_provider{};                              ///< Pointer to the datalayer provider
    comm::datalayer::IDataLayerFactory* m_datalayerFactory{};              ///< Pointer to the datalayer factory
    std::shared_ptr<comm::datalayer::IMemoryOwner> m_ownerInputBuffer;     ///< Access to the datalayer ethercat realtime input memory
    std::shared_ptr<comm::datalayer::IMemoryOwner> m_ownerOutputBuffer;    ///< Access to the datalayer ethercat realtime output memory

    // Nodes
    MockEthercatMasterInstancesNode m_ecatInstancesNode{};               ///< Datalayer node to browse the master instances
    MockEthercatMasterStateNode m_ecatFieldbusStateNode{};               ///< Datalayer node to read the fieldbus master state
    MockEthercatMasterStatusNode m_ecatFieldbusStatusNode {};            ///< Datalayer node to read the fieldbus master status
    MockEthercatMasterParamRequestNodeSoE m_ecatParamRequestNodeSoE{};   ///< Datalayer node for SoE parameter requests
    MockEthercatMasterSDORequestNodeCoE m_ecatSDORequestNodeCoE{};       ///< Datalayer node for CoE sdo requests
    MockEthercatMasterSlaveStateNodeSoE m_ecatSlaveStateNodeSoE{};       ///< Datalayer node to read the slave state via SoE
    MockEthercatMasterSlaveStatusNodeSoE m_ecatSlaveStatusNodeSoE {};    ///< Datalayer node to read the slave status via SoE
    MockEthercatMasterSlaveDcConfigInfoNode m_ecatSlaveDcCfgInfo {};     ///< Datalayer node to read the slave DC config info
    MockEthercatMasterSlaveAddressMapping m_ecatSlaveAddressMapping {};  ///< Datalayer node to read the slave mapping info
    MockEthercatMasterCfgSlaveInfo m_ecatSlaveCfgInfo {};                ///< Datalayer node to read the slave config info
  };
}
