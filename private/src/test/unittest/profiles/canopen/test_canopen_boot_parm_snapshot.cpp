#include "stdafx.h"

#include "helper/result_helper.h"

#include "profiles/canopen/canopen_boot_param_snapshot.h"

#include "profiles/canopen/canopen_boot_params.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "test_canopen_boot_parm_snapshot.h"

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "comm/ethercat/master/fbs/sdo_generated.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;
using namespace test;

REGISTER_TEST(TestCanopenBootParmSnapshot);
REGISTER_TEST_TO_GROUP(TestCanopenBootParmSnapshot, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestCanopenBootParmSnapshot, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestCanopenBootParmSnapshot, Profiles_Canopen);

namespace
{
  using ObjKey = std::pair<std::uint16_t, std::uint8_t>;

  bool getParamU16(const BootParamMap& parameterMap, const std::string& name, std::uint16_t& value)
  {
    auto it = parameterMap.find(name);
    if (it == parameterMap.end() || it->second.type != BootParamValueType::UNSIGNED16)
    {
      return false;
    }

    value = it->second.value.valueUint16;
    return true;
  }

  bool getParamU32(const BootParamMap& parameterMap, const std::string& name, std::uint32_t& value)
  {
    auto it = parameterMap.find(name);
    if (it == parameterMap.end() || it->second.type != BootParamValueType::UNSIGNED32)
    {
      return false;
    }

    value = it->second.value.valueUint32;
    return true;
  }

  bool getParamByteArray(const BootParamMap& parameterMap, const std::string& name, std::vector<std::uint8_t>& value)
  {
    auto it = parameterMap.find(name);
    if (it == parameterMap.end() || it->second.type != BootParamValueType::BYTEARRAY)
    {
      return false;
    }

    value = it->second.value.valueByteArray;
    return true;
  }

  void setParamU16(BootParamMap& parameterMap, const std::string& name, std::uint16_t value)
  {
    auto& item = parameterMap[name];
    item.type = BootParamValueType::UNSIGNED16;
    item.value.valueUint16 = value;
  }

  void setParamUnknownType(BootParamMap& parameterMap, const std::string& name)
  {
    auto& item = parameterMap[name];
    item.type = static_cast<BootParamValueType>(0xFF);
  }

  bool getParamU16(const BootParamSnapshot& snapshot, const std::string& name, std::uint16_t& value)
  {
    return getParamU16(snapshot.paramMap, name, value);
  }

  bool getParamU32(const BootParamSnapshot& snapshot, const std::string& name, std::uint32_t& value)
  {
    return getParamU32(snapshot.paramMap, name, value);
  }

  bool getParamByteArray(const BootParamSnapshot& snapshot, const std::string& name, std::vector<std::uint8_t>& value)
  {
    return getParamByteArray(snapshot.paramMap, name, value);
  }

  void setParamU16(BootParamSnapshot& snapshot, const std::string& name, std::uint16_t value)
  {
    setParamU16(snapshot.paramMap, name, value);
  }

  void setParamUnknownType(BootParamSnapshot& snapshot, const std::string& name)
  {
    setParamUnknownType(snapshot.paramMap, name);
  }

  class MockSnapshotParamRequestNodeCoE : public test::MockEthercatMasterSDORequestNodeCoE
  {
  public:
    template <typename TValue>
    void setValue(std::uint16_t index, std::uint8_t subIndex, TValue value)
    {
      std::vector<std::uint8_t> raw(sizeof(TValue));
      std::memcpy(raw.data(), &value, sizeof(TValue));
      m_responseData[{index, subIndex}] = raw;
    }

    void setByteArray(std::uint16_t index, std::uint8_t subIndex, const std::vector<std::uint8_t>& value)
    {
      m_responseData[{index, subIndex}] = value;
    }

    void setFailure(std::uint16_t index, std::uint8_t subIndex, comm::datalayer::DlResult result)
    {
      m_failures[{index, subIndex}] = result;
    }

    void clearFailure(std::uint16_t index, std::uint8_t subIndex)
    {
      m_failures.erase({index, subIndex});
    }

    void onRead(const std::string& /*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) ||
          comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifySDOBuffer)))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }

      auto sdoData = comm::ethercat::master::fbs::GetSDO(data->getData());
      if (!flatbuffers::IsFieldPresent(sdoData, comm::ethercat::master::fbs::SDO::VT_REQUEST))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }

      const auto index = sdoData->request()->objectIndex();
      const auto subIndex = sdoData->request()->subIndex();
      const ObjKey key {index, subIndex};

      const auto failureIt = m_failures.find(key);
      if (failureIt != m_failures.end())
      {
        callback(failureIt->second, nullptr);
        return;
      }

      const auto dataIt = m_responseData.find(key);
      comm::datalayer::Variant returnValue {};
      if (dataIt != m_responseData.end())
      {
        flatbuffers::FlatBufferBuilder builder;
        builder.ForceDefaults(true);

        const auto dataOffset = builder.CreateVector(dataIt->second.data(), dataIt->second.size());
        const auto responseOffset = comm::ethercat::master::fbs::CreateSDOResponse(builder, dataOffset);

        comm::ethercat::master::fbs::SDOBuilder sdoBuilder(builder);
        sdoBuilder.add_response(responseOffset);
        builder.Finish(sdoBuilder.Finish());
        returnValue.copyFlatbuffers(builder);
      }
      else
      {
        m_fbsHelper.generateSDOResponseFlatbuffer(&returnValue, index, subIndex);
      }

      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  private:
    std::map<ObjKey, std::vector<std::uint8_t>> m_responseData;
    std::map<ObjKey, comm::datalayer::DlResult> m_failures;
  };

  ProfileBasicInfo createProfileBasicInfo(test::MockFieldbusAdapterCoE& adapter)
  {
    ProfileBasicInfo profileBasicInfo {};
    profileBasicInfo.m_deviceAddress = 1;
    profileBasicInfo.m_driveNumber = 0;
    profileBasicInfo.m_fieldbusAdapter = &adapter;
    profileBasicInfo.m_profileName = "testProfile";
    return profileBasicInfo;
  }
}

HRESULT TestCanopenBootParmSnapshot::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Canopen boot parameter snapshot class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testGetParameterReadFromDriveAndPersist();
  hresult |= testGetParameterReadFromSnapshotCache();
  hresult |= testGetParameterForceReadFromDrive();
  hresult |= testGetParameterValidationAndErrorPaths();
  hresult |= testGetParameterValueFromSnapshotNullChecks();
  hresult |= testGenerateReqDataNullForInvalidBootParamId();
  return hresult;
}

HRESULT TestCanopenBootParmSnapshot::testGetParameterReadFromDriveAndPersist()
{
  log(L"\n<b>Test reading Canopen parameters from drive and persisting snapshot.</b>");

  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeCoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  requestNode.setValue(ENCODER_1_PROPERTY.index, ENCODER_1_PROPERTY.subIndex, static_cast<std::uint16_t>(0x0011));
  requestNode.setValue(SCALING_SI_UNIT_POSITION.index, SCALING_SI_UNIT_POSITION.subIndex, static_cast<std::uint32_t>(0x12345678));

  const std::uint8_t SIGNAL_STATUS_WORD_LIST_SIZE = 16;
  std::vector<std::uint8_t> signalStatusAssignList {};
  signalStatusAssignList.reserve(SIGNAL_STATUS_WORD_LIST_SIZE * sizeof(std::uint16_t));
  for (std::size_t i = 0; i < SIGNAL_STATUS_WORD_LIST_SIZE * sizeof(std::uint16_t); ++i)
  {
    signalStatusAssignList.push_back(static_cast<std::uint8_t>(i + 1));
  }
  requestNode.setByteArray(SIGNAL_STATUS_WORD_ASSIGN_LIST.index, SIGNAL_STATUS_WORD_ASSIGN_LIST.subIndex, signalStatusAssignList);

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  CanopenBootParamSnapshot testClass {profileBasicInfo};

  std::size_t retLen {};
  AxisProfileResult result {AxisProfileResult::ERR_NOK};
  std::uint16_t encoder1Property {};
  result = testClass.getParameter(ENCODER_1_PROPERTY, reinterpret_cast<std::uint8_t*>(&encoder1Property), sizeof(encoder1Property), retLen);
  LOG_TEST_RESULT_ANY(retLen == sizeof(encoder1Property), L"\nUnexpected return length.");
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(encoder1Property, L"\nUnexpected encoder property.", 0x0011);

  std::uint32_t positionUnit {};
  result = testClass.getParameter(SCALING_SI_UNIT_POSITION, reinterpret_cast<std::uint8_t*>(&positionUnit), sizeof(positionUnit), retLen);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(positionUnit, L"\nUnexpected position unit.", 0x12345678);

  std::uint8_t signalStatusAssignRaw[SIGNAL_STATUS_WORD_LIST_SIZE * sizeof(std::uint16_t)] {};
  result = testClass.getParameter(SIGNAL_STATUS_WORD_ASSIGN_LIST, signalStatusAssignRaw, sizeof(signalStatusAssignRaw), retLen);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for byte array parameter.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT_ANY(retLen == signalStatusAssignList.size(), L"\nUnexpected return length.");
  LOG_TEST_RESULT_ANY(std::memcmp(signalStatusAssignRaw, signalStatusAssignList.data(), signalStatusAssignList.size()) == 0,
                      L"\nUnexpected signal status assign list value.");

  const auto& snapshot = testClass.getBootParamSnapshot();
  LOG_TEST_RESULT_ANY(snapshot.protocol == BootSnapshotProtocol::CANOPEN, L"\nSnapshot protocol should be Canopen.");
  LOG_TEST_RESULT_ANY(snapshot.profileName == "testProfile", L"\nSnapshot profile name should be initialized by constructor.");

  std::uint16_t persistedEncoder1Property {};
  LOG_TEST_RESULT_ANY(getParamU16(snapshot, objDirToString(ENCODER_1_PROPERTY), persistedEncoder1Property), L"\nUnexpected persisted encoder property.");
  LOG_TEST_RESULT(persistedEncoder1Property, L"\nUnexpected persisted encoder property.", 0x0011);

  std::uint32_t persistedPositionUnit {};
  LOG_TEST_RESULT_ANY(getParamU32(snapshot, objDirToString(SCALING_SI_UNIT_POSITION), persistedPositionUnit), L"\nUnexpected persisted position unit.");
  LOG_TEST_RESULT(persistedPositionUnit, L"\nUnexpected persisted position unit.", 0x12345678);

  std::vector<std::uint8_t> persistedSignalStatusAssignList {};
  LOG_TEST_RESULT_ANY(getParamByteArray(snapshot, objDirToString(SIGNAL_STATUS_WORD_ASSIGN_LIST), persistedSignalStatusAssignList),
                      L"\nUnexpected persisted signal status assign list.");
  LOG_TEST_RESULT_ANY(persistedSignalStatusAssignList == signalStatusAssignList, L"\nUnexpected persisted signal status assign list value.");

  return S_OK;
}

HRESULT TestCanopenBootParmSnapshot::testGetParameterReadFromSnapshotCache()
{
  log(L"\n<b>Test reading Canopen parameter from configured snapshot cache.</b>");

  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeCoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const ObjectDirEntry objEntry = ENCODER_1_PROPERTY;
  requestNode.setValue(objEntry.index, objEntry.subIndex, static_cast<std::uint16_t>(0x5678));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  CanopenBootParamSnapshot testClass {profileBasicInfo};

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::CANOPEN;
  setParamU16(snapshot, objDirToString(objEntry), 0x1234);
  testClass.setBootParamSnapshot(snapshot);

  std::size_t retLen {};
  std::uint16_t value {};
  const auto result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nCache value should be used when snapshot is valid.", 0x1234);

  return S_OK;
}

HRESULT TestCanopenBootParmSnapshot::testGetParameterForceReadFromDrive()
{
  log(L"\n<b>Test forcing drive read bypasses cache and updates snapshot.</b>");

  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeCoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const ObjectDirEntry objEntry = ENCODER_1_PROPERTY;
  requestNode.setValue(objEntry.index, objEntry.subIndex, static_cast<std::uint16_t>(0x5678));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  CanopenBootParamSnapshot testClass {profileBasicInfo};

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::CANOPEN;
  setParamU16(snapshot, objDirToString(objEntry), 0x1234);
  testClass.setBootParamSnapshot(snapshot);

  std::size_t retLen {};
  std::uint16_t value {};
  auto result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nCache value should be returned without force flag.", 0x1234);

  result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nDrive value should be returned when force flag is set.", 0x5678);

  const auto& persisted = testClass.getBootParamSnapshot();
  std::uint16_t persistedValue {};
  LOG_TEST_RESULT_ANY(getParamU16(persisted, objDirToString(objEntry), persistedValue), L"\nPersisted value should exist after force read.");
  LOG_TEST_RESULT(persistedValue, L"\nForce read should refresh persisted value.", 0x5678);

  return S_OK;
}

HRESULT TestCanopenBootParmSnapshot::testGetParameterValidationAndErrorPaths()
{
  log(L"\n<b>Test parameter validation and error paths.</b>");

  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeCoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const ObjectDirEntry objEntry = ENCODER_1_PROPERTY;
  requestNode.setValue(objEntry.index, objEntry.subIndex, static_cast<std::uint16_t>(0x1111));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  CanopenBootParamSnapshot testClass {profileBasicInfo};

  std::size_t retLen {};
  std::uint16_t value {};
  AxisProfileResult result = testClass.getParameter(ObjectDirEntry {0xFFFF, 0xFF}, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nUnknown object should be rejected.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  std::uint8_t shortBuffer[1] {};
  result = testClass.getParameter(objEntry, shortBuffer, sizeof(shortBuffer), retLen);
  RETURN_TEST_RESULT(result, L"\nToo small buffer should be rejected for drive read.", AxisProfileResult::ERR_WRONGDATASIZE);

  requestNode.setFailure(objEntry.index, objEntry.subIndex, comm::datalayer::DlResult::DL_FAILED);
  result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nDrive read failure should be reported.", AxisProfileResult::ERR_FIELDBUS);

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::CANOPEN;
  setParamU16(snapshot, objDirToString(objEntry), 0x2222);
  testClass.setBootParamSnapshot(snapshot);

  result = testClass.getParameter(objEntry, shortBuffer, sizeof(shortBuffer), retLen);
  RETURN_TEST_RESULT(result, L"\nToo small buffer should be rejected for cache read.", AxisProfileResult::ERR_WRONGDATASIZE);

  setParamUnknownType(snapshot, objDirToString(objEntry));
  testClass.setBootParamSnapshot(snapshot);
  result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nUnknown cache value type should be rejected.", AxisProfileResult::ERR_WRONGDATATYPE);

  snapshot.protocol = BootSnapshotProtocol::UNKNOWN;
  setParamU16(snapshot, objDirToString(objEntry), 0x3333);
  testClass.setBootParamSnapshot(snapshot);
  requestNode.clearFailure(objEntry.index, objEntry.subIndex);
  requestNode.setValue(objEntry.index, objEntry.subIndex, static_cast<std::uint16_t>(0x4444));

  result = testClass.getParameter(objEntry, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), retLen);
  RETURN_TEST_RESULT(result, L"\nUnknown protocol should bypass cache and read drive.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nUnknown protocol must not serve cached value.", 0x4444);

  return S_OK;
}

HRESULT TestCanopenBootParmSnapshot::testGetParameterValueFromSnapshotNullChecks()
{
  log(L"\n<b>Test getParameterValueFromSnapshot null-check paths.</b>");

  std::uint16_t value {};
  auto result = CanopenBootParamSnapshot::getParameterValueFromSnapshot(nullptr,
                                                                        "test-null-sharing-handler",
                                                                        ENCODER_1_PROPERTY,
                                                                        reinterpret_cast<std::uint8_t*>(&value),
                                                                        sizeof(value));
  RETURN_TEST_RESULT(result, L"\nNull sharing handler should return ERR_EXEC_NULL.", AxisProfileResult::ERR_EXEC_NULL);

  test::MockAxisProfileTrace profileTraceMockup {};
  AxisProfileSharingHandler sharingHandler {&profileTraceMockup};
  result = CanopenBootParamSnapshot::getParameterValueFromSnapshot(&sharingHandler,
                                                                   "test-missing-interfaces",
                                                                   ENCODER_1_PROPERTY,
                                                                   reinterpret_cast<std::uint8_t*>(&value),
                                                                   sizeof(value));
  RETURN_TEST_RESULT(result, L"\nMissing snapshot/deviceparam interfaces should return ERR_EXEC_NULL.", AxisProfileResult::ERR_EXEC_NULL);

  return S_OK;
}

HRESULT TestCanopenBootParmSnapshot::testGenerateReqDataNullForInvalidBootParamId()
{
  log(L"\n<b>Test generateReqData returns nullptr for non-Canopen BootParamId.</b>");

  test::MockFieldbusAdapterCoE mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  CanopenBootParamSnapshot testClass {profileBasicInfo};
  std::uint32_t maxLen {16};
  const auto reqData = testClass.generateReqData(BootParamId(static_cast<std::uint16_t>(0x1234)), maxLen);
  RETURN_TEST_RESULT_ANY(reqData == nullptr, L"\nExpected nullptr request data for non-Canopen BootParamId.");

  return S_OK;
}
