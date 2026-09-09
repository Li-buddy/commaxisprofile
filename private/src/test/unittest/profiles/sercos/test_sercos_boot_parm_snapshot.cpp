#include "stdafx.h"

#include "helper/result_helper.h"
#include "profiles/sercos/sercos_boot_params.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "test_sercos_boot_parm_snapshot.h"

#include <cstring>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_fieldbus_adapter_soe.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;
using namespace test;

REGISTER_TEST(TestSercosBootParmSnapshot);
REGISTER_TEST_TO_GROUP(TestSercosBootParmSnapshot, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSercosBootParmSnapshot, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSercosBootParmSnapshot, Profiles_Sercos);

namespace
{
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

  bool getParamI32(const BootParamMap& parameterMap, const std::string& name, std::int32_t& value)
  {
    auto it = parameterMap.find(name);
    if (it == parameterMap.end() || it->second.type != BootParamValueType::SIGNED32)
    {
      return false;
    }
    value = it->second.value.valueInt32;
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

  bool getParamU16(const BootParamSnapshot& parameters, const std::string& name, std::uint16_t& value)
  {
    return getParamU16(parameters.paramMap, name, value);
  }

  void setParamU16(BootParamSnapshot& parameters, const std::string& name, std::uint16_t value)
  {
    setParamU16(parameters.paramMap, name, value);
  }

  void setParamUnknownType(BootParamSnapshot& parameters, const std::string& name)
  {
    setParamUnknownType(parameters.paramMap, name);
  }

  class MockSnapshotParamRequestNodeSoE : public test::MockEthercatMasterParamRequestNodeSoE
  {
  public:
    template <typename TValue>
    void setValue(const std::uint16_t idn, const TValue value)
    {
      std::vector<std::uint8_t> raw(sizeof(TValue));
      std::memcpy(raw.data(), &value, sizeof(TValue));
      m_responseData[idn] = raw;
    }

    void setFailure(const std::uint16_t idn, const comm::datalayer::DlResult result)
    {
      m_failures[idn] = result;
    }

    void clearFailure(const std::uint16_t idn)
    {
      m_failures.erase(idn);
    }

    void onRead(const std::string& /*address*/, const comm::datalayer::Variant* data, const ResponseCallback& callback) override
    {
      if (!data || (data->getType() != comm::datalayer::VariantType::FLATBUFFERS) ||
          comm::datalayer::STATUS_FAILED(data->verifyFlatbuffers(comm::ethercat::master::fbs::VerifyParameterBuffer)))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }

      auto paramData = comm::ethercat::master::fbs::GetParameter(data->getData());
      if (!flatbuffers::IsFieldPresent(paramData, comm::ethercat::master::fbs::Parameter::VT_REQUEST))
      {
        callback(comm::datalayer::DlResult::DL_INVALID_VALUE, nullptr);
        return;
      }

      const auto idn = paramData->request()->idn();
      const auto failureIt = m_failures.find(idn);
      if (failureIt != m_failures.end())
      {
        callback(failureIt->second, nullptr);
        return;
      }

      const auto dataIt = m_responseData.find(idn);
      comm::datalayer::Variant returnValue {};
      if (dataIt != m_responseData.end())
      {
        flatbuffers::FlatBufferBuilder builder;
        builder.ForceDefaults(true);

        const auto dataOffset = builder.CreateVector(dataIt->second.data(), dataIt->second.size());
        const auto responseOffset = CreateParameterResponse(builder,
                                                            comm::ethercat::master::fbs::ElementFlags_value,
                                                            dataOffset);

        comm::ethercat::master::fbs::ParameterBuilder paramBuilder(builder);
        paramBuilder.add_response(responseOffset);
        builder.Finish(paramBuilder.Finish());
        returnValue.copyFlatbuffers(builder);
      }
      else
      {
        m_fbsHelper.generateParamResponseFlatbuffer(&returnValue, idn);
      }
      callback(comm::datalayer::DlResult::DL_OK, &returnValue);
    }

  private:
    std::unordered_map<std::uint16_t, std::vector<std::uint8_t>> m_responseData;
    std::unordered_map<std::uint16_t, comm::datalayer::DlResult> m_failures;
  };

  class MockFieldbusAdapterSoECaptureElementFlag : public test::MockFieldbusAdapterSoE
  {
  public:
    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      const auto* readReqSoE = static_cast<const ParamReqDataSoE*>(&readReq);
      m_lastIdn = readReqSoE->idn;
      m_lastElementFlag = static_cast<comm::ethercat::master::fbs::ElementFlags>(readReqSoE->elementFlag);

      if (bufPtr == nullptr || bufLen < sizeof(m_returnValue))
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      std::memcpy(bufPtr, &m_returnValue, sizeof(m_returnValue));
      readLen = sizeof(m_returnValue);
      return AxisProfileResult::STS_OK;
    }

    std::uint16_t m_lastIdn {};
    std::uint16_t m_returnValue {0x55AA};
    comm::ethercat::master::fbs::ElementFlags m_lastElementFlag {comm::ethercat::master::fbs::ElementFlags::ElementFlags_NONE};
  };

  ProfileBasicInfo createProfileBasicInfo(test::MockFieldbusAdapterSoE& adapter)
  {
    ProfileBasicInfo profileBasicInfo {};
    profileBasicInfo.m_deviceAddress = 1;
    profileBasicInfo.m_driveNumber = 0;
    profileBasicInfo.m_fieldbusAdapter = &adapter;
    profileBasicInfo.m_profileName = "testProfile";
    return profileBasicInfo;
  }
}


HRESULT TestSercosBootParmSnapshot::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the Sercos boot parameter snapshot class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testGetParameterReadFromDriveAndPersist();
  hresult |= testGetParameterReadFromSnapshotCache();
  hresult |= testGetParameterForceReadFromDrive();
  hresult |= testGetParameterValidationAndErrorPaths();
  hresult |= testGetParameterElementFlagForwarding();
  hresult |= testGetSoeIndraDriveBootParamList();
  return hresult;
}

HRESULT TestSercosBootParmSnapshot::testGetParameterReadFromDriveAndPersist()
{
  log(L"\n<b>Test reading Sercos parameters from drive and persisting snapshot.</b>");

  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeSoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  requestNode.setValue(P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR, static_cast<std::uint16_t>(0x0011));
  requestNode.setValue(S_0_0429_EMERGENCY_HALT_DECELERATION, static_cast<std::int32_t>(123456));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  SercosBootParamSnapshot testClass {profileBasicInfo};

  AxisProfileResult result {AxisProfileResult::ERR_NOK};
  std::uint16_t reactionOnError {};
  result = testClass.getParameter(P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR, reinterpret_cast<std::uint8_t*>(&reactionOnError), sizeof(reactionOnError));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(reactionOnError, L"\nUnexpected reaction on error.", 0x0011);

  std::int32_t emergencyHaltDeceleration {};
  result = testClass.getParameter(S_0_0429_EMERGENCY_HALT_DECELERATION, reinterpret_cast<std::uint8_t*>(&emergencyHaltDeceleration), sizeof(emergencyHaltDeceleration));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(emergencyHaltDeceleration, L"\nUnexpected emergency halt deceleration.", 123456);

  const auto& snapshot = testClass.getBootParamSnapshot();
  LOG_TEST_RESULT_ANY(snapshot.protocol == BootSnapshotProtocol::SERCOS, L"\nSnapshot protocol should be Sercos.");
  LOG_TEST_RESULT_ANY(snapshot.profileName == "testProfile", L"\nSnapshot profile name should be initialized by constructor.");
  LOG_TEST_RESULT_ANY(getParamU16(snapshot.paramMap, idnToString(P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR), reactionOnError), L"\nUnexpected persisted reaction on error.");
  LOG_TEST_RESULT(reactionOnError, L"\nUnexpected persisted reaction on error.", 0x0011);
  LOG_TEST_RESULT_ANY(getParamI32(snapshot.paramMap, idnToString(S_0_0429_EMERGENCY_HALT_DECELERATION), emergencyHaltDeceleration), L"\nUnexpected persisted emergency halt deceleration.");
  LOG_TEST_RESULT(emergencyHaltDeceleration, L"\nUnexpected persisted emergency halt deceleration.", 123456);

  return S_OK;
}

HRESULT TestSercosBootParmSnapshot::testGetParameterReadFromSnapshotCache()
{
  log(L"\n<b>Test reading Sercos parameter from configured snapshot cache.</b>");

  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeSoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const std::uint16_t idn = P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR;
  requestNode.setValue(idn, static_cast<std::uint16_t>(0x5678));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  SercosBootParamSnapshot testClass {profileBasicInfo};

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::SERCOS;
  setParamU16(snapshot, idnToString(idn), 0x1234);
  testClass.setBootParamSnapshot(snapshot);

  std::uint16_t value {};
  const auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nCache value should be used when snapshot is valid.", 0x1234);

  return S_OK;
}

HRESULT TestSercosBootParmSnapshot::testGetParameterForceReadFromDrive()
{
  log(L"\n<b>Test forcing drive read bypasses cache and updates snapshot.</b>");

  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeSoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const std::uint16_t idn = P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR;
  requestNode.setValue(idn, static_cast<std::uint16_t>(0x5678));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  SercosBootParamSnapshot testClass {profileBasicInfo};

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::SERCOS;
  setParamU16(snapshot, idnToString(idn), 0x1234);
  testClass.setBootParamSnapshot(snapshot);

  std::uint16_t value {};
  auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nCache value should be returned without force flag.", 0x1234);

  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nDrive value should be returned when force flag is set.", 0x5678);

  const auto& persisted = testClass.getBootParamSnapshot();
  std::uint16_t persistedValue {};
  LOG_TEST_RESULT_ANY(getParamU16(persisted, idnToString(idn), persistedValue), L"\nPersisted value should exist after force read.");
  LOG_TEST_RESULT(persistedValue, L"\nForce read should refresh persisted value.", 0x5678);

  return S_OK;
}

HRESULT TestSercosBootParmSnapshot::testGetParameterValidationAndErrorPaths()
{
  log(L"\n<b>Test parameter validation and error paths.</b>");

  test::MockFieldbusAdapterSoE mockFieldbusAdapter {};
  test::MockDatalayerClient mockClient {};
  MockSnapshotParamRequestNodeSoE requestNode {};
  mockClient.m_testNode = &requestNode;
  mockFieldbusAdapter.setDatalayer(static_cast<comm::datalayer::IClient*>(&mockClient));

  const std::uint16_t idn = P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR;
  requestNode.setValue(idn, static_cast<std::uint16_t>(0x1111));

  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  SercosBootParamSnapshot testClass {profileBasicInfo};

  std::uint16_t value {};
  AxisProfileResult result = testClass.getParameter(0xFFFF, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnknown IDN should be rejected.", AxisProfileResult::ERR_INVALID_ARGUMENT);

  std::uint8_t shortBuffer[1] {};
  result = testClass.getParameter(idn, shortBuffer, sizeof(shortBuffer));
  RETURN_TEST_RESULT(result, L"\nToo small buffer should be rejected for drive read.", AxisProfileResult::ERR_WRONGDATASIZE);

  requestNode.setFailure(idn, comm::datalayer::DlResult::DL_FAILED);
  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nDrive read failure should be reported.", AxisProfileResult::ERR_FIELDBUS);

  BootParamSnapshot snapshot {};
  snapshot.protocol = BootSnapshotProtocol::SERCOS;
  setParamU16(snapshot, idnToString(idn), 0x2222);
  testClass.setBootParamSnapshot(snapshot);

  result = testClass.getParameter(idn, shortBuffer, sizeof(shortBuffer));
  RETURN_TEST_RESULT(result, L"\nToo small buffer should be rejected for cache read.", AxisProfileResult::ERR_WRONGDATASIZE);

  setParamUnknownType(snapshot, idnToString(idn));
  testClass.setBootParamSnapshot(snapshot);
  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnknown cache value type should be rejected.", AxisProfileResult::ERR_WRONGDATATYPE);

  snapshot.protocol = BootSnapshotProtocol::UNKNOWN;
  setParamU16(snapshot, idnToString(idn), 0x3333);
  testClass.setBootParamSnapshot(snapshot);
  requestNode.clearFailure(idn);
  requestNode.setValue(idn, static_cast<std::uint16_t>(0x4444));
  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnknown protocol should bypass cache and read drive.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nUnknown protocol must not serve cached value.", 0x4444);

  return S_OK;
}

HRESULT TestSercosBootParmSnapshot::testGetParameterElementFlagForwarding()
{
  log(L"\n<b>Test SoE element flag forwarding in parameter read request.</b>");

  MockFieldbusAdapterSoECaptureElementFlag mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  SercosBootParamSnapshot testClass {profileBasicInfo};

  const std::uint16_t idn = P_0_0117_CONTROL_UNIT_REACTION_ON_ERROR;
  std::uint16_t value {};

  auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), true, comm::ethercat::master::fbs::ElementFlags::ElementFlags_maxValue);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for explicit element flag.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nUnexpected value for explicit element flag.", mockFieldbusAdapter.m_returnValue);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_lastIdn, L"\nUnexpected IDN forwarded to fieldbus read request.", idn);
  LOG_TEST_RESULT(static_cast<std::uint32_t>(mockFieldbusAdapter.m_lastElementFlag),
                  L"\nExplicit element flag should be forwarded to fieldbus read request.",
                  static_cast<std::uint32_t>(comm::ethercat::master::fbs::ElementFlags::ElementFlags_maxValue));

  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value), true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for default element flag.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(static_cast<std::uint32_t>(mockFieldbusAdapter.m_lastElementFlag),
                  L"\nDefault element flag should be forwarded to fieldbus read request.",
                  static_cast<std::uint32_t>(comm::ethercat::master::fbs::ElementFlags::ElementFlags_value));

  return S_OK;
}

HRESULT TestSercosBootParmSnapshot::testGetSoeIndraDriveBootParamList()
{
  log(L"\n<b>Test default IndraDrive SoE boot parameter list.</b>");

  std::vector<std::uint16_t> expected {};
  expected.reserve(SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE.size());
  for (const auto& descriptor : SERCOS_BOOT_PARAM_DESCRIPTOR_TABLE)
  {
    if (descriptor.series == DeviceSeries::INDRADRIVE)
    {
      expected.push_back(descriptor.idn);
    }
  }

  const auto actual = getSoeIndraDriveBootParamList();
  LOG_TEST_RESULT(actual.size(), L"\nUnexpected number of IndraDrive boot parameters.", expected.size());
  LOG_TEST_RESULT_ANY(actual == expected, L"\nIndraDrive boot parameter list does not match descriptor table order/filtering.");

  const auto ctrlxSpecific1 = std::find(actual.begin(), actual.end(), P_0_0117_0_1_REACTION_DELAY_F2_F3);
  LOG_TEST_RESULT_ANY(ctrlxSpecific1 == actual.end(), L"\nCtrlX-specific IDN must not be part of the IndraDrive list.");

  const auto ctrlxSpecific2 = std::find(actual.begin(), actual.end(), P_0_0117_0_2_REACTION_DELAY_F4_F6_F7);
  LOG_TEST_RESULT_ANY(ctrlxSpecific2 == actual.end(), L"\nCtrlX-specific IDN must not be part of the IndraDrive list.");

  return S_OK;
}
