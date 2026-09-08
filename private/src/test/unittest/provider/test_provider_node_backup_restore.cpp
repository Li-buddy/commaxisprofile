#include <thread>
#include "test_provider_node_backup_restore.h"
#include "helper/provider_helper_unittest.h"

// Mockups
#include "mockup/mock_axisprofile_cmp.h"

// Test Class
#include "provider/nodes/node_backup_restore.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderNodeBackupRestore);
REGISTER_TEST_TO_GROUP(TestProviderNodeBackupRestore, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeBackupRestore, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderNodeBackupRestore, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderNodeBackupRestore::TestProviderNodeBackupRestore()
{
}


TestProviderNodeBackupRestore::~TestProviderNodeBackupRestore(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestProviderNodeBackupRestore::doInit()
{
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressLoadSingle, m_profileName);
  test::ProviderHelperCommon::insertProfileName(m_nodeAddressSaveSingle, m_profileName);
  return S_OK;
}


HRESULT TestProviderNodeBackupRestore::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------------");
  log(L"\n Test the backup-restore datalayer node for the axisprofiles. ");
  log(L"\n----------------------------------------------------------------------\n");

  hResult |= testNode();
  hResult |= testErrors();

  return hResult;
}


HRESULT TestProviderNodeBackupRestore::testErrors()
{
  log(L"\n<b>----- Test all possible errors -----</b>");
  HRESULT hResult = S_OK;

  provider::NodeBackupRestore nodeInvalid(nullptr, provider::SaveLoadOperation::LOAD_SINGLE_PROFILE, {"", ""});

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
  provider::NodeBackupRestore node(mockComponent.getProviderManager(), provider::SaveLoadOperation::LOAD_SINGLE_PROFILE, {"", ""});
  testBackupRestoreManager->setReturnValue(comm::axisprofile::AxisProfileResult::ERR_NOK);

  log(L"\nTest onCreate with fail in operation.");
  log(L"\nonCreate: %ls", UTF8TOWIDE(m_nodeAddressLoadSingle.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_OK;
  node.onCreate(m_nodeAddressLoadSingle, nullptr, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  expResult = comm::datalayer::DlResult::DL_SUBMODULE_FAILURE;
  if (test::ProviderHelperUnittest::m_returnStatus != expResult)
  {
    error(L"\nOnCreate did not return expResult. Result: %ls, Exp: 0x%08X", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()), UTF8TOWIDE(expResult.toString()));
    hResult = E_FAIL;
  }

  log(L"\n");
  return hResult;
}


HRESULT TestProviderNodeBackupRestore::testNode()
{
  log(L"\n<b>----- Test all class methods -----</b>");
  HRESULT hResult = S_OK;

  hResult |= testOperation(provider::SaveLoadOperation::LOAD_SINGLE_PROFILE);
  hResult |= testOperation(provider::SaveLoadOperation::SAVE_SINGLE_PROFILE);
  hResult |= testOperation(provider::SaveLoadOperation::LOAD_ALL);
  hResult |= testOperation(provider::SaveLoadOperation::SAVE_ALL);

  hResult |= testOperationPath(provider::SaveLoadOperation::LOAD_SINGLE_PROFILE);
  hResult |= testOperationPath(provider::SaveLoadOperation::SAVE_SINGLE_PROFILE);
  hResult |= testOperationPath(provider::SaveLoadOperation::LOAD_ALL);
  hResult |= testOperationPath(provider::SaveLoadOperation::SAVE_ALL);

  log(L"\n");
  return hResult;
}

HRESULT TestProviderNodeBackupRestore::testOperation(comm::axisprofile::provider::SaveLoadOperation operation)
{
  log(L"\n<b>Run testcase for operation </b>");
  std::string currentNodeAddress{};
  switch ( operation )
  {
    case comm::axisprofile::provider::SaveLoadOperation::SAVE_SINGLE_PROFILE:
      currentNodeAddress = m_nodeAddressSaveSingle;
      log(L"<b> save single profile.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::LOAD_SINGLE_PROFILE:
      currentNodeAddress = m_nodeAddressLoadSingle;
      log(L"<b> load single profile.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::SAVE_ALL:
      currentNodeAddress = m_nodeAddressSaveSystem;
      log(L"<b> save all profiles.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::LOAD_ALL:
      currentNodeAddress = m_nodeAddressLoadSystem;
      log(L"<b> load all profiles.");
      break;
    default:
      return E_FAIL;
      break;
  }

  test::MockAxisProfileCmp mockComponent;
  provider::NodeBackupRestore node(mockComponent.getProviderManager(), operation, {"", ""});

  log(L"\nonCreate: %ls", UTF8TOWIDE(currentNodeAddress.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onCreate(currentNodeAddress, nullptr, test::ProviderHelperUnittest::m_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  if (test::ProviderHelperUnittest::m_returnStatus != comm::datalayer::DlResult::DL_OK)
  {
    error(L"\nOperation failed. Result: %ls", UTF8TOWIDE(test::ProviderHelperUnittest::m_returnStatus.toString()));
    return E_FAIL;
  }

  return S_OK;
}

HRESULT TestProviderNodeBackupRestore::testOperationPath(comm::axisprofile::provider::SaveLoadOperation operation)
{
  log(L"\n<b>Run testcase for operation with path</b>");
  std::string currentNodeAddress{};
  switch ( operation )
  {
    case comm::axisprofile::provider::SaveLoadOperation::SAVE_SINGLE_PROFILE:
      currentNodeAddress = m_nodeAddressSaveSingle;
      log(L"<b> save single profile.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::LOAD_SINGLE_PROFILE:
      currentNodeAddress = m_nodeAddressLoadSingle;
      log(L"<b> load single profile.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::SAVE_ALL:
      currentNodeAddress = m_nodeAddressSaveSystem;
      log(L"<b> save all profiles.");
      break;
    case comm::axisprofile::provider::SaveLoadOperation::LOAD_ALL:
      currentNodeAddress = m_nodeAddressLoadSystem;
      log(L"<b> load all profiles.");
      break;
    default:
      return E_FAIL;
      break;
  }
  test::MockAxisProfileCmp mockComponent;
  provider::NodeBackupRestore node(mockComponent.getProviderManager(), operation, {"", ""});

  std::string testpath{ "invalidPath" };
  comm::datalayer::Variant data;
  data.setValue(testpath.c_str());

  log(L"\nonCreate: %ls", UTF8TOWIDE(currentNodeAddress.c_str()));
  test::ProviderHelperUnittest::m_returnStatus = comm::datalayer::DlResult::Value::DL_FAILED;
  node.onCreate(currentNodeAddress, &data, test::ProviderHelperUnittest::m_callback);
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
