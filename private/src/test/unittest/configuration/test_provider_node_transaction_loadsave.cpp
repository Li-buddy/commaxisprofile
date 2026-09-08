#include <thread>
#include "test_provider_node_transaction_loadsave.h"
#include "helper/provider_helper_unittest.h"
#include "comm/datalayer/persistence_param_generated.h"
// Mockups
#include "mockup/mock_axisprofile_cmp.h"



using namespace comm::axisprofile;

REGISTER_TEST(TestProviderNodeTransactionLoadSave);
REGISTER_TEST_TO_GROUP(TestProviderNodeTransactionLoadSave, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeTransactionLoadSave, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeTransactionLoadSave, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeTransactionLoadSave::TestProviderNodeTransactionLoadSave()
{
}


TestProviderNodeTransactionLoadSave::~TestProviderNodeTransactionLoadSave(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderNodeTransactionLoadSave::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressLoadSystem, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressSaveSystem, m_profileName);
  return S_OK;
}


HRESULT TestProviderNodeTransactionLoadSave::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the transaction based load and save datalayer node for the axisprofiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testNode();
  hResult |= testErrors();

  return hResult;
}


HRESULT TestProviderNodeTransactionLoadSave::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");
  HRESULT hResult = S_OK;

  provider::NodeTransactionLoadSave nodeInvalid(nullptr, provider::PersistanceOperation::LOAD_AXISPROFILE, {"",""});

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
  provider::NodeTransactionLoadSave node(mockComponent.getProviderManager(), provider::PersistanceOperation::LOAD_AXISPROFILE, {"", ""});
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

  provider::NodeTransactionLoadSave node2(mockComponent.getProviderManager(), static_cast<provider::PersistanceOperation>(-1), {"", ""});
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


HRESULT TestProviderNodeTransactionLoadSave::testNode()
{
  log(L"\n<b>----- Test all class methods -----</b>");
  HRESULT hResult = S_OK;

  hResult |= testOperation(provider::PersistanceOperation::LOAD_AXISPROFILE);
  hResult |= testOperation(provider::PersistanceOperation::SAVE_AXISPROFILE);

  hResult |= testOperationPath(provider::PersistanceOperation::LOAD_AXISPROFILE);
  hResult |= testOperationPath(provider::PersistanceOperation::SAVE_AXISPROFILE);

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeTransactionLoadSave::testOperation(comm::axisprofile::provider::PersistanceOperation operation)
{
  log(L"\n<b>Run testcase for operation </b>");
  std::string currentNodeAddress{};
  switch ( operation )
  {
    case comm::axisprofile::provider::PersistanceOperation::SAVE_AXISPROFILE:
      currentNodeAddress = m_nodeAddressSaveSystem;
      log(L"<b> save all profiles.");
      break;
    case comm::axisprofile::provider::PersistanceOperation::LOAD_AXISPROFILE:
      currentNodeAddress = m_nodeAddressLoadSystem;
      log(L"<b> load all profiles.");
      break;
    default:
      return E_FAIL;
      break;
  }

  test::MockAxisProfileCmp mockComponent;
  provider::NodeTransactionLoadSave node(mockComponent.getProviderManager(), operation, {"", ""});

  log(L"\nonCreate: %ls", UTF8TOWIDE(currentNodeAddress.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;

  comm::datalayer::Variant fbCreateData;
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  createFlatbuffer("path", loadPhasetoString(static_cast<LoadPhases>(2)), &fbCreateData);

  node.onCreate(currentNodeAddress, &fbCreateData, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestProviderNodeTransactionLoadSave::testOperationPath(comm::axisprofile::provider::PersistanceOperation operation)
{
  log(L"\n<b>Run testcase for operation with path</b>");
  std::string currentNodeAddress{};
  switch ( operation )
  {
    case comm::axisprofile::provider::PersistanceOperation::SAVE_AXISPROFILE:
      currentNodeAddress = m_nodeAddressSaveSystem;
      log(L"<b> save all profiles.");
      break;
    case comm::axisprofile::provider::PersistanceOperation::LOAD_AXISPROFILE:
      currentNodeAddress = m_nodeAddressLoadSystem;
      log(L"<b> load all profiles.");
      break;
    default:
      return E_FAIL;
      break;
  }
  test::MockAxisProfileCmp mockComponent;
  provider::NodeTransactionLoadSave node(mockComponent.getProviderManager(), operation, {"", ""});

  comm::datalayer::Variant fbCreateData;
  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(true);
  createFlatbuffer("path", loadPhasetoString(static_cast<LoadPhases>(2)), &fbCreateData);


  log(L"\nonCreate: %ls", UTF8TOWIDE(currentNodeAddress.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onCreate(currentNodeAddress, &fbCreateData, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  log(L"\nonMetadata: %ls", UTF8TOWIDE(currentNodeAddress.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onMetadata(currentNodeAddress, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}


HRESULT TestProviderNodeTransactionLoadSave::createFlatbuffer(const std::string& configPath, const std::string& phase, comm::datalayer::Variant* var, bool defaultData /*= false*/)
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
