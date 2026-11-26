#include "helper/datalayer_memory_map_helper.h"
#include "profiles/category/encoder/category_adapter_encoder.h"
#include "profiles/category/drive/category_drive_defs.h"
#include "profiles/category/drive/drive_category_adapter_soe.h"
#include "profiles/value_mapping/mapping_handler.h"
#include "test_mapping_handler.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"

// External Includes
#include "comm/datalayer/datalayer_types.h"
#include "comm/datalayer/memory_map_generated.h"

// Test Class
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "test/helper/result_helper.h"


REGISTER_TEST(TestMappingHandler);
REGISTER_TEST_TO_GROUP(TestMappingHandler, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestMappingHandler, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestMappingHandler, Profiles);

using namespace comm::axisprofile;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestMappingHandler::TestMappingHandler()
{
}

TestMappingHandler::~TestMappingHandler(void)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestMappingHandler::doTest()
{
  HRESULT returnVal = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile mapping handler.");
  log(L"\n----------------------------------------------------------------");

  returnVal |= testErrors();
  returnVal |= testHandler();
  returnVal |= testVectorConfig();
  returnVal |= testStringConvertMethods();

  return returnVal;
}


HRESULT TestMappingHandler::testErrors()
{
  HRESULT result = S_OK;
  AxisProfileResult retVal;
  test::MockAxisProfileTrace m_profileTraceMockup{};
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;

  AxisProfileMappingHandler mappingHandler(profileBasicInfo);

  auto pProfileCategory = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  auto sharingHandler = AxisProfileSharingHandler(&profileBasicInfo.m_profileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandler);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, pProfileCategory.get());

  log(L"\n\n<b>----- Test all possible errors -----</b>");

  log(L"\nCall configureInputValue() with invalid arguments.");
  retVal = mappingHandler.configureInputValue(InputMappingValueID::STATUSWORD, nullptr);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall configureOutputValue() with invalid arguments.");
  retVal = mappingHandler.configureOutputValue(OutputMappingValueID::CONTROLWORD, nullptr);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall initRealTimeMappingInput() with invalid arguments.");
  retVal = mappingHandler.initRealTimeMappingInput(nullptr);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall initRealTimeMappingOutput() with invalid arguments.");
  retVal = mappingHandler.initRealTimeMappingOutput(nullptr);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nCall getConfiguredInputURI() for a non-existing input value.");
  const char* receivedUri = mappingHandler.getConfiguredInputURI(InputMappingValueID::UNKNOWN);
  if ( receivedUri )
  {
    error(L"\nExpected a null-pointer but received data!", retVal);
    result = E_FAIL;
  }

  log(L"\nCall configureInputType() for a wrong mapping type.");
  retVal = mappingHandler.configureInputCfgInfo(InputMappingValueID::POSITION, {MappingGroup::UNKNOWN,true});
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\nCall getConfiguredOutputURI() for a non-existing output value.");
  receivedUri = mappingHandler.getConfiguredOutputURI(OutputMappingValueID::UNKNOWN);
  if ( receivedUri )
  {
    error(L"\nExpected a null-pointer but received data!", retVal);
    result = E_FAIL;
  }

  log(L"\n\nCreate a simple memory map for further testing.");
  test::MemoryMapHelper memMapHelper;
  memMapHelper.addVariable("TestVar1", 0, 16, comm::datalayer::TYPE_PLC_WORD);
  memMapHelper.addVariable("TestVar2", 16, 32, comm::datalayer::TYPE_PLC_UDINT);
  auto memMapVariant = memMapHelper.createMemoryMap(0);
  auto memMap = comm::datalayer::GetMemoryMap(memMapVariant.getData());

  log(L"\nConfigure a non existing input and output to simulate next errors.");
  mappingHandler.configureInputValue(InputMappingValueID::STATUSWORD, "ThisNameDoesNotExist");
  mappingHandler.configureOutputValue(OutputMappingValueID::CONTROLWORD, "ThisNameDoesNotExist");

  log(L"\n\nCall initRealTimeMappingInput() with empty input mapping configuration.");
  retVal = mappingHandler.initRealTimeMappingInput(memMap);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_DLRT_MEMMAP_VAR_NOT_FOUND);

  log(L"\nCall initRealTimeMappingOutput() with empty input mapping configuration.");
  retVal = mappingHandler.initRealTimeMappingOutput(memMap);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_DLRT_MEMMAP_VAR_NOT_FOUND);

  return result;
}

HRESULT TestMappingHandler::testHandler()
{
  HRESULT result = S_OK;
  AxisProfileResult retVal;
  test::MockAxisProfileTrace m_profileTraceMockup{};
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;

  AxisProfileMappingHandler mappingHandler(profileBasicInfo);

  log(L"\n\n<b>----- Test all class methods -----</b>");

  log(L"\nCall configureInputValue() to configure an input.");
  retVal = mappingHandler.configureInputValue(InputMappingValueID::STATUSWORD, "ABC");
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall getConfiguredInputURI() to check the input configuration.");
  const char* receivedUri = mappingHandler.getConfiguredInputURI(InputMappingValueID::STATUSWORD);
  if ( !receivedUri )
  {
    error(L"\nExpected a null-pointer but received data!", retVal);
    return E_FAIL;
  }
  std::string convUriIn(receivedUri);
  if ( convUriIn.compare("ABC") != 0 )
  {
    error(L"\nThe written and the read configuration is unequal!", retVal);
    return E_FAIL;
  }

  log(L"\nCall configureInputValue() again to overwrite the existing configuration.");
  retVal = mappingHandler.configureInputValue(InputMappingValueID::STATUSWORD, "TestStatusword");
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall configureOutputValue() to configure an output.");
  retVal = mappingHandler.configureOutputValue(OutputMappingValueID::CONTROLWORD, "XYZ");
  if ( retVal != AxisProfileResult::STS_OK )
  {
    error(L"\nFunction call did fail with return code 0x%08X", retVal);
    return E_FAIL;
  }

  log(L"\nCall getConfiguredOutputURI() to check the output configuration.");
  receivedUri = mappingHandler.getConfiguredOutputURI(OutputMappingValueID::CONTROLWORD);
  if ( !receivedUri )
  {
    error(L"\nExpected a null-pointer but received data!", retVal);
    return E_FAIL;
  }
  std::string convUriOut(receivedUri);
  if ( convUriOut.compare("XYZ") != 0 )
  {
    error(L"\nThe written and the read configuration is unequal!", retVal);
    return E_FAIL;
  }

  log(L"\nCall configureOutputValue() again to overwrite the existing configuration.");
  retVal = mappingHandler.configureOutputValue(OutputMappingValueID::CONTROLWORD, "TestControlword");
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall configureInputType() to configure the mapping value type.");
  retVal = mappingHandler.configureInputCfgInfo(InputMappingValueID::POSITION, {MappingGroup::BASE,true});
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall configureOutputCfgInfo() to configure the mapping value type.");
  retVal = mappingHandler.configureOutputCfgInfo(OutputMappingValueID::POSITION, {MappingGroup::BASE, true});
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall configureOutputCfgInfo() to configure the mapping value type.");
  retVal = mappingHandler.configureOutputCfgInfo(OutputMappingValueID::POSITION, {MappingGroup::UNKNOWN,true});
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n\nCreate a simple input and output memory map for further testing.");
  test::MemoryMapHelper memMapHelper;
  memMapHelper.addVariable("TestStatusword", 0, 16, comm::datalayer::TYPE_PLC_WORD);
  auto memMapVariantIn = memMapHelper.createMemoryMap(0);
  auto memMapIn = comm::datalayer::GetMemoryMap(memMapVariantIn.getData());
  memMapHelper.clearMemMapData();
  memMapHelper.addVariable("TestControlword", 0, 16, comm::datalayer::TYPE_PLC_WORD);
  auto memMapVariantOut = memMapHelper.createMemoryMap(0);
  auto memMapOut = comm::datalayer::GetMemoryMap(memMapVariantOut.getData());

  log(L"\n\nCall initRealTimeMappingInput() for the configured input variable.");
  retVal = mappingHandler.initRealTimeMappingInput(memMapIn);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall getInputRealtimeInfo() to check if there is exactly one entry.");
  auto rtInputMapping = mappingHandler.getInputRealtimeInfo();
  if ( rtInputMapping.size() != 0x01 )
  {
    error(L"\nUnexpected input mapping configuration! Expected entries: 1 Actual: %d", rtInputMapping.size());
    return E_FAIL;
  }

  log(L"\nCall initRealTimeMappingOutput() for the configured output variable.");
  retVal = mappingHandler.initRealTimeMappingOutput(memMapOut);
  LOG_TEST_RESULT(retVal, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nCall getOutputRealtimeInfo() to check if there is exactly one entry.");
  auto& rtOutputMapping = mappingHandler.getOutputRealtimeInfo();
  if ( rtOutputMapping.size() != 0x01 )
  {
    error(L"\nUnexpected output mapping configuration! Expected entries: 1 Actual: %d", rtOutputMapping.size());
    return E_FAIL;
  }

  log(L"\nTest clearOutputConfiguration.");
  mappingHandler.clearOutputConfiguration();

  log(L"\nTest clearInputConfiguration.");
  mappingHandler.clearInputConfiguration();

  // End of Test
  log(L"\n");

  return result;
}

HRESULT TestMappingHandler::testVectorConfig()
{
  HRESULT result = S_OK;
  test::MockAxisProfileTrace m_profileTraceMockup{};
  AcknowledgeTelegram atData;
  MasterDataTelegram  mdtData;
  ProfileBasicInfo profileBasicInfo;
  test::MockAxisProfileTrace profileTraceMockup {};
  test::MockFieldbusAdapterSoE fieldbusAdapterMockUp {};
  profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapterMockUp;
  profileBasicInfo.m_axisProfileType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
    
  AxisProfileMappingHandler mappingHandler(profileBasicInfo);
  auto pProfileCategory = std::make_unique<DriveCategoryAdapterSoE>(profileBasicInfo, &atData, &mdtData);
  auto sharingHandler = AxisProfileSharingHandler(&profileBasicInfo.m_profileTrace);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandler);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, pProfileCategory.get());

  log(L"\n\n<b>----- Test class methods with vector -----</b>");

  log(L"\nTest to configure input mapping.");
  std::vector<InputMappingEntry> inputConfig;
  InputMappingEntry invalidEntryIDIn{ InputMappingValueID::UNKNOWN, "ABC", MappingGroup::UNKNOWN, false };
  inputConfig.push_back(invalidEntryIDIn);
  AxisProfileResult retRes = mappingHandler.configureInputValues(inputConfig);
  LOG_TEST_RESULT(retRes, L"Got an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);

  InputMappingEntry validEntryIn{ InputMappingValueID::POSITION, "ABC", MappingGroup::BASE, true };
  inputConfig.clear();
  inputConfig.push_back(validEntryIn);
  retRes = mappingHandler.configureInputValues(inputConfig);
  LOG_TEST_RESULT(retRes, L"Got an unexpected result", AxisProfileResult::STS_OK);


  log(L"\nTest to read input mapping.");
  std::vector<InputMappingEntry> actInputConfig;
  actInputConfig = mappingHandler.getConfiguredInputs();
  if ( ( actInputConfig[ 0 ].m_id != inputConfig[ 0 ].m_id ) || ( actInputConfig[ 0 ].m_uri.compare(inputConfig[ 0 ].m_uri) != 0 ) )
  {
    error(L"Read configuration of input mapping is not as expected.");
    result = E_FAIL;
  }

  log(L"\nTest to configure output mapping.");
  std::vector<OutputMappingEntry> outputConfig;
  OutputMappingEntry invalidEntryIDOut{ OutputMappingValueID::UNKNOWN, "ABC", MappingGroup::UNKNOWN, false};
  outputConfig.push_back(invalidEntryIDOut);
  retRes = mappingHandler.configureOutputValues(outputConfig);
  LOG_TEST_RESULT(retRes, L"Got an unexpected result", AxisProfileResult::ERR_INVALID_ARGUMENT);


  OutputMappingEntry validEntryOut{ OutputMappingValueID::POSITION, "ABC", MappingGroup::BASE, true };
  outputConfig.clear();
  outputConfig.push_back(validEntryOut);
  retRes = mappingHandler.configureOutputValues(outputConfig);
  LOG_TEST_RESULT(retRes, L"Got an unexpected result", AxisProfileResult::STS_OK);

  log(L"\nTest to read output mapping.");
  std::vector<OutputMappingEntry> actOutputConfig;
  actOutputConfig = mappingHandler.getConfiguredOutputs();
  if ( ( actOutputConfig[ 0 ].m_id != outputConfig[ 0 ].m_id ) || ( actOutputConfig[ 0 ].m_uri.compare(outputConfig[ 0 ].m_uri) != 0 ) )
  {
    error(L"Read configuration of output mapping is not as expected.");
    result = E_FAIL;
  }

  log(L"\nTest to read available input mapping.");
  actInputConfig = mappingHandler.getAvailableInputs();
  std::vector<InputMappingValueID> vecInput = category_drive::AVAILABLE_INPUT_MAPPING;
  for (size_t i = 0; i< actInputConfig.size(); i++)
  {
    if (actInputConfig[i].m_id != vecInput[i])
    {
      error(L"Read configuration of input mapping is not as expected.");
      result = E_FAIL;
    }
  }

  log(L"\nTest to read available output mapping.");
  actOutputConfig = mappingHandler.getAvailableOutputs();
  std::vector<OutputMappingValueID> vecOutput = category_drive::AVAILABLE_OUTPUT_MAPPING;
  for (size_t i = 0; i < actOutputConfig.size(); i++)
  {
    if (actOutputConfig[i].m_id != vecOutput[i])
    {
      error(L"Read configuration of output mapping is not as expected.");
      result = E_FAIL;
    }
  }

  log(L"\nTest to read available input mapping.");
  std::multimap<std::string, ProfileMappingCheckInfo> mapInfo;
    
  AxisProfileMappingHandler encoderMappingHandler(profileBasicInfo);
  auto scaling = std::make_unique<sercos::ScalingStrategySoEDrive>(sercos::ScalingStrategySoEDrive {profileBasicInfo});
  auto encoderCategoryObj = std::make_unique<CategoryAdapterEncoder>(profileBasicInfo, &atData, &mdtData);
  auto encoderSharingHandler = AxisProfileSharingHandler(&profileBasicInfo.m_profileTrace);
  encoderSharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &encoderMappingHandler);
  encoderSharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, encoderCategoryObj.get());
  encoderSharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, scaling.get());
  encoderMappingHandler.checkInputMappingItems(nullptr, mapInfo);

  log(L"\nTest to read available output mapping.");
  encoderMappingHandler.checkOutputMappingItems(nullptr, mapInfo);

  return result;
}

HRESULT TestMappingHandler::testStringConvertMethods()
{
  HRESULT hResult = S_OK;
  log(L"\n\n<b>----- Test convert methods for value id -----</b>");

  log(L"\nTest the stringToInputMappingValueID method");
  InputMappingValueID expIDInput = InputMappingValueID::UNKNOWN;
  InputMappingValueID retIDInput = stringToInputMappingValueID(nullptr);
  LOG_TEST_RESULT(retIDInput, L"Got an unexpected result", expIDInput);

  retIDInput = stringToInputMappingValueID("HELLO");
  LOG_TEST_RESULT(retIDInput, L"Got an unexpected result", expIDInput);

  expIDInput = InputMappingValueID::POSITION;
  retIDInput = stringToInputMappingValueID(AprInputMappingValueIDStrings.POSITION.c_str());
  LOG_TEST_RESULT(retIDInput, L"Got an unexpected result", expIDInput);

  log(L"\nTest the inputMappingValueIdToString method");
  std::string expStringIn{ AprInputMappingValueIDStrings.POSITION };
  std::string retStringIn = inputMappingValueIdToString(InputMappingValueID::POSITION);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.STATUSWORD;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::STATUSWORD);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.DIAGMSG;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::DIAGMSG);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.VELOCITY;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::VELOCITY);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.SWITCH_REF;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::SWITCH_REF);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.SWITCH_POS_LIM;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::SWITCH_POS_LIM);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.SWITCH_NEG_LIM;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::SWITCH_NEG_LIM);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.IO_IN_REFERENCE;
  retStringIn = inputMappingValueIdToString(InputMappingValueID::IO_IN_REFERENCE);
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  expStringIn = AprInputMappingValueIDStrings.UNKNOWN;
  retStringIn = inputMappingValueIdToString(static_cast<InputMappingValueID>( 99 ));
  LOG_TEST_RESULT_STRING(retStringIn, L"Got an unexpected result", expStringIn);

  log(L"\nTest the inputMappingValueIdToDescription method");
  std::string expDescriptionIn{ AprInputMappingValueIDDescriptions.POSITION };
  std::string retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::POSITION);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.STATUSWORD;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::STATUSWORD);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.DIAGMSG;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::DIAGMSG);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.VELOCITY;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::VELOCITY);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.SWITCH_REF;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::SWITCH_REF);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.SWITCH_POS_LIM;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::SWITCH_POS_LIM);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.SWITCH_NEG_LIM;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::SWITCH_NEG_LIM);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.UNKNOWN;
  retDescriptionIn = inputMappingValueIdToDescription(static_cast<InputMappingValueID>( 99 ));
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.TORQUE_FORCE;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::TORQUE_FORCE);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.TORQUE_LIMIT_STATUS;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::TORQUE_LIMIT_STATUS);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_POS_EDG;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_POSEDG);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_NEG_EDG;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_NEGEDG);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_ACTIVE;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_ACTIVE);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_POS_LAT;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_POSLAT);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_NEG_LAT;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_NEGLAT);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_POS_DIF;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_POSDIF);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE1_MAK_FAL;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE1_MAKFAL);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_POS_EDG;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_POSEDG);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_NEG_EDG;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_NEGEDG);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_ACTIVE;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_ACTIVE);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_POS_LAT;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_POSLAT);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_NEG_LAT;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_NEGLAT);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_POS_DIF;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_POSDIF);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.PROBE2_MAK_FAL;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::PROBE2_MAKFAL);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.OP_MODE_DISPLAY;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::OP_MODE_DISPLAY);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  expDescriptionIn = AprInputMappingValueIDDescriptions.IO_IN_REFERENCE;
  retDescriptionIn = inputMappingValueIdToDescription(InputMappingValueID::IO_IN_REFERENCE);
  LOG_TEST_RESULT_STRING(retDescriptionIn, L"Got an unexpected result", expDescriptionIn);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Output


  log(L"\nTest the stringToOutputMappingValueID method");
  OutputMappingValueID expIDOutput = OutputMappingValueID::UNKNOWN;
  OutputMappingValueID retIDOutput = stringToOutputMappingValueID(nullptr);
  LOG_TEST_RESULT(retIDOutput, L"Got an unexpected result", expIDOutput);

  retIDOutput = stringToOutputMappingValueID("HELLO");
  LOG_TEST_RESULT(retIDOutput, L"Got an unexpected result", expIDOutput);

  expIDOutput = OutputMappingValueID::POSITION;
  retIDOutput = stringToOutputMappingValueID(AprInputMappingValueIDStrings.POSITION.c_str());
  LOG_TEST_RESULT(retIDOutput, L"Got an unexpected result", expIDOutput);

  log(L"\nTest the outputMappingValueIdToString method");
  std::string expStringOut{ AprOutputMappingValueIDStrings.POSITION };
  std::string retStringOut = outputMappingValueIdToString(OutputMappingValueID::POSITION);
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  expStringOut = AprOutputMappingValueIDStrings.CONTROLWORD;
  retStringOut = outputMappingValueIdToString(OutputMappingValueID::CONTROLWORD);
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  expStringOut = AprOutputMappingValueIDStrings.VELOCITY;
  retStringOut = outputMappingValueIdToString(OutputMappingValueID::VELOCITY);
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  expStringOut = AprOutputMappingValueIDStrings.IO_SET_REFERENCE;
  retStringOut = outputMappingValueIdToString(OutputMappingValueID::IO_SET_REFERENCE);
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  expStringOut = AprOutputMappingValueIDStrings.IO_REFERENCE_OFFSET;
  retStringOut = outputMappingValueIdToString(OutputMappingValueID::IO_REFERENCE_OFFSET);
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  expStringOut = AprInputMappingValueIDStrings.UNKNOWN;
  retStringOut = outputMappingValueIdToString(static_cast<OutputMappingValueID>( 99 ));
  LOG_TEST_RESULT_STRING(retStringOut, L"Got an unexpected result", expStringOut);

  log(L"\nTest the outputMappingValueIdToDescription method");
  std::string expDescriptionOut{ AprOutputMappingValueIDDescriptions.POSITION };
  std::string retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::POSITION);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.CONTROLWORD;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::CONTROLWORD);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.VELOCITY;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::VELOCITY);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.TORQUE_FORCE;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::TORQUE_FORCE);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.TORQUE_VEL_LIMIT_HIGH;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.TORQUE_VEL_LIMIT_LOW;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::TORQUE_VEL_LIMIT_LOW);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.IO_SET_REFERENCE;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::IO_SET_REFERENCE);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.IO_REFERENCE_OFFSET;
  retDescriptionOut = outputMappingValueIdToDescription(OutputMappingValueID::IO_REFERENCE_OFFSET);
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  expDescriptionOut = AprOutputMappingValueIDDescriptions.UNKNOWN;
  retDescriptionOut = outputMappingValueIdToDescription(static_cast<OutputMappingValueID>( 99 ));
  LOG_TEST_RESULT_STRING(retDescriptionOut, L"Got an unexpected result", expDescriptionOut);

  return hResult;
}
