#include "helper/datalayer_helper.h"
#include "helper/datalayer_memory_map_helper.h"
#include "helper/result_helper.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_defs.h"
#include "mockup/external_bundles/mock_datalayer_factory.h"
#include "mockup/mock_axisprofile.h"
#include "test_fieldbus_master.h"

REGISTER_TEST(TestFieldbusMaster);
REGISTER_TEST_TO_GROUP(TestFieldbusMaster, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestFieldbusMaster, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestFieldbusMaster, Fieldbuses);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test
using namespace test;
using namespace comm::axisprofile;

const std::vector<MasterRtDataMap> TestFieldbusMaster::MOCK_RT_DATA_MAP
{
  {MasterRtDataID::TASK_START_TIME,   "dummyA", { 0, 64}},
  {MasterRtDataID::TASK_CYCLE_TIME,   "dummyB", {64, 32}},
  {MasterRtDataID::OUTPUT_SHIFT_TIME, "dummyC", {96, 32}},
  {MasterRtDataID::SEND_OFFSET,       "dummyD", {128, 32}},
};

HRESULT TestFieldbusMaster::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestFieldbusMaster");
  log(L"\n----------------------------------------------------------------\n");

  res |= testMiscellaneous();
  res |= testRtDataAccess();

  return res;
}

HRESULT TestFieldbusMaster::testMiscellaneous()
{
  HRESULT res = S_OK;
  MockDatalayerFactory mockDlFactory;
  m_fieldbusMaster.reset(new FieldbusMaster(m_masterName.c_str(), &mockDlFactory));

  log(L"\nTest getMasterName...");
  auto masterName = m_fieldbusMaster->getMasterName();
  LOG_TEST_RESULT_STRING(masterName, L"\nUnexpected master name", m_masterName);

  log(L"\nTest addLinkedProfile...");
  MockAxisProfile mockSoEProfile("testProfile1", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);
  MockAxisProfile mockCoEProfile("testProfile2", AxisProfileType::CANOPEN_OVER_ETHERCAT);
  m_fieldbusMaster->addLinkedProfile(&mockSoEProfile);
  m_fieldbusMaster->addLinkedProfile(&mockCoEProfile);

  log(L"\nTest getLinkedProfiles...");
  auto& profiles = m_fieldbusMaster->getLinkedProfiles();
  auto profile = profiles.find(&mockSoEProfile);
  RETURN_TEST_RESULT_ANY(profile != profiles.end(), L"\nCan't find inserted SoE profile");
  profile = profiles.find(&mockCoEProfile);
  RETURN_TEST_RESULT_ANY(profile != profiles.end(), L"\nCan't find inserted SoE profile");

  log(L"\nTest fieldbusCycleTime...");
  auto fieldbusCycleTime = m_fieldbusMaster->getFieldbusCycleTime();
  LOG_TEST_RESULT_FLT(fieldbusCycleTime, L"\nUnexpected fieldbus cycle time", 0.0, 1E-9);

  log(L"\nTest getActDcBaseTime...");
  auto actDcBaseTime = m_fieldbusMaster->getActDcBaseTime();
  LOG_TEST_RESULT_FLT(actDcBaseTime, L"\nUnexpected DC base time", 0.0, 1E-9);

  m_fieldbusMaster.reset(nullptr);

  log(L"\nTest getMasterRtDataIDString...");
  auto idStr = getMasterRtDataIDString(MasterRtDataID::TASK_START_TIME);
  LOG_TEST_RESULT_STRING(idStr, L"\nUnexpected ID string", "Task.startTime");
  idStr = getMasterRtDataIDString(MasterRtDataID::TASK_CYCLE_TIME);
  LOG_TEST_RESULT_STRING(idStr, L"\nUnexpected ID string", "Task.cycleTime");
  idStr = getMasterRtDataIDString(MasterRtDataID::OUTPUT_SHIFT_TIME);
  LOG_TEST_RESULT_STRING(idStr, L"\nUnexpected ID string", "OutputShiftTime");
  idStr = getMasterRtDataIDString(MasterRtDataID::SEND_OFFSET);
  LOG_TEST_RESULT_STRING(idStr, L"\nUnexpected ID string", "SendOffset");

  idStr = getMasterRtDataIDString(static_cast<MasterRtDataID>(-1));
  LOG_TEST_RESULT_STRING(idStr, L"\nUnexpected ID string", "UnknownType");

  log(L"\n");
  return res;
}

HRESULT TestFieldbusMaster::testRtDataAccess()
{
  MockDatalayerFactory mockDlFactory;
  std::shared_ptr<test::MockMemoryUser> mockNullMemUser = nullptr;
  std::shared_ptr<test::MockMemoryUser> mockMemUser (new test::MockMemoryUser(sizeof(MasterRtDataRawValue)));
  MockAxisProfile mockSoEProfile("testProfile", AxisProfileType::SERVODRIVE_OVER_ETHERCAT);

  log(L"\nTest initRtData with empty DL factory...");
  m_fieldbusMaster.reset(new FieldbusMaster(m_masterName.c_str(), nullptr));
  auto result = m_fieldbusMaster->initRtData(false);
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::ERR_DATALAYER);

  m_fieldbusMaster.reset(new FieldbusMaster(m_masterName.c_str(), &mockDlFactory));
  log(L"\nTest initRtData with no linked profile...");
  result = m_fieldbusMaster->initRtData(false);
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_OK);

  m_fieldbusMaster->addLinkedProfile(&mockSoEProfile);
  log(L"\nTest initRtData when open memory failed...");
  mockDlFactory.setReturnValue(comm::datalayer::DlResult::DL_RT_INVALIDOBJECT);
  result = m_fieldbusMaster->initRtData(false);
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_DATALAYER_RT_NOT_OPEN);

  log(L"\nTest initRtData when get memory map failed...");
  mockDlFactory.setReturnValue(comm::datalayer::DlResult::DL_OK);
  mockDlFactory.setMemoryUser(mockMemUser);
  result = m_fieldbusMaster->initRtData(false);
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::ERR_DLRT);

  log(L"\nTest initRtData when every is fine...");
  MemoryMapHelper memMapHelper;
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_TASK_START_TIME,    MOCK_RT_DATA_MAP[0].m_memLocation.m_bitOffset, MOCK_RT_DATA_MAP[0].m_memLocation.m_bitSize, comm::datalayer::TYPE_PLC_ULINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_TASK_CYCLE_TIME,    MOCK_RT_DATA_MAP[1].m_memLocation.m_bitOffset, MOCK_RT_DATA_MAP[1].m_memLocation.m_bitSize, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_OUTPUT_SHIFT_TIME,  MOCK_RT_DATA_MAP[2].m_memLocation.m_bitOffset, MOCK_RT_DATA_MAP[2].m_memLocation.m_bitSize, comm::datalayer::TYPE_PLC_UDINT);
  memMapHelper.addVariable(MOCK_ECAT_MASTERBUFFER_SEND_OFFSET,        MOCK_RT_DATA_MAP[3].m_memLocation.m_bitOffset, MOCK_RT_DATA_MAP[3].m_memLocation.m_bitSize, comm::datalayer::TYPE_PLC_UDINT);
  auto& mockMap = memMapHelper.createMemoryMap(0);
  mockMemUser->setMemoryMap(&mockMap);
  memMapHelper.clearMemMapData();

  // reset memory
  std::uint8_t* destData;
  std::uint32_t revision {0};
  mockMemUser->beginAccess(destData, revision);
  memset(destData, 0, sizeof(MasterRtDataRawValue));

  result = m_fieldbusMaster->initRtData(false);
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nTest ProcessRtData when memory user is empty...");
  mockDlFactory.setMemoryUser(mockNullMemUser);
  m_fieldbusMaster->initRtData(false);
  result = m_fieldbusMaster->ProcessRtData();
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nTest ProcessRtData when beginAccess failed...");
  mockDlFactory.setMemoryUser(mockMemUser);
  m_fieldbusMaster->initRtData(false);
  mockMemUser->setReturnValue(MockMemoryUser::BEGIN_ACCESS, comm::datalayer::DlResult::DL_RT_NOVALIDDATA);
  result = m_fieldbusMaster->ProcessRtData();
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::ERR_DATALAYER);

  log(L"\nTest ProcessRtData when every works fine...");
  mockMemUser->setReturnValue(MockMemoryUser::BEGIN_ACCESS, comm::datalayer::DlResult::DL_OK);
  mockMemUser->setReturnValue(MockMemoryUser::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  result = m_fieldbusMaster->ProcessRtData();
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nTest ProcessRtData when endAccess failed...");
  log(L"\n  clear trace sent flag...");
  mockMemUser->setReturnValue(MockMemoryUser::BEGIN_ACCESS, comm::datalayer::DlResult::DL_OK);
  mockMemUser->setReturnValue(MockMemoryUser::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  m_fieldbusMaster->ProcessRtData();
  log(L"\n  endAccess failed...");
  mockMemUser->setReturnValue(MockMemoryUser::BEGIN_ACCESS, comm::datalayer::DlResult::DL_OK);
  mockMemUser->setReturnValue(MockMemoryUser::END_ACCESS, comm::datalayer::DlResult::DL_RT_NOVALIDDATA);
  result = m_fieldbusMaster->ProcessRtData();
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::ERR_DATALAYER);

  log(L"\nTest ProcessRtData when every works fine...");
  mockMemUser->setReturnValue(MockMemoryUser::BEGIN_ACCESS, comm::datalayer::DlResult::DL_OK);
  mockMemUser->setReturnValue(MockMemoryUser::END_ACCESS, comm::datalayer::DlResult::DL_OK);
  result = m_fieldbusMaster->ProcessRtData();
  RETURN_TEST_RESULT(result, L"\nGet unexpected return value", AxisProfileResult::STS_OK);

  log(L"\nTest RT data reading...");
  MasterRtDataRawValue mockRawValue {80000000, 2000000, 600000, 100000};
  double expCycleTime = NANOSECOND_2_SECOND(mockRawValue.m_taskCycleTimeNs);
  double expDcBaseTime = NANOSECOND_2_SECOND(mockRawValue.m_taskStartTimeNs + mockRawValue.m_outputShiftTimeNs);

  mockMemUser->beginAccess(destData, revision);
  DatalayerHelper::copyBits(destData + MOCK_RT_DATA_MAP[0].m_memLocation.m_bitOffset / 8, 0, reinterpret_cast<std::uint8_t*>(&(mockRawValue.m_taskStartTimeNs)),   0, MOCK_RT_DATA_MAP[0].m_memLocation.m_bitSize);
  DatalayerHelper::copyBits(destData + MOCK_RT_DATA_MAP[1].m_memLocation.m_bitOffset / 8, 0, reinterpret_cast<std::uint8_t*>(&(mockRawValue.m_taskCycleTimeNs)),   0, MOCK_RT_DATA_MAP[1].m_memLocation.m_bitSize);
  DatalayerHelper::copyBits(destData + MOCK_RT_DATA_MAP[2].m_memLocation.m_bitOffset / 8, 0, reinterpret_cast<std::uint8_t*>(&(mockRawValue.m_outputShiftTimeNs)), 0, MOCK_RT_DATA_MAP[2].m_memLocation.m_bitSize);
  DatalayerHelper::copyBits(destData + MOCK_RT_DATA_MAP[3].m_memLocation.m_bitOffset / 8, 0, reinterpret_cast<std::uint8_t*>(&(mockRawValue.m_sendOffsetNs)),      0, MOCK_RT_DATA_MAP[3].m_memLocation.m_bitSize);
  m_fieldbusMaster->ProcessRtData();
  log(L"\nTest getActDcBaseTime...");
  auto actDCTime = m_fieldbusMaster->getActDcBaseTime();
  LOG_TEST_RESULT_FLT(actDCTime, L"\nUnexpected DC base time", expDcBaseTime, 1E-9);

  log(L"\nTest getFieldbusCycleTime...");
  m_fieldbusMaster->ProcessRtData();
  auto cycleTime = m_fieldbusMaster->getFieldbusCycleTime();
  LOG_TEST_RESULT_FLT(cycleTime, L"\nUnexpected fieldbus cycle time", expCycleTime, 1E-9);

  mockMemUser.reset();
  m_fieldbusMaster.reset(nullptr);
  return S_OK;
}
