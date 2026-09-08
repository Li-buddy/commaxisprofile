#include <thread>
#include "functest_provider_system_boot_state.h"
#include "configuration/i_boot_mgr.h"
#include "provider/provider_helper.h"
#include "helper/provider_helper_functest.h"
#include "comm/axisprofile/datatypes/aprBootState_generated.h"
#include "comm/axisprofile/datatypes/dummyFbs_generated.h"


using namespace comm::axisprofile;

REGISTER_TEST(FuncTestProviderBootState);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootState, 1_AutoTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootState, 3_FuncTest);
REGISTER_TEST_TO_GROUP(FuncTestProviderBootState, Provider);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

FuncTestProviderBootState::FuncTestProviderBootState()
{
}


FuncTestProviderBootState::~FuncTestProviderBootState(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT FuncTestProviderBootState::doInit()
{
  return startTestEnvironment();
}

HRESULT FuncTestProviderBootState::doEnd()
{
  return stopTestEnvironment();
}

HRESULT FuncTestProviderBootState::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Functiontest for profile system boot state via datalayer.");
  log(L"\n----------------------------------------------------------------\n");
  std::string urlOutputBuffer = comm::axisprofile::provider::NodeTree.APR_SYSTEM_BOOT_STATE;
  HRESULT hResult = S_OK;
  comm::datalayer::Variant fb;
  FbVerifier fbVerifier;
  bool emptyInput = false;
  hResult |= createFlatbuffer(&fb, fbVerifier);

  // test cases
  hResult |= testCompleteRead(&fb, urlOutputBuffer, fbVerifier, emptyInput);
  hResult |= testPartialRead (&fb, urlOutputBuffer, fbVerifier);
  hResult |= testFailRead(urlOutputBuffer);
  hResult |= testMetadata(urlOutputBuffer);

  return hResult;
}

HRESULT FuncTestProviderBootState::createFlatbuffer(comm::datalayer::Variant* var, FbVerifier &fbVerifier)
{
  HRESULT hResult = S_OK;

  std::uint32_t bootActStep;
  std::uint32_t bootMaxStep;
  std::string bootTxt;
  m_axisProfileComponent->getBootManager()->getBootingState(bootActStep, bootMaxStep, bootTxt);

  flatbuffers::FlatBufferBuilder builder;
  builder.ForceDefaults(false);
  auto textFB = builder.CreateString(bootTxt.c_str());

  auto fb = fbtypes::CreateAPRBootState(builder, textFB, bootActStep, bootMaxStep);
  FinishAPRBootStateBuffer(builder, fb);
  fbVerifier = fbtypes::VerifyAPRBootStateBuffer;
  var->copyFlatbuffers(builder);
  return hResult; 
}

HRESULT FuncTestProviderBootState::testCompleteRead(const comm::datalayer::Variant* expFb, const std::string& url, FbVerifier &fbVerifier, bool failOnEmpty)
{
  log(L"\nRead complete fbs:");
  HRESULT hResult = S_OK;
  comm::datalayer::Variant actData;
  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, url.c_str(), &actData, !failOnEmpty, expFb, fbVerifier));
  return hResult;
}

HRESULT FuncTestProviderBootState::testPartialRead(const comm::datalayer::Variant* expFb, const std::string& url, FbVerifier &fbVerifier)
{
  log(L"\nRead partial fbs:");
  HRESULT hResult = S_OK;
  comm::datalayer::Variant actData = *expFb; // reads complete fb to increase test coverage
  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, url.c_str(), &actData, true, expFb, fbVerifier));
  return hResult;
}

HRESULT FuncTestProviderBootState::testFailRead(const std::string& url)
{
  log(L"\nRead invalid fbs:");
  HRESULT hResult = S_OK;
  // create wrong flatbuffer (dummyFbs)
  comm::datalayer::Variant dummyFbs;
  flatbuffers::FlatBufferBuilder fbBuilder;
  fbBuilder.ForceDefaults(true);
  fbtypes::dummyFbsBuilder dummyFbBuilder(fbBuilder);

  dummyFbBuilder.add_dummyInt(10);
  dummyFbBuilder.add_dummyByte(0);
  dummyFbBuilder.add_dummyDouble(123.4);

  auto fbFinisher = dummyFbBuilder.Finish();
  fbBuilder.Finish(fbFinisher);
  dummyFbs.copyFlatbuffers(fbBuilder);

  PROVIDER_CHECK(test::ProviderHelperFunctest::readNodeSync(this, m_datalayerTestClient, url.c_str(), &dummyFbs, false));
  return hResult;
}

HRESULT FuncTestProviderBootState::testMetadata(const std::string & url)
{
  log(L"\nRead metadata:");
  HRESULT hResult = S_OK;
  comm::datalayer::Variant actMetaData;
  PROVIDER_CHECK(test::ProviderHelperFunctest::readMetadataSync(this, m_datalayerTestClient, url.c_str(), &actMetaData));
  // ToDo: Compare actMetaData with database in future
  return hResult;
 
}


