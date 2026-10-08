#include "test_provider_profiles_map_single_in_out.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/profiles/cfg/node_profiles_map_single_input.h"
#include "provider/nodes/profiles/cfg/node_profiles_map_single_output.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderProfilesMapSingleInputOutput);
REGISTER_TEST_TO_GROUP(TestProviderProfilesMapSingleInputOutput, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesMapSingleInputOutput, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderProfilesMapSingleInputOutput, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderProfilesMapSingleInputOutput::TestProviderProfilesMapSingleInputOutput()
{
}


TestProviderProfilesMapSingleInputOutput::~TestProviderProfilesMapSingleInputOutput(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderProfilesMapSingleInputOutput::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressIn, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressOut, m_profileName);
  return S_OK;
}


HRESULT TestProviderProfilesMapSingleInputOutput::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the datalayer node, used to map single in- and outputs. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testErrors();
  hResult |= testNode();

  return hResult;
}

HRESULT TestProviderProfilesMapSingleInputOutput::testErrors()
{
  log(L"\n<b>Test all error cases.</b>");

  log(L"\nCreate nodes without manager.");
  provider::NodeProfilesMapSingleInput nodeInNoMgr(nullptr);
  provider::NodeProfilesMapSingleOutput nodeOutNoMgr(nullptr);
  log(L"\nCall onRead() of NodeProfilesMapSingleInput without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeInNoMgr, m_nodeAddressIn, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onRead() of NodeProfilesMapSingleOutput without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeOutNoMgr, m_nodeAddressOut, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onWrite() of NodeProfilesMapSingleInput without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeInNoMgr, m_nodeAddressIn, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCall onWrite() of NodeProfilesMapSingleOutput without provider manager.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeOutNoMgr, m_nodeAddressOut, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate mockups.");
  test::MockAxisProfileCmp mockComponent;

  log(L"\nCreate profile for test.");
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  test::MockAxisProfile* profile = static_cast<test::MockAxisProfile*>(mockComponent.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));

  log(L"\nCreate nodes with manager.");
  provider::NodeProfilesMapSingleInput nodeIn(mockComponent.getProviderManager());
  log(L"\nCall onWrite() of NodeProfilesMapSingleInput with wrong variant type.");
  comm::datalayer::Variant data{};
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeIn, m_nodeAddressIn, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  provider::NodeProfilesMapSingleOutput nodeOut(mockComponent.getProviderManager());
  log(L"\nCall onWrite() of NodeProfilesMapSingleOutput with wrong variant type.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeOut, m_nodeAddressOut, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onBrowse().");
  std::vector<std::string> expectedDataInput = {"statusword", "position", "velocity", "torque-force", "torque-limit-status", "diagcode", "switch-ref", "switch-poslim", "switch-neglim", "probe1-posedg",
    "probe1-negedg", "probe1-active", "probe1-poslat", "probe1-neglat", "probe1-posdif", "probe1-makfal", "probe2-posedg", "probe2-negedg", "probe2-active", "probe2-poslat", "probe2-neglat", "probe2-posdif"
    , "probe2-makfal", "safety-selection", "safety-status", "user-pdo-input1", "user-pdo-input2", "user-pdo-input3", "user-pdo-input4", "user-pdo-input5", "user-pdo-input6", "signal-status-word"};
  data.setValue(expectedDataInput);

  log(L"\nCall onBrowse() of NodeProfilesMapSingleInput with right variant type.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onBrowse(this, &nodeIn, m_nodeAddressIn, true, &data));

  std::vector<std::string> expectedDataOutput = {"controlword", "position", "velocity", "torque-force", "torque-high-velocity-limit", "torque-low-velocity-limit", "user-pdo-output1", "user-pdo-output2",
    "user-pdo-output3", "user-pdo-output4", "user-pdo-output5", "user-pdo-output6", "signal-control-word"};
  data.setValue(expectedDataOutput);
  log(L"\nCall onBrowse() of NodeProfilesMapSingleOutput with right variant type.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onBrowse(this, &nodeOut, m_nodeAddressOut, true, &data));

  log(L"\nSet correct variant type.");
  data.setType(comm::datalayer::VariantType::STRING);

  log(L"\nSimulate error in mapping handler.");
  static_cast<test::MockAxisProfileMappingHandler*>(profile->getMappingHandler())->setReturnValue(AxisProfileResult::ERR_NOK);

  log(L"\nCall onWrite() of NodeProfilesMapSingleInput when setting the input configuration fails.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeIn, m_nodeAddressIn, &data, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  log(L"\nCall onWrite() of NodeProfilesMapSingleOutput when setting the input configuration fails.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeOut, m_nodeAddressOut, &data, false, comm::datalayer::DlResult::DL_INVALID_VALUE));

  return S_OK;
}

HRESULT TestProviderProfilesMapSingleInputOutput::testNode()
{
  log(L"\n\n<b>Test successful node requests.</b>");

  log(L"\nCreate mockups.");
  test::MockAxisProfileCmp mockComponent;
  mockComponent.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  MockAxisProfileMappingHandlerSpecial* mockMappingHandler = new MockAxisProfileMappingHandlerSpecial(); // Will be deleted by the axis profile
  test::MockAxisProfile* profile = static_cast<test::MockAxisProfile*>(mockComponent.getAxisProfileManager()->getProfileByName(m_profileName.c_str()));
  profile->setMappingHandler(true, mockMappingHandler);

  log(L"\nCreate nodes.");
  provider::NodeProfilesMapSingleInput nodeIn(mockComponent.getProviderManager());
  provider::NodeProfilesMapSingleOutput nodeOut(mockComponent.getProviderManager());

  comm::datalayer::Variant writeData;
  writeData.setType(comm::datalayer::VariantType::STRING);
  writeData.setValue(mockMappingHandler->getConfiguredInputURI(InputMappingValueID::STATUSWORD));

  log(L"\nCall onWrite() of NodeProfilesMapSingleInput.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeIn, m_nodeAddressIn, &writeData));

  log(L"\nCall onWrite() of NodeProfilesMapSingleOutput.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &nodeOut, m_nodeAddressOut, &writeData));

  log(L"\nCall onRead() of NodeProfilesMapSingleInput.");
  comm::datalayer::Variant readDataIn;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeIn, m_nodeAddressIn, &readDataIn, true, comm::datalayer::DlResult::DL_OK, &writeData));

  log(L"\nCall onRead() of NodeProfilesMapSingleOutput.");
  comm::datalayer::Variant readDataOut;
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeOut, m_nodeAddressOut, &readDataOut, true, comm::datalayer::DlResult::DL_OK, &writeData));

  test::MockAxisProfileCmp mockComponentNoConfig;
  mockComponentNoConfig.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  provider::NodeProfilesMapSingleInput nodeInNoConfig(mockComponent.getProviderManager());
  provider::NodeProfilesMapSingleOutput nodeOutNoConfig(mockComponent.getProviderManager());

  comm::datalayer::Variant data;
  data.setType(comm::datalayer::VariantType::STRING);
  data.setValue(mockMappingHandler->getConfiguredInputURI(InputMappingValueID::STATUSWORD));

  log(L"\nCall onRead() of NodeProfilesMapSingleInput when input configuration does not exist.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeInNoConfig, m_nodeAddressIn, &readDataIn, true, comm::datalayer::DlResult::DL_OK, &data));

  log(L"\nCall onRead() of NodeProfilesMapSingleOutput when input configuration does not exist.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onRead(this, &nodeOutNoConfig, m_nodeAddressOut, &readDataOut, true, comm::datalayer::DlResult::DL_OK, &data));

  log(L"\nCall onMetadata() of NodeProfilesMapSingleOutput when input configuration does not exist.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &nodeOutNoConfig, m_nodeAddressOut, true, comm::datalayer::DlResult::DL_OK));

  log(L"\nCall onMetadata() of NodeProfilesMapSingleOutput when input configuration does not exist.");
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onMetadata(this, &nodeInNoConfig, m_nodeAddressIn, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return S_OK;
}
