/// @file
///
/// @brief This file is part of the axisprofile library and implements the node of axisprofile EtherCAT name
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "node_profiles_ethercatmaster.h"
#include "profiles/i_axisprofile_internal.h"
#include "fieldbuses/fieldbus_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::provider;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
NodeProfilesEtherCATMasterName::NodeProfilesEtherCATMasterName(ProviderMgr* mgr):NodeProfilesBase(mgr)
{
  m_metaDataInfo.m_description = "The name of the EtherCAT master instance configured in this axisprofile object.";
  m_metaDataInfo.m_descriptionUrl = "";
  m_metaDataInfo.m_nodeClass = comm::datalayer::NodeClass::NodeClass_Variable;
  m_metaDataInfo.nodeActions = node_actions::READ | node_actions::WRITE;
}

void NodeProfilesEtherCATMasterName::onWrite(const std::string& address, const comm::datalayer::Variant* data, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // allow empty name
  const char* newEtherCATName = *data;
  if (strlen(newEtherCATName) == 0)
  {
    profile->getPropertyInterface()->setEtherCATName(std::string().c_str());
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
    return;
  }

  // Check and extract the received variant
  if (!ProviderHelper::checkVariantType(data, comm::datalayer::VariantType::STRING, callback))
  {
    return;
  }

  IAxisProfileManagerInternal* profileManager = getProfileManager(callback);
  if (!profileManager)
  {
    return;
  }

  // Write variant data
  // log original name
  std::string oldEtherCATName = profile->getPropertyInterface()->getEtherCATName();
  // set new name
  AxisProfileResult aprReturn = profile->getPropertyInterface()->setEtherCATName(newEtherCATName);

  // Finish
  if (aprStatusSucceeded(aprReturn))
  {
    // synchronize with input/output buffer string
    std::string bufferString {};
    bufferString = FieldbusHelper::generateRTBufferString(newEtherCATName, BufferOrient::INPUT_BUFFER);
    profileManager->setProfileInputBuffer(profile->getProfileName(), bufferString.c_str());
    bufferString = FieldbusHelper::generateRTBufferString(newEtherCATName, BufferOrient::OUTPUT_BUFFER);
    profileManager->setProfileOutputBuffer(profile->getProfileName(), bufferString.c_str());
    // after success set then update the address according to device name
    callback(comm::datalayer::DlResult::DL_OK, nullptr);
  }
  else
  {
    //when error or discard, need clean the name to original one.
    profile->getPropertyInterface()->setEtherCATName(oldEtherCATName.c_str());
    ProviderHelper::sendDlErrorResponse(callback, comm::datalayer::DlResult::DL_INVALID_VALUE,
                                        MAIN_DIAG_DATALAYER_ERR, DETAILED_DIAG_INVALID_DEVICENAME, "Set EtherCAT name failed!");
  }
}

void NodeProfilesEtherCATMasterName::onRead(const std::string& address, const comm::datalayer::Variant* /*data*/, const ResponseCallback& callback)
{
  // Get profile pointer
  IAxisProfileInternal* profile = getProfileByAddress(address, callback);
  if (!profile)
  {
    return;
  }

  // Get configured output URI
  std::string etherCATName = profile->getPropertyInterface()->getEtherCATName();

  // Finish
  comm::datalayer::Variant returnVal;
  etherCATName.empty() ? returnVal.setValue("") : returnVal.setValue(etherCATName);
  callback(comm::datalayer::DlResult::DL_OK, &returnVal);
}

void NodeProfilesEtherCATMasterName::onMetadata(const std::string& address, const ResponseCallback& callback)
{
  onMetadataBase(address, callback, this->m_metaDataInfo);
}
