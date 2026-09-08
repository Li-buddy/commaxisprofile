#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "profiles/profile_defs.h"
#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "test_profile_property.h"
#include <mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h>
#include <mockup/mock_fieldbus_adapter_coe.h>
#include "profiles/canopen/canopen_profile.h"
#include "test_axisprofile.h"
#include <profiles/data_sharing/sharing_handler.h>
#include <mockup/mock_fieldbus_adapter_soe.h>

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestProfileProperty);
REGISTER_TEST_TO_GROUP(TestProfileProperty, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileProperty, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileProperty, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProfileProperty::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the base class for all profile properties.");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testCInterface();
  hResult |= testClass();

  return hResult;
}


HRESULT TestProfileProperty::testCInterface()
{
  HRESULT hResult = S_OK;

  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  test::MockCategory mockCategoty;
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "dummy", 0, 0, &adapter, trace};
  m_testProperty.reset(new AxisProfileProperty(profileInfo));
  auto sharingHandler = AxisProfileSharingHandler(&profileInfo.m_profileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, &mockCategoty);
  m_testProperty->setSharingHandler(&sharingHandler);

  log(L"\n<b>Test of the property C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfilePropertyC* propertyItf = m_testProperty->getCInterface();
  LOG_TEST_RESULT_SET_PTR(propertyItf, L"\nGet an unexpected result!");

  log(L"\nCall AXISPROFILE_setDeviceAddress() with nullptr handle...");
  std::uint16_t deviceAddress = 0x1234;
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult aprResult = propertyItf->setDeviceAddress(nullptr, deviceAddress);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", expResult);

  log(L"\nCall AXISPROFILE_setDeviceAddress()...");
  aprResult = propertyItf->setDeviceAddress(propertyItf->handle, deviceAddress);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCall AXISPROFILE_getDeviceAddress() with nullptr handle...");
  propertyItf->getDeviceAddress(nullptr);

  log(L"\nCall AXISPROFILE_getDeviceAddress()...");
  propertyItf->getDeviceAddress(propertyItf->handle);

  log(L"\nCall AXISPROFILE_getProfileType() with nullptr handle...");
  propertyItf->getProfileType(nullptr);

  log(L"\nCall AXISPROFILE_getProfileType()...");
  propertyItf->getProfileType(propertyItf->handle);

  log(L"\nCall AXISPROFILE_getDeviceState() with nullptr handle...");
  propertyItf->getDeviceState(nullptr);

  log(L"\nCall AXISPROFILE_getDeviceState()...");
  propertyItf->getDeviceState(propertyItf->handle);

  log(L"\nCall AXISPROFILE_getDeviceScalingType() with nullptr handle...");
  propertyItf->getDeviceScalingType(nullptr, AxisProfileDeviceValueType::POSITION);

  log(L"\nCall AXISPROFILE_getDeviceScalingType()...");
  propertyItf->getDeviceScalingType(propertyItf->handle, AxisProfileDeviceValueType::POSITION);

  log(L"\nCall AXISPROFILE_getDeviceState() with nullptr handle...");
  propertyItf->getProfileCategory(nullptr);

  log(L"\nCall AXISPROFILE_getDeviceState()...");
  propertyItf->getProfileCategory(propertyItf->handle);

  log(L"\nCall AXISPROFILE_getCfgedEtherCATName()...");
  propertyItf->getCfgedFieldbusMasterName(propertyItf->handle);

  log(L"\n");
  return hResult;
}

HRESULT TestProfileProperty::testClass()
{
  HRESULT result = S_OK;
  canopen::CanopenProfile property {m_profileName, comm::axisprofile::NetworkTechnology::ETHERCAT};

  test::MockAxisProfileTrace mockProfileTrace {};
  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  test::MockEthercatMasterSDORequestNodeCoE sdoRequestNodeMockCoE {};
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));
  mockClient.m_testNode = &sdoRequestNodeMockCoE;
  mockFieldbusAdapter.m_returnValue = AxisProfileResult::STS_OK;

  AxisProfileType receivedType = property.getPropertyInterface()->getProfileType();
  RETURN_TEST_RESULT(receivedType, L"\nUnexpected return value.", AxisProfileType::CANOPEN_OVER_ETHERCAT);

  std::uint32_t deviceAddress = 235;
  log(L"\nCall setDeviceAddress() with valid address %d.", deviceAddress);
  AxisProfileResult aprResult = property.getPropertyInterface()->setDeviceAddress(deviceAddress);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCall getDeviceAddress() and check the result.");
  std::uint32_t readAddress = property.getPropertyInterface()->getDeviceAddress();
  LOG_TEST_RESULT_S(readAddress, L"\nIncorrect device address", deviceAddress);

  log(L"\nCall getDeviceAddress() and check the result.");
  std::string strDeviceName {"testDevice"};
  aprResult = property.getPropertyInterface()->setDeviceName(strDeviceName.c_str());
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  log(L"\nCall getDeviceName() and check the result.");
  std::string strReadDeviceName = property.getPropertyInterface()->getDeviceName();
  LOG_TEST_RESULT_STRING(strReadDeviceName, L"\nIncorrect device address", strDeviceName);

  log(L"\nCall setDeviceIdentification() and check the result.");
  std::uint32_t vendorId = 0x00000024;
  std::uint32_t productCode = 0x00242804;
  aprResult = property.getPropertyInterface()->setDeviceIdentification(vendorId, productCode);  
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  RETURN_TEST_RESULT(property.getPropertyInterface()->getDeviceSeries(), L"\nUnexpected return value.", DeviceSeries::INDRADRIVE);

  log(L"\nCall setDeviceManufacturerVersion() and check the version result.");
  aprResult = property.getPropertyInterface()->setDeviceManufacturerVersion("FWA-INDRV*-MPB-20V32-D5-1-NNN-NN");
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);

  std::uint8_t majorVersion = 0;
  std::uint8_t minorVersion = 0;
  aprResult = property.getPropertyInterface()->getDeviceSoftwareVersion(majorVersion, minorVersion);
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_ANY(majorVersion == 20, L"\nUnexpected return value.");
  LOG_TEST_RESULT_ANY(minorVersion == 32, L"\nUnexpected return value.");
  LOG_TEST_RESULT_ANY(property.getPropertyInterface()->getDeviceSoftwareVersion() == "2032", L"\nUnexpected return value.");

  log(L"\nCall setDeviceManufacturerVersion() with error case, and check the version result.");
  aprResult = property.getPropertyInterface()->setDeviceManufacturerVersion("FWA-INDRV");
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::ERR_NOK);
  aprResult = property.getPropertyInterface()->setDeviceManufacturerVersion("FWA-INDRV*-MPB-ABCDEFGHI-20V32-D5-1-NNN-NN");
  RETURN_TEST_RESULT(aprResult, L"\nUnexpected return value.", AxisProfileResult::ERR_NOK);

  log(L"\nCall enableChecksum() with normal case, and check the enable value.");
  property.getPropertyInterface()->enableChecksum(false);
  RETURN_TEST_RESULT(property.getPropertyInterface()->isChecksumEnabled(), L"\nUnexpected return value.", false);
  
  log(L"\n");
  return result;
}
