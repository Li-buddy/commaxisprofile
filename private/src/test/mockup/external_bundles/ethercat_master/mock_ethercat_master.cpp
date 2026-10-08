/// @file
///
/// This source file is part of the axisprofile library and implement mock EtherCAT master.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mock_ethercat_master.h"
#include <thread>
#include "helper/datalayer_memory_map_helper.h"
#include "axisprofile_internal_defs.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;

MockEthercatMaster::MockEthercatMaster(const xTST_Test* test, comm::datalayer::IDataLayerFactory* datalayerFactory): m_testClass(test), m_datalayerFactory(datalayerFactory)
{
  // Initialize class members
  m_statusword.m_word = STSWORD_INOPERATION;
}

MockEthercatMaster::~MockEthercatMaster()
{
  deInit();
  m_testClass = nullptr;
}

std::string MockEthercatMaster::getBufferName(const std::string &path)
{
  return path.substr(strlen(MOCK_ECAT_BASE_ADDRESS) + 1);
}

HRESULT MockEthercatMaster::init()
{
  m_provider = m_datalayerFactory->createProvider();
  m_provider->start();

  // Register nodes
  comm::datalayer::DlResult resultDatalayer = m_provider->registerNode(MOCK_ECAT_BASE_ADDRESS, &m_ecatInstancesNode);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_BASE_ADDRESS), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_FIELDBUS_MASTER_STATE, &m_ecatFieldbusStateNode);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_FIELDBUS_MASTER_STATE), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_FIELDBUS_MASTER_STATUS, &m_ecatFieldbusStatusNode);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_FIELDBUS_MASTER_STATUS), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SOE_PARAM_REQUEST, &m_ecatParamRequestNodeSoE);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SOE_PARAM_REQUEST), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_COE_SDO_REQUEST, &m_ecatSDORequestNodeCoE);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_COE_SDO_REQUEST), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }  

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SLAVE_STATE, &m_ecatSlaveStateNodeSoE);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_STATE), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SLAVE_STATUS, &m_ecatSlaveStatusNodeSoE);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_STATUS), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SLAVE_DC_CFG_INFO, &m_ecatSlaveDcCfgInfo);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_DC_CFG_INFO), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SLAVE_ADDRESS_MAPP, &m_ecatSlaveAddressMapping);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_ADDRESS_MAPP), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_provider->registerNode(MOCK_ECAT_SLAVE_CFG_INFO, &m_ecatSlaveCfgInfo);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nregisterNode() failed for %ls! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_CFG_INFO), UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  // Create realtime memory
  resultDatalayer = m_datalayerFactory->createMemorySync(m_ownerInputBuffer, MOCK_ECAT_INPUTBUFFER_STRING, m_provider, ARRAY_SIZE_BUFFER, comm::datalayer::MemoryType::MemoryType_Input);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nCreateMemorySync() failed! Result: %ls", UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  resultDatalayer = m_datalayerFactory->createMemorySync(m_ownerOutputBuffer, MOCK_ECAT_OUTPUTBUFFER_STRING, m_provider, ARRAY_SIZE_BUFFER, comm::datalayer::MemoryType::MemoryType_Output);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nCreateMemorySync() failed! Result: %ls", UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }

  MemoryMapHelper memMapHelper;

  // Note: If you change the start offset (SLAVE_DATA_AT_START_OFFSET_IN_BIT or SLAVE_DATA_MDT_START_OFFSET_IN_BIT) then you may also have to change
  // the corresponding offset defines used in copyMdtToAt() (SLAVE_DATA_AT_START_OFFSET_IN_BYTE or SLAVE_DATA_MDT_START_OFFSET_IN_BYTE).

  // Create Input MemMap
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_STATUSWORD, SLAVE_DATA_AT_START_OFFSET_IN_BIT, 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_POSITION, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 16), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_DIAGMSG, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 48), 32, comm::datalayer::TYPE_PLC_DWORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_VELOCITY, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 80), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SWITCH_REF, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 112), 1, comm::datalayer::TYPE_PLC_BOOL);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SWITCH_POS_LIM, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 113), 1, comm::datalayer::TYPE_PLC_BOOL);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SWITCH_NEG_LIM, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 114), 1, comm::datalayer::TYPE_PLC_BOOL);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_POS_EDG, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 115), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_NEG_EDG, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 147), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_ACTIVE,  (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 179), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_POS_LAT, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 195), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_NEG_LAT, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 211), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_POS_DIF, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 227), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE1_MAK_FAL, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 259), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_POS_EDG, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 275), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_NEG_EDG, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 307), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_ACTIVE,  (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 339), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_POS_LAT, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 355), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_NEG_LAT, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 371), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_POS_DIF, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 387), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_PROBE2_MAK_FAL, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 419), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_TORQUE_FORCE,   (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 435), 16, comm::datalayer::TYPE_PLC_INT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_TORQUE_FORCE_LIMIT_STATUS, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 451), 8, comm::datalayer::TYPE_PLC_BYTE);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_OP_MODE_DISPLAY, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 459), 16, comm::datalayer::TYPE_PLC_INT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_1, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 475), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_2, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 491), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_3, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 507), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_4, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 523), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_5, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 539), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_INPUT_6, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 555), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SMO_SAFETY_SELECTION, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 571), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SMO_SAFETY_STATUS,    (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 603), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_SIGNAL_STATUS_WORD,   (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 635), 16, comm::datalayer::TYPE_PLC_WORD);

  // Create master rt data MemMap
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_TASK_START_TIME, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 635), 64, comm::datalayer::TYPE_PLC_ULINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_TASK_CYCLE_TIME, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 699), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_OUTPUT_SHIFT_TIME, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 731), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_SEND_OFFSET, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 763), 32, comm::datalayer::TYPE_PLC_UDINT);
  
  auto& memMapIn = memMapHelper.createMemoryMap(0);
  resultDatalayer = m_ownerInputBuffer->setMemoryMap(memMapIn);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nsetMemoryMap() failed! Result: %ls", UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }
  memMapHelper.clearMemMapData();

  // Create Output MemMap
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_CMDWORD, SLAVE_DATA_MDT_START_OFFSET_IN_BIT, 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_POSITION, (SLAVE_DATA_MDT_START_OFFSET_IN_BIT + 16), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_VELOCITY, (SLAVE_DATA_MDT_START_OFFSET_IN_BIT + 48), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_TORQUE_FORCE, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 80), 16, comm::datalayer::TYPE_PLC_INT);
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_HIGH, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 96), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_OUTBUFFER_TORQUE_FORCE_LIMIT_LOW, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 128), 32, comm::datalayer::TYPE_PLC_UDINT);

  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_OP_MODE, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 160), 32, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_1, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 192), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_2, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 208), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_3, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 224), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_4, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 240), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_5, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 256), 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable(MOCK_ECAT_INBUFFER_USER_PDO_OUTPUT_6, (SLAVE_DATA_AT_START_OFFSET_IN_BIT + 272), 16, comm::datalayer::TYPE_PLC_WORD);

  auto& memMapOut = memMapHelper.createMemoryMap(0);
  resultDatalayer = m_ownerOutputBuffer->setMemoryMap(memMapOut);
  if (resultDatalayer != comm::datalayer::DlResult::DL_OK)
  {
    m_testClass->error(L"\nsetMemoryMap() failed! Result: %ls", UTF8TOWIDE(resultDatalayer.toString()));
    return E_FAIL;
  }
  memMapHelper.clearMemMapData();

  return S_OK;
}

HRESULT MockEthercatMaster::deInit()
{
  HRESULT result = S_OK;

  // If provider is not available, deInit() has already been called!
  if (m_provider)
  {
    comm::datalayer::DlResult dlResult = m_provider->unregisterNode(MOCK_ECAT_BASE_ADDRESS);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_BASE_ADDRESS), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    dlResult = m_provider->unregisterNode(MOCK_FIELDBUS_MASTER_STATE);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_FIELDBUS_MASTER_STATE), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    dlResult = m_provider->unregisterNode(MOCK_FIELDBUS_MASTER_STATUS);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_FIELDBUS_MASTER_STATUS), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SOE_PARAM_REQUEST);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SOE_PARAM_REQUEST), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SLAVE_STATE);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_STATE), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SLAVE_STATUS);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_STATUS), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SLAVE_DC_CFG_INFO);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_DC_CFG_INFO), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SLAVE_ADDRESS_MAPP);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_ADDRESS_MAPP), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->unregisterNode(MOCK_ECAT_SLAVE_CFG_INFO);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\nprovider->unregisterNode() failed for node %s! Result: %ls", UTF8TOWIDE(MOCK_ECAT_SLAVE_CFG_INFO), UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    dlResult = m_datalayerFactory->deleteMemorySync(m_ownerInputBuffer);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\ndlFactory->deleteMemorySync() failed for input buffer! Result: %ls", UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    dlResult = m_datalayerFactory->deleteMemorySync(m_ownerOutputBuffer);
    if (comm::datalayer::STATUS_FAILED(dlResult))
    {
      m_testClass->error(L"\ndlFactory->deleteMemorySync() failed for output buffer! Result: %ls", UTF8TOWIDE(dlResult.toString()));
      result = E_FAIL;
    }

    m_provider->stop();
    delete m_provider;
    m_provider = nullptr;
  }

  return result;
}

HRESULT MockEthercatMaster::copyMdtToAt(std::uint8_t numberOfAxes)
{
  std::uint8_t* pointerToOutputDataImage = nullptr;
  std::uint8_t* pointerToInputDataImage = nullptr;

  comm::datalayer::DlResult resultDatalayerOutput = m_ownerOutputBuffer->beginAccess(pointerToOutputDataImage, 0);
  if ( resultDatalayerOutput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Failed to begin access of output data. Result: %ls", UTF8TOWIDE(resultDatalayerOutput.toString()));
    return E_FAIL;
  }
  
  comm::datalayer::DlResult resultDatalayerInput = m_ownerInputBuffer->beginAccess(pointerToInputDataImage, 0);
  if ( resultDatalayerInput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Failed to begin access of input data. Result: %ls", UTF8TOWIDE(resultDatalayerInput.toString()));
    m_ownerOutputBuffer->endAccess();
    return E_FAIL;
  }

  memcpy(pointerToInputDataImage, pointerToOutputDataImage, ARRAY_SIZE_BUFFER);

  // Set statusword of all devices in input data image to 'in operation' to allow execution of motion commands in test
  for ( std::uint32_t loopCount = 0; loopCount < numberOfAxes; loopCount++ )
  {
    // Explanation: If the start offset of the of the configured input/output memory maps are e.g. set to 80 Bits ( see define SLAVE_DATA_AT_START_OFFSET_IN_BIT
    // and SLAVE_DATA_MDT_START_OFFSET_IN_BIT) then we have to use an offset of 10 here (SLAVE_DATA_AT_START_OFFSET_IN_BYTE and SLAVE_DATA_MDT_START_OFFSET_IN_BYTE).
    // Reason: We work with an std::uint8_t* here -> std::uint8_t[10] is an offset of 10*8 Bit.
    std::uint8_t* pointerToNextDeviceDataBlockInAt = &pointerToInputDataImage[ SLAVE_DATA_AT_START_OFFSET_IN_BYTE + ( AXISPROFILE_SLAVE_DATA_AT_SIZE*loopCount ) ];

    comm::axisprofile::AcknowledgeTelegram* atData = reinterpret_cast<comm::axisprofile::AcknowledgeTelegram*>(pointerToNextDeviceDataBlockInAt);

    // Set statusword without toggle bit
    atData->m_statusWord.m_word = m_statusword.m_word;
  }

  resultDatalayerInput = m_ownerInputBuffer->endAccess();
  resultDatalayerOutput = m_ownerOutputBuffer->endAccess();
  if ( resultDatalayerInput != comm::datalayer::DlResult::DL_OK || resultDatalayerOutput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Call of MockEthercatMaster::copyMdtToAt() failed. RT-Access not closed");
    return E_FAIL;
  }

  return S_OK;
}

HRESULT MockEthercatMaster::copyAtToMdt(std::uint8_t numberOfAxes)
{
  comm::axisprofile::MasterDataTelegram* mdtData = nullptr;
  std::uint8_t* pointerToOutputDataImage = nullptr;
  std::uint8_t* pointerToInputDataImage = nullptr;

  comm::datalayer::DlResult resultDatalayerOutput = m_ownerOutputBuffer->beginAccess(pointerToInputDataImage, 0);
  if ( resultDatalayerOutput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Failed to begin access of output data. Result: %ls", UTF8TOWIDE(resultDatalayerOutput.toString()));
    return E_FAIL;
  }
  
  comm::datalayer::DlResult resultDatalayerInput = m_ownerInputBuffer->beginAccess(pointerToOutputDataImage, 0);
  if ( resultDatalayerInput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Failed to begin access of input data. Result: %ls", UTF8TOWIDE(resultDatalayerInput.toString()));
    m_ownerOutputBuffer->endAccess();
    return E_FAIL;
  }

  memcpy(pointerToOutputDataImage, pointerToInputDataImage , ARRAY_SIZE_BUFFER);

  // Set statusword of all devices in input data image to 'in operation' to allow execution of motion commands in test
  for ( std::uint32_t loopCount = 0; loopCount < numberOfAxes; loopCount++ )
  {
    // Explanation: If the start offset of the of the configured input/output memory maps are e.g. set to 80 Bits ( see define SLAVE_DATA_AT_START_OFFSET_IN_BIT
    // and SLAVE_DATA_MDT_START_OFFSET_IN_BIT) then we have to use an offset of 10 here (SLAVE_DATA_AT_START_OFFSET_IN_BYTE and SLAVE_DATA_MDT_START_OFFSET_IN_BYTE).
    // Reason: We work with an std::uint8_t* here -> std::uint8_t[10] is an offset of 10*8 Bit.
    std::uint8_t* pointerToNextDeviceDataBlockInAt = &pointerToOutputDataImage[ SLAVE_DATA_AT_START_OFFSET_IN_BYTE + ( AXISPROFILE_SLAVE_DATA_AT_SIZE*loopCount ) ];

    mdtData = reinterpret_cast<comm::axisprofile::MasterDataTelegram*>(pointerToNextDeviceDataBlockInAt);

    m_controlword.m_word = mdtData->m_controlWord.m_word;
  }

  resultDatalayerInput = m_ownerInputBuffer->endAccess();
  resultDatalayerOutput = m_ownerOutputBuffer->endAccess();
  if ( resultDatalayerInput != comm::datalayer::DlResult::DL_OK || resultDatalayerOutput != comm::datalayer::DlResult::DL_OK )
  {
    m_testClass->error(L"Call of MockEthercatMaster::copyAtToMdt() failed. RT-Access not closed");
    return E_FAIL;
  }

  return S_OK; 
}

HRESULT MockEthercatMaster::waitUntilRtAvailable(std::uint32_t timeout)
{
  std::uint32_t timePerCycle = 200;
  std::int32_t timeoutCycles = static_cast<std::int32_t>( timeout / timePerCycle );

  comm::datalayer::DlResult resultDatalayerOutput;
  comm::datalayer::DlResult resultDatalayerInput;

  std::uint8_t* pointerToOutputDataImage = nullptr;
  std::uint8_t* pointerToInputDataImage = nullptr;
  do
  {
    resultDatalayerOutput = m_ownerOutputBuffer->beginAccess(pointerToOutputDataImage, 0);
    if ( resultDatalayerOutput != comm::datalayer::DlResult::DL_OK )
    {
      m_ownerOutputBuffer->endAccess();
    }

    resultDatalayerInput = m_ownerInputBuffer->beginAccess(pointerToInputDataImage, 0);
    if ( resultDatalayerInput != comm::datalayer::DlResult::DL_OK )
    {
      m_ownerInputBuffer->endAccess();
    }

    if ( ( ( resultDatalayerOutput == comm::datalayer::DlResult::DL_OK ) || ( resultDatalayerOutput == comm::datalayer::DlResult::DL_RT_NOVALIDDATA ) ) &&
         ( ( resultDatalayerInput  == comm::datalayer::DlResult::DL_OK ) || ( resultDatalayerInput  == comm::datalayer::DlResult::DL_RT_NOVALIDDATA ) ) )
    {
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(timePerCycle));
    timeoutCycles--;
  } while ( timeoutCycles > 0 );

  m_ownerOutputBuffer->endAccess();
  m_ownerInputBuffer->endAccess();

  if ( timeoutCycles > 0 )
  {
    return S_OK;
  }
  else
  {
    m_testClass->error(L"Timeout! Ethercat master mockup could not access RT memory.");
    return E_FAIL;
  }
}

void MockEthercatMaster::setSoEStatuswordErrorBit(bool error)
{
  // statusword.bit13: axis error
  m_statusword.m_bits.bit13 = error;
}

void MockEthercatMaster::setSoEStatuswordCmdChangeBit(bool changBit)
{
  // statusword.bit5: SERCOS command changed
  m_statusword.m_bits.bit5 = changBit;
}

void MockEthercatMaster::runCoEStatusControl()
{
  // controlword.bit7: clear error
  if (m_controlword.m_bits.bit7)
  {
    // statusword.bit3: error cleared
    m_statusword.m_bits.bit7 = false;
  }
  // controlword.bit3: enable operation
  if (m_controlword.m_bits.bit3)
  {
    // statusword.bit2: operation enabled
    m_statusword.m_bits.bit0 = true;
    m_statusword.m_bits.bit1 = true;
    m_statusword.m_bits.bit2 = true;
    m_statusword.m_bits.bit5 = true;
  }
}

void MockEthercatMaster::setStatusWord(std::uint16_t statusword)
{
  m_statusword.m_word = statusword;
}
