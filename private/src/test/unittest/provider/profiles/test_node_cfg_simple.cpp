/// @file  test_node_cfg_simple.cpp
///
/// This source file is part of the axisprofile library and implement
/// the node for simple data type.
///
/// @copyright (C) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "test_node_cfg_simple.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/templates/node_cfg_simple.h"
// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

REGISTER_TEST(TestNodeCfgSimple);
REGISTER_TEST_TO_GROUP(TestNodeCfgSimple, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestNodeCfgSimple, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestNodeCfgSimple, Provider);

HRESULT TestNodeCfgSimple::doInit()
{
  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");
  return S_OK;
}

HRESULT TestNodeCfgSimple::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the axisprofile datalayer node, used to set/get the simple type profile node data.");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testOnRead();
  hResult |= testOnWrite();
  hResult |= testOnMetadata();

  return hResult;
}

HRESULT TestNodeCfgSimple::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");
  provider::MetaDataInfo metaData {"Used for situations where multiple axes correspond to a single address. If there is only one address corresponding to one axis, the default value is 0. Otherwise, add 1 in sequence", ""};

  log(L"\nCreate node without manager.");
  provider::NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult> nodeNoManager(nullptr, &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber, metaData);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult> node(mockAxisProfileCmp.getProviderManager(), &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber, metaData);

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  std::uint8_t driveNumber = 10;
  log(L"\nSet drive number to %d.", driveNumber);
  IAxisProfileInternal* profile = static_cast<IAxisProfileInternal*>(mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }
  profile->setDriveNumber(driveNumber);

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData{};
  expData.setValue(driveNumber);
  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true, comm::datalayer::DlResult::DL_OK, &expData));

  log(L"\n");
  return hResult;
}

HRESULT TestNodeCfgSimple::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");
  provider::MetaDataInfo metaData {"Used for situations where multiple axes correspond to a single address. If there is only one address corresponding to one axis, the default value is 0. Otherwise, add 1 in sequence", ""};

  log(L"\nCreate node without manager.");
  provider::NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult> nodeNoManager(nullptr, &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber, metaData);
  comm::datalayer::Variant data{};
  data.setType(comm::datalayer::VariantType::UINT8);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &nodeNoManager, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult> node(mockAxisProfileCmp.getProviderManager(), &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber, metaData);

  log(L"\nCall onWrite() with invalid variant type.");  
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when profile does not exist.");
  data.setType(comm::datalayer::VariantType::UINT8);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, &data, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));
  
  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::CANOPEN_OVER_ETHERCAT, m_profileName);

  log(L"\nCall onWrite() when everything is OK.");
  std::uint8_t driveNumber = 10;
  data.setValue(std::uint8_t(driveNumber));
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

HRESULT TestNodeCfgSimple::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onMetadata() -----</b>");

  log(L"\nCreate node with manager.");
  provider::MetaDataInfo metaData {"Used for situations where multiple axes correspond to a single address. If there is only one address corresponding to one axis, the default value is 0. Otherwise, add 1 in sequence", ""};
  test::MockAxisProfileCmp mockAxisProfileCmp{};
  provider::NodeCfgSimple<std::uint8_t, IAxisProfileInternal, comm::axisprofile::AxisProfileResult> node(mockAxisProfileCmp.getProviderManager(), &IAxisProfileInternal::getDriveNumber, &IAxisProfileInternal::setDriveNumber, metaData);

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData{};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}
