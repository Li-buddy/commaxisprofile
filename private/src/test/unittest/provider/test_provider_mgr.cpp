#include "test_provider_mgr.h"
#include "provider/provider_mgr.h"
#include "comm/datalayer/datalayer_system.h"

// mockups
#include "mockup/mock_axisprofile_cmp.h"
#include "mockup/mock_provider_node.h"
#include "mockup/external_bundles/mock_datalayer_factory.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestProviderManager);
REGISTER_TEST_TO_GROUP(TestProviderManager, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestProviderManager, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestProviderManager, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestProviderManager::TestProviderManager()
{
}


TestProviderManager::~TestProviderManager(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestProviderManager::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile provider manager");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hResult = S_OK;

  hResult |= doFailTest();
  hResult |= doSimpleTest();

  return hResult;
}

HRESULT TestProviderManager::doSimpleTest()
{
  HRESULT hResult = S_OK;
  log(L"\n<b>Test to start the provider manager successful.");

  test::MockAxisProfileCmp axisProfileCmp;

  log(L"\nConfigure test classes.");
  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();
  comm::datalayer::PersistenceHelper testPersistenceHelper(&testDatalayerSystem);
  axisProfileCmp.setPersistenceHelper(&testPersistenceHelper);
  axisProfileCmp.addDataLayer(testDatalayerSystem.factory());

  provider::ProviderMgr testProviderMgr;
  testProviderMgr.init(&axisProfileCmp);

  log(L"\nRegister additional node (mockup) for the test.");
  log(L"\nUse the generated .bfbs file of the dummyFbs.fbs flatbuffer stored in the public folder for the mockup node.");
  test::MockProviderNode* mockupNode = new test::MockProviderNode(); // Will be deleted by the ProviderMgr
  mockupNode->setBfbsName("/dummyFbs.bfbs");
  testProviderMgr.addNode("comm/axisprofile/mock/node", mockupNode);

  log(L"\nStart provider manager (should succeed).");
  if ( !testProviderMgr.run() )
  {
    error(L"Start of provider manager failed.");
    hResult = E_FAIL;
  }

  log(L"\nRestart provider manager (should fail).");
  if ( !testProviderMgr.run() )
  {
    error(L"Start of provider manager succeeded but should fail.");
    hResult = E_FAIL;
  }

  log(L"\nStop provider manager (should succeed).");
  if ( !testProviderMgr.stop() )
  {
    error(L"Stop of provider manager failed.");
    hResult = E_FAIL;
  }

  if ( hResult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  testDatalayerSystem.stop();
  log(L"\n");

  return hResult;
}

HRESULT TestProviderManager::doFailTest()
{
  HRESULT hResult = S_OK;

  log(L"\n<b>Test to start the provider manager with errors.");

  test::MockAxisProfileCmp axisProfileCmp;

  log(L"\nCreate and delete provider manager without datalayer.");
  provider::ProviderMgr* tempProvider = new provider::ProviderMgr();
  tempProvider->init(&axisProfileCmp);
  delete tempProvider;
  tempProvider = nullptr;

  log(L"\nStart provider manager with datalayer factory mockup (should fail).");
  test::MockDatalayerFactory datalayerFactoryMock;
  axisProfileCmp.addDataLayer(&datalayerFactoryMock);

  // Init persistence helper
  comm::datalayer::DatalayerSystem testDatalayerSystem;
  testDatalayerSystem.start();
  comm::datalayer::PersistenceHelper testPersistenceHelper(&testDatalayerSystem);
  axisProfileCmp.setPersistenceHelper(&testPersistenceHelper);

  provider::ProviderMgr testProviderMgr1;
  testProviderMgr1.init(&axisProfileCmp);
  if (testProviderMgr1.run())
  {
    error(L"Start of provider manager with datalayer factory mockup succeeded.");
    hResult = E_FAIL;
  }

  log(L"\nStart provider manager with datalayer factory mockup and simulate createClient() error.");
  datalayerFactoryMock.simulateCreateClientError(true);
  if (testProviderMgr1.run())
  {
    error(L"Start of provider manager with datalayer factory mockup succeeded.");
    hResult = E_FAIL;
  }

  log(L"\nStart provider manager with datalayer factory mockup and simulate createProvider() error.");
  datalayerFactoryMock.simulateCreateProviderError(true);
  if (testProviderMgr1.run())
  {
    error(L"Start of provider manager with datalayer factory mockup succeeded.");
    hResult = E_FAIL;
  }

  testProviderMgr1.stop();

  log(L"\nConfigure test classes.");
  axisProfileCmp.removeDatalayer(nullptr);

  provider::ProviderMgr testProviderMgr2;

  log(L"\nStart provider manager without axisprofile component (should fail).");
  if (testProviderMgr2.run() )
  {
    error(L"Start of provider manager without axisprofile component succeeded.");
    hResult = E_FAIL;
  }

  log(L"\nStart provider manager without datalayer but with registered nodes (should succeed).");
  testProviderMgr2.init(&axisProfileCmp);
  if ( !testProviderMgr2.run() )
  {
    error(L"Start of provider manager without datalayer but with registered nodes failed.");
    hResult = E_FAIL;
  }

  log(L"\nStart provider manager with everything set (should succeed).");
  axisProfileCmp.addDataLayer(testDatalayerSystem.factory());
  if ( !testProviderMgr2.run() )
  {
    error(L"Start of provider manager failed.");
    hResult = E_FAIL;
  }

  if ( hResult == S_OK )
  {
    log(L"... <DARK_GREEN>OK</DARK_GREEN>");
  }

  testProviderMgr2.stop();
  testDatalayerSystem.stop();
  log(L"\n");

  return hResult;
}
