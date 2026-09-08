#include "test_diagnosis.h"

// External Includes
#include "comm/datalayer/diagnosis_generated.h"

// Test Class
#include "diagnosis/dia.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestDia);
REGISTER_TEST_TO_GROUP(TestDia, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestDia, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestDia, Diagnosis);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestDia::TestDia()
{
}


TestDia::~TestDia(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestDia::doInit()
{
  if (m_testClass)
  {
    delete m_testClass;
    m_testClass = nullptr;
  }
  m_testClass = new Dia();
  return S_OK;
}


HRESULT TestDia::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the diagnosis object in the axis profile bundle");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;

  hresult |= testDiagnosisNotAvailable();
  hresult |= testSendDiag();
  hresult |= testSendDiagVariant();
  hresult |= testDeInit();

  return hresult;
}

HRESULT TestDia::doEnd()
{
  m_mockDiagRegistration.setTestLog(nullptr);
  delete m_testClass;
  m_testClass = nullptr;
  m_mockDiagLog.reset();
  return S_OK;
}


HRESULT TestDia::testDiagnosisNotAvailable()
{
  HRESULT hresult = S_OK;
  log(L"\n<b>----- Test behavior when diagnosis bundle is not available -----</b>");

  log(L"\nCall sendDiagnosis() when the diagnosis bundle is not set.");
  AxisProfileResult aprResult = Dia::sendDiagnosis(m_exampleMainDiagCode, m_exampleDetailedDiagCode, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, m_dynamicDesc.c_str());
  if (AxisProfileResult::STS_OK != aprResult)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() (variant overload) when the diagnosis bundle is not set.");
  aprResult = Dia::sendDiagnosis(nullptr, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  if (AxisProfileResult::STS_OK != aprResult)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }

  log(L"\nSet the diagnosis bundle in the diagnosis class.");
  m_mockDiagLog = std::make_shared<test::MockDiagnosisLog>();
  m_mockDiagRegistration.setTestLog(m_mockDiagLog);
  std::string testBundlePath;
  m_testClass->setDiagnosisHandler(&m_mockDiagRegistration, testBundlePath);

  log(L"\n");
  return hresult;
}

HRESULT TestDia::testSendDiag()
{
  HRESULT hresult = S_OK;
  log(L"\n<b>----- Test to send a diagnosis -----</b>");

  log(L"\nCall sendDiagnosis() when the diagnosis bundle is set.");
  AxisProfileResult aprResult = Dia::sendDiagnosis(m_exampleMainDiagCode, m_exampleDetailedDiagCode, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, m_dynamicDesc.c_str());
  if (AxisProfileResult::STS_OK != aprResult)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() without a profile name.");
  aprResult = Dia::sendDiagnosis(m_exampleMainDiagCode, m_exampleDetailedDiagCode, __FILE__, __FUNCTION__, __LINE__, m_dynamicDesc.c_str());
  if (AxisProfileResult::STS_OK != aprResult)
  {
    error(L"\nWrong return code! Expected: 'STS_OK' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() and simulate an error in the diagnosis log class.");
  static_cast<test::MockDiagnosisLog*>(m_mockDiagLog.get())->setReturnValue(common::log::diagnosis::DiagnosisResult::NOK);
  aprResult = Dia::sendDiagnosis(m_exampleMainDiagCode, m_exampleDetailedDiagCode, __FILE__, __FUNCTION__, __LINE__, m_dynamicDesc.c_str());
  if (AxisProfileResult::ERR_SEND_DIAGNOSIS != aprResult)
  {
    error(L"\nWrong return code! Expected: 'ERR_SEND_DIAGNOSIS' Actual: 0x%08X", aprResult);
    hresult = E_FAIL;
  }
  static_cast<test::MockDiagnosisLog*>(m_mockDiagLog.get())->setReturnValue(common::log::diagnosis::DiagnosisResult::OKAY);

  log(L"\n");
  return hresult;
}

HRESULT TestDia::testSendDiagVariant()
{
  HRESULT hresult = S_OK;
  log(L"\n<b>----- Test to send a diagnosis with variant data -----</b>");

  log(L"\nCall sendDiagnosis() when no variant data is available.");
  AxisProfileResult actResult = Dia::sendDiagnosis(nullptr, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  AxisProfileResult expResult = AxisProfileResult::ERR_EXEC_NULL;
  if (expResult != actResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expResult, actResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() with wrong variant type.");
  comm::datalayer::Variant data{};
  actResult = Dia::sendDiagnosis(&data, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  expResult = AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
  if (expResult != actResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expResult, actResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() with wrong flatbuffer (verify fail).");
  data.setType(comm::datalayer::VariantType::FLATBUFFERS);
  actResult = Dia::sendDiagnosis(&data, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  if (expResult != actResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expResult, actResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() with incomplete flatbuffer (missing field).");
  diagnosisFbData fbData{ m_exampleMainDiagCode, m_exampleDetailedDiagCode, m_dynamicDesc };
  generateDiagnosisFlatbuffer(fbData, data, true);
  actResult = Dia::sendDiagnosis(&data, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  if (expResult != actResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expResult, actResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() when everything is OK.");
  generateDiagnosisFlatbuffer(fbData, data);
  actResult = Dia::sendDiagnosis(&data, m_profileName.c_str(), __FILE__, __FUNCTION__, __LINE__, "");
  expResult = AxisProfileResult::STS_OK;
  if (expResult != actResult)
  {
    error(L"Method should have succeeded! Error Code: 0x%X", actResult);
    hresult = E_FAIL;
  }

  log(L"\nCall sendDiagnosis() when no variant data is available.");
  actResult = Dia::sendDiagnosis(nullptr, m_profileName.c_str(),  __FUNCTION__, __LINE__, "");
  expResult = AxisProfileResult::ERR_EXEC_NULL;
  if (expResult != actResult)
  {
    error(L"Method should have failed. Expected: 0x%X, Actual: 0x%X", expResult, actResult);
    hresult = E_FAIL;
  }

  log(L"\n");
  return hresult;
}

HRESULT TestDia::testDeInit()
{
  HRESULT hresult = S_OK;
  log(L"\n<b>----- Test the deinitialization of the diagnosis class -----</b>");

  log(L"\nRemove the diagnosis bundle from the diagnosis class.");
  m_testClass->removeDiagnosisHandler(&m_mockDiagRegistration);

  log(L"\n");
  return hresult;
}

bool TestDia::generateDiagnosisFlatbuffer(const diagnosisFbData& diagData, comm::datalayer::Variant& outData, bool leaveFieldEmpty)
{
  flatbuffers::FlatBufferBuilder builder;
  auto dynamicDesc = builder.CreateString(diagData.m_dynamicDescription);

  comm::datalayer::DiagnosisBuilder diagBuilder(builder);
  if (!leaveFieldEmpty)
  {
    diagBuilder.add_mainDiagnosisCode(diagData.m_mainDiagCode);
  }
  diagBuilder.add_detailedDiagnosisCode(diagData.m_detailedDiagCode);
  diagBuilder.add_dynamicDescription(dynamicDesc);

  // Finish
  builder.Finish(diagBuilder.Finish());
  outData.copyFlatbuffers(builder);
  return true;
}
