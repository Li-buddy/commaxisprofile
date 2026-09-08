#include "mockup/mock_scaling.h"
#include "test_profile_probe.h"
#include "helper/result_helper.h"

#include "comm/axisprofile/interfaces/i_axisprofile_probe_c.h"

using namespace comm::axisprofile;


REGISTER_TEST(TestProfileProbe);
REGISTER_TEST_TO_GROUP(TestProfileProbe, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProfileProbe, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProfileProbe, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProfileProbe::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test base class for all probe commands.");
  log(L"\n----------------------------------------------------------------");

  log(L"\n");
  hResult |= testCInterface();
  hResult |= testMisc();
  log(L"\n");

  return hResult;
}

HRESULT TestProfileProbe::testCInterface()
{
  HRESULT hResult = S_OK;
  test::MockAxisProfileTrace profileTraceMock{};
  std::uint8_t driveNumber = 0;
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = m_deviceAddress;
  profileBasicInfo.m_driveNumber = driveNumber;
  profileBasicInfo.m_fieldbusAdapter = nullptr;
  profileBasicInfo.m_profileName = "testProfile";

  AxisProfileProbeUnittest testClass(profileBasicInfo);

  log(L"\n<b>Test of the probe C-Interface</b>");

  log(L"\nCall getCInterface()...");
  const IAxisProfileProbeC* probeItf = testClass.getCInterface();
  if (!probeItf)
  {
    error(L"\nCould not get the C-Interface (received nullptr)!");
    return E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeBaseConfig() with nullptr handle...");
  ProbeIndex probeIdx = ProbeIndex::PROBE_1;
  ProbeBaseConfig probeConfig{};
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  AxisProfileResult aprResult = probeItf->probeBaseConfig(nullptr, probeIdx, probeConfig, false);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeBaseConfig()...");
  aprResult = probeItf->probeBaseConfig(probeItf->handle, probeIdx, probeConfig, false);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeExtConfig() with nullptr handle...");
  ProbeExtConfig probeExtendConfig{};
  aprResult = probeItf->probeExtConfig(nullptr, probeIdx, probeExtendConfig, false);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeExtConfig()...");
  aprResult = probeItf->probeExtConfig(probeItf->handle, probeIdx, probeExtendConfig, false);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeStatusCtrl() with nullptr handle...");
  aprResult = probeItf->probeStatusCtrl(nullptr, probeIdx, false);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_probeStatusCtrl()...");
  aprResult = probeItf->probeStatusCtrl(probeItf->handle, probeIdx, false);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getProbeActualValuesRT() with nullptr handle...");
  ProbeActualValues actualValueProbe{};
  aprResult = probeItf->getProbeActualValuesRT(nullptr, probeIdx, &actualValueProbe);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getProbeActualValuesRT()...");
  aprResult = probeItf->getProbeActualValuesRT(probeItf->handle, probeIdx, &actualValueProbe);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_onlineChangeParams() with nullptr handle...");
  ProbeUpdateMeasurement updateValue{};
  aprResult = probeItf->onlineChangeParams(nullptr, probeIdx, updateValue);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_onlineChangeParams()...");
  aprResult = probeItf->onlineChangeParams(probeItf->handle, probeIdx, updateValue);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getProbeValues() with nullptr handle...");
  aprResult = probeItf->getProbeValues(nullptr, probeIdx, &probeConfig, &probeExtendConfig);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_getProbeValues()...");
  aprResult = probeItf->getProbeValues(probeItf->handle, probeIdx, &probeConfig, &probeExtendConfig);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_abortAndResetProbe() with nullptr handle...");
  aprResult = probeItf->abortAndResetProbe(nullptr);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_abortAndResetProbe()...");
  aprResult = probeItf->abortAndResetProbe(probeItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_isProbeSupported() with nullptr handle...");
  aprResult = probeItf->isProbeSupported(nullptr);
  if (aprResult != expResult)
  {
    error(L"Method did not fail with expected error code! Expected: 0x%08X, Actual: 0x%08X", expResult, aprResult);
    hResult = E_FAIL;
  }

  log(L"\nCall AXISPROFILE_isProbeSupported()...");
  aprResult = probeItf->isProbeSupported(probeItf->handle);
  if (aprStatusFailed(aprResult))
  {
    error(L"Method should have succeeded! Return Code: 0x%08X", aprResult);
    hResult = E_FAIL;
  }

  log(L"\n");
  
  return hResult;
}

HRESULT TestProfileProbe::testMisc()
{
  ProfileBasicInfo profileBasicInfo;
  profileBasicInfo.m_deviceAddress = 100;
  profileBasicInfo.m_profileName = "profile";
  AxisProfileProbeUnittest testClass(profileBasicInfo);

  log(L"\nTest success case of item list checker...");
  std::vector<EnableCheckItem> checkList;
  checkList.push_back({"item1", true, "condition1", true, "0x1234"});
  bool res = testClass.itemListChecker(ProbeIndex::PROBE_1, checkList);
  LOG_TEST_RESULT_ANY(res == true, L"\nUnexpected return value");
  checkList.clear();

  log(L"\nTest success case of item list checker...");
  checkList.push_back({"item1", true, "condition1", false, "0x1234"});
  res = testClass.itemListChecker(ProbeIndex::PROBE_1, checkList);
  LOG_TEST_RESULT_ANY(res == false, L"\nUnexpected return value");

  return S_OK;
}
