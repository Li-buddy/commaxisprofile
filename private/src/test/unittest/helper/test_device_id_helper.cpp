#include "test_device_id_helper.h"
#include "helper/device_id_helper.h"
#include "helper/result_helper.h"

// mockups

using namespace comm::axisprofile;

REGISTER_TEST(TestDeviceIdHelper);
REGISTER_TEST_TO_GROUP(TestDeviceIdHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestDeviceIdHelper, 2_UnitTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestDeviceIdHelper::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the device judge helper class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= testIsRexrothDrive();
  hResult |= testGetDeviceSeries();
  hResult |= testDeviceSupportCheckSumParam();
  hResult |= testExtractFirmwareByStr();
  hResult |= testConvertSoftwareVersion();
  return hResult;
}

HRESULT TestDeviceIdHelper::testIsRexrothDrive()
{
  log(L"\n<b>Test to isRexrothDrive().");
  LOG_TEST_RESULT_ANY(DeviceIdHelper::isRexrothDrive(DeviceSeries::INDRADRIVE), L"\nUnexpected return value.");
  LOG_TEST_RESULT_ANY(!DeviceIdHelper::isRexrothDrive(DeviceSeries::NOTINDICATES), L"\nUnexpected return value.");
  LOG_TEST_RESULT_ANY(DeviceIdHelper::isRexrothCtrlXDrive(DeviceSeries::CTRLXDRIVE_XC20), L"\nUnexpected return value.");
  LOG_TEST_RESULT_ANY(!DeviceIdHelper::isRexrothCtrlXDrive(DeviceSeries::INDRADRIVE), L"\nUnexpected return value.");
  log(L"\n");
  return S_OK;
}

HRESULT TestDeviceIdHelper::testGetDeviceSeries()
{
  std::wstring str = L"\nUnexpected return value.";
  log(L"\n<b>Test to getDeviceSeries()."); 
  uint32_t productCode = makeProductCode(0x0024); // INDRA_DRIVE = 0x0024 (highWord)
  auto series = DeviceIdHelper::calculateDeviceSeries(0x00000024 /*rexroth*/, productCode);
  LOG_TEST_RESULT_ANY(series == DeviceSeries::INDRADRIVE, str);

  productCode = makeProductCode(0x2000); // CTRLX_IP20_MODULAR = 0x2000
  series = DeviceIdHelper::calculateDeviceSeries(0x00000024 /*rexroth*/, productCode);
  LOG_TEST_RESULT_ANY(series == DeviceSeries::CTRLXDRIVE, str);

  productCode = makeProductCode(0x2200);// CTRLX_XC20 = 0x2200
  series = DeviceIdHelper::calculateDeviceSeries(0x00000024 /*rexroth*/, productCode);
  LOG_TEST_RESULT_ANY(series == DeviceSeries::CTRLXDRIVE_XC20, str);
  
  // same product code but non-rexroth vendor
  series = DeviceIdHelper::calculateDeviceSeries(0x00000002 /*non-rexroth*/, productCode);
  LOG_TEST_RESULT_ANY(series == DeviceSeries::NOTINDICATES, str);

  log(L"\n");
  return S_OK;
}

HRESULT TestDeviceIdHelper::testDeviceSupportCheckSumParam()
{
  log(L"\n");
  log(L"\n<b>Test to deviceSupportCheckSumParam().");
  return S_OK;
}

HRESULT TestDeviceIdHelper::testExtractFirmwareByStr()
{
  log(L"\n<b>Test to extractFirmwareByStr().");
  std::wstring str = L"\nUnexpected return value.";

  std::string manu = "FWA-XD1-AXS-V-0504N-NN-00";
  std::string ver = DeviceIdHelper::extractFirmwareByStr(DeviceSeries::CTRLXDRIVE, manu);
  LOG_TEST_RESULT_ANY(ver == "0504", str);

  manu = "FWA-XD1-AXS-V-0610A-NN-00";
  ver = DeviceIdHelper::extractFirmwareByStr(DeviceSeries::CTRLXDRIVE_XC20, manu);
  LOG_TEST_RESULT_ANY(ver == "0610", str);

  manu = "FWA-123";
  ver = DeviceIdHelper::extractFirmwareByStr(DeviceSeries::CTRLXDRIVE, manu);
  LOG_TEST_RESULT_ANY(ver == "", str);

  manu = "FWA-INDRV*-MPB-20V28-D5-1-SNC-NN";
  ver = DeviceIdHelper::extractFirmwareByStr(DeviceSeries::INDRADRIVE, manu);
  LOG_TEST_RESULT_ANY(ver == "2028", str);

  manu = "FWA-INDR";
  ver = DeviceIdHelper::extractFirmwareByStr(DeviceSeries::INDRADRIVE, manu);
  LOG_TEST_RESULT_ANY(ver == "", str);
  return S_OK;
}

HRESULT TestDeviceIdHelper::testConvertSoftwareVersion()
{
  log(L"\n<b>Test to convertSoftwareVersion().");
  std::wstring str = L"\nUnexpected return value.";
  std::uint8_t major = 0;
  std::uint8_t minor = 0;

  bool ok = DeviceIdHelper::convertSoftwareVersion("0504", major, minor);
  LOG_TEST_RESULT_ANY(ok, str);
  LOG_TEST_RESULT_ANY(major == 5, str);
  LOG_TEST_RESULT_ANY(minor == 4, str);

  ok = DeviceIdHelper::convertSoftwareVersion("123", major, minor);
  LOG_TEST_RESULT_ANY(!ok, str);
  return S_OK;
}

uint32_t TestDeviceIdHelper::makeProductCode(uint16_t highWord, uint16_t lowWord)
{
  return (static_cast<uint32_t>(highWord) << 16) | static_cast<uint32_t>(lowWord);
}
