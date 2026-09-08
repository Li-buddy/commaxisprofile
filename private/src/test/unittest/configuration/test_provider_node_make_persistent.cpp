#include <thread>
#include "test_provider_node_make_persistent.h"
#include "helper/provider_helper_unittest.h"
#include "comm/datalayer/persistence_param_generated.h"
// Mockups
#include "mockup/mock_axisprofile_cmp.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestNodeCfgMakePersistent);
REGISTER_TEST_TO_GROUP(TestNodeCfgMakePersistent, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestNodeCfgMakePersistent, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestNodeCfgMakePersistent, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestNodeCfgMakePersistent::doInit()
{
  HRESULT res = S_OK;

  test::ProviderHelperCommon::insertProfileName(m_nodeAddressMakePersistent, m_profileName);

  return res;
}

HRESULT TestNodeCfgMakePersistent::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestNodeCfgMakePersistent");
  log(L"\n----------------------------------------------------------------\n");

  res |= testNode();
  res |= testErrors();

  return res;
}

HRESULT TestNodeCfgMakePersistent::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");
  HRESULT hResult = S_OK;

  provider::NodeCfgMakePersistent nodeInvalid(nullptr);

  log(L"\nTest onCreate without component.");
  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressLoadSingle.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_OK;
  nodeInvalid.onCreate(m_nodeAddressLoadSingle, nullptr, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  comm::datalayer::DlResult expResult = comm::datalayer::DlResult::DL_INVALID_HANDLE;
  if (test::ProviderHelperUnittest::m_returnStatus != expResult)
  {
    error(L"\nOnCreate did not return expResult. Result: %ls, Exp: 0x%08X", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()), UTF8TOWIDE(expResult.toString()));
    return E_FAIL;
  }

  test::MockAxisProfileCmp mockComponent;
  test::MockBackupRestoreManager* testBackupRestoreManager = new test::MockBackupRestoreManager();
  mockComponent.setBackupRestoreManager(true, testBackupRestoreManager);
  provider::NodeCfgMakePersistent node(mockComponent.getProviderManager());
  testBackupRestoreManager->setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);

  log(L"\nTest onCreate with fail in operation.");
  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressLoadSingle.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_INVALID_HANDLE;
  node.onCreate(m_nodeAddressLoadSingle, nullptr, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  expResult = comm::datalayer::DlResult::DL_INVALID_HANDLE;
  if (test::ProviderHelperUnittest::m_returnStatus != expResult)
  {
    error(L"\nOnCreate did not return expResult. Result: %ls, Exp: 0x%08X", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()), UTF8TOWIDE(expResult.toString()));
    hResult = E_FAIL;
  }

  provider::NodeCfgMakePersistent node2(mockComponent.getProviderManager());
  testBackupRestoreManager->setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);
  log(L"\nTest onCreate with fail in operation.");
  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressLoadSingle.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_OK;

  comm::datalayer::Variant fbCreateData;
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  createFlatbuffer("path", loadPhasetoString(static_cast<LoadPhases>(2)), &fbCreateData);
  node2.onCreate(m_nodeAddressLoadSingle, &fbCreateData, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  expResult = comm::datalayer::DlResult::DL_SUBMODULE_FAILURE;
  if (test::ProviderHelperUnittest::m_returnStatus != expResult)
  {
    error(L"\nOnCreate did not return expResult. Result: %ls, Exp: 0x%08X", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()), UTF8TOWIDE(expResult.toString()));
    hResult = E_FAIL;
  }
  node2.onMetadata(m_nodeAddressLoadSingle, test::ProviderHelperUnittest::m_callback); //for code coverage

  log(L"\n");
  return hResult;
}

HRESULT TestNodeCfgMakePersistent::testNode()
{
  log(L"\n<b>----- Test all class methods -----</b>");
  HRESULT hResult = S_OK;

  hResult |= testOperation();

  log(L"\n");
  return hResult;
}

HRESULT TestNodeCfgMakePersistent::testOperation()
{
  log(L"\n<b>Run testcase for operation </b>");

  test::MockAxisProfileCmp mockComponent;
  provider::NodeCfgMakePersistent node(mockComponent.getProviderManager());

  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressMakePersistent.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;

  comm::datalayer::Variant fbCreateData;
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  createFlatbuffer("path", loadPhasetoString(static_cast<LoadPhases>(2)), &fbCreateData);

  node.onCreate(m_nodeAddressMakePersistent, &fbCreateData, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestNodeCfgMakePersistent::testOperationPath()
{
  log(L"\n<b>Run testcase for operation with path</b>");

  test::MockAxisProfileCmp mockComponent;
  provider::NodeCfgMakePersistent node(mockComponent.getProviderManager());

  comm::datalayer::Variant fbCreateData;
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  createFlatbuffer("path", loadPhasetoString(static_cast<LoadPhases>(2)), &fbCreateData);


  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressMakePersistent.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onCreate(m_nodeAddressMakePersistent, &fbCreateData, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\nonMetadata: %ls", UTF8TOWIDE(m_nodeAddressMakePersistent.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onMetadata(m_nodeAddressMakePersistent, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestNodeCfgMakePersistent::createFlatbuffer(const std::string& configPath, const std::string& phase, comm::datalayer::Variant* var, bool defaultData /*= false*/)
{
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);

  auto fbPath = builder.CreateString(configPath);
  auto fbId = builder.CreateString("id123");
  auto fbPhase = builder.CreateString(phase);

  if (defaultData)
  {
    auto fb = comm::datalayer::CreatePersistenceParam(builder);
    comm::datalayer::FinishPersistenceParamBuffer(builder, fb);
  }
  else
  {
    auto fb = comm::datalayer::CreatePersistenceParam(builder, fbPath, fbId, fbPhase);
    comm::datalayer::FinishPersistenceParamBuffer(builder, fb);
  }

  var->copyFlatbuffers(builder);

  return S_OK;
}
