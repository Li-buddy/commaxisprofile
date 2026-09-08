#include "test_provider_node_profiles_scalingFactor.h"
#include "provider/nodes/profiles/cfg/node_profiles_scalingFactor.h"

#include "helper/result_helper.h"
#include "helper/provider_helper_common.h"
#include "helper/provider_helper_unittest.h"

#include "mockup/mock_axisprofile_cmp.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestProviderNodeProfilesScalingFactor);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingFactor, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingFactor, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeProfilesScalingFactor, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderNodeProfilesScalingFactor::doInit()
{
  HRESULT res = S_OK;

  m_invalidNodeAddress = m_nodeAddress;
  test::ProviderHelperCommon::insertProfileName(m_nodeAddress, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_invalidNodeAddress, "doesNotExist");

  return res;
}

HRESULT TestProviderNodeProfilesScalingFactor::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestProviderNodeProfilesScalingFactor");
  log(L"\n----------------------------------------------------------------\n");

  res |= doTestCase();

  return res;
}


HRESULT TestProviderNodeProfilesScalingFactor::doEnd()
{
  HRESULT res = S_OK;

  log(L"\nDeinit test cases...");
  LOG_TEST_RESULT(res, L"\nGet an unexpected result!", S_OK);

  return res;
}

HRESULT TestProviderNodeProfilesScalingFactor::doTestCase()
{
  HRESULT res = S_OK;
  res |= testOnMetadata();
  res |= testOnRead();
  res |= testOnWrite();

  return res;
}


HRESULT TestProviderNodeProfilesScalingFactor::testOnRead()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node without manager.");
  provider::NodeProfilesScalingFactorCfg nodeNoManager(nullptr);

  log(L"\nCall onRead() when provider manager does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &nodeNoManager, m_nodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesScalingFactorCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onRead() when requested profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  IAxisProfile* profile = mockAxisProfileCmp.getAxisProfileManager()->getProfileByName(m_profileName.c_str());
  if (!profile)
  {
    error(L"\nCould not find previously created profile!");
    return E_FAIL;
  }

  log(L"\nCall onRead() when everything is OK.");
  comm::datalayer::Variant expData {};
  comm::axisprofile::ScalingFactor scalingFactor = profile->getProfileScalingStrategyCfg().m_velFactor;

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  auto factorOffset = node.generateFlatbufferData(builder, scalingFactor);

  builder.Finish(factorOffset);
  expData.copyFlatbuffers(builder);

  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onRead(this, &node, m_nodeAddress, &readData, true,
                                                      comm::datalayer::DlResult::DL_OK, &expData, fbtypes::VerifyScalingFactorBuffer));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesScalingFactor::testOnMetadata()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onRead() -----</b>");

  log(L"\nCreate node with manager.");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesScalingFactorCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCreate profile with name '%ls' for further testing.", UTF8TOWIDE(m_profileName));
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);

  comm::datalayer::Variant readData {};
  PROVIDER_CHECK(test::ProviderHelperUnittest::onMetadata(this, &node, m_nodeAddress, true, comm::datalayer::DlResult::DL_OK));

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeProfilesScalingFactor::testOnWrite()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>----- Test method onWrite() -----</b>");
  test::MockAxisProfileCmp mockAxisProfileCmp {};
  provider::NodeProfilesScalingFactorCfg node(mockAxisProfileCmp.getProviderManager());

  log(L"\nCall onWrite() when profile does not exist.");
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_invalidNodeAddress, nullptr, false, comm::datalayer::DlResult::DL_INVALID_HANDLE));

  log(L"\nCreate profile for further testing.");
  mockAxisProfileCmp.getAxisProfileManager()->createAxisProfile(AxisProfileType::SERVODRIVE_OVER_ETHERCAT, m_profileName);
  log(L"\nCall onWrite() with invalid variant type.");
  comm::datalayer::Variant data {};
  data.setType(comm::datalayer::VariantType::UNKNOWN);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite with with flatbuffer that has a missing field (but scaling factor not is mandatory).");  
  data = createFBData(true);
  PROVIDER_CHECK_AND_RET(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data, false, comm::datalayer::DlResult::DL_TYPE_MISMATCH));

  log(L"\nCall onWrite() when everything is OK.But velocity scaling factor is deactivated.");
  ScalingFactor factor {};
  data = createFBData(false, factor);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\nCall onWrite() when everything is OK.But velocity scaling factor is deactivated.");
  factor = {true,1,1};
  data = createFBData(false, factor);
  PROVIDER_CHECK(test::ProviderHelperUnittest::onWrite(this, &node, m_nodeAddress, &data));

  log(L"\n");
  return hResult;
}

comm::datalayer::Variant TestProviderNodeProfilesScalingFactor::createFBData(bool bFieldEmpty, const ScalingFactor& factor)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  // Finish
  fbtypes::ScalingFactorBuilder sFbuilder(builder);
  if (!bFieldEmpty)
  {
    sFbuilder.add_denominator(factor.m_denominator);
    sFbuilder.add_numerator(factor.m_numerator);
    sFbuilder.add_isEnabled(factor.m_enabled);
  }

  builder.Finish(sFbuilder.Finish());
  comm::datalayer::Variant variantData;
  variantData.copyFlatbuffers(builder);
  return variantData;
}
