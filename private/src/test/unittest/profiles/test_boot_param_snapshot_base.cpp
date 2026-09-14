#include "stdafx.h"

#include "helper/result_helper.h"
#include "profiles/boot_param_snapshot_base.h"
#include "test_boot_param_snapshot_base.h"

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "mockup/mock_fieldbus_adapter.h"

using namespace comm::axisprofile;
using namespace test;

REGISTER_TEST(TestBootParamSnapshotBase);
REGISTER_TEST_TO_GROUP(TestBootParamSnapshotBase, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestBootParamSnapshotBase, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestBootParamSnapshotBase, Profiles);

namespace
{
  bool getParamU16(const BootParamSnapshot& snapshot, const std::string& name, std::uint16_t& value)
  {
    auto it = snapshot.paramMap.find(name);
    if (it == snapshot.paramMap.end() || it->second.type != BootParamValueType::UNSIGNED16)
    {
      return false;
    }

    value = it->second.value.valueUint16;
    return true;
  }

  bool getParamByteArray(const BootParamSnapshot& snapshot, const std::string& name, std::vector<std::uint8_t>& value)
  {
    auto it = snapshot.paramMap.find(name);
    if (it == snapshot.paramMap.end() || it->second.type != BootParamValueType::BYTEARRAY)
    {
      return false;
    }

    value = it->second.value.valueByteArray;
    return true;
  }

  class MockFieldbusAdapterSnapshotBase : public test::MockFieldbusAdapter
  {
  public:
    AxisProfileResult getParamResponseData(const comm::datalayer::Variant* /*variantData*/, std::uint8_t* bufPtr, std::size_t bufLen,
                                           std::size_t& readLen) override
    {
      if (bufPtr == nullptr || bufLen < m_nextReadData.size())
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      if (!m_returnResultVariable.has_value())
      {
        std::memcpy(bufPtr, m_nextReadData.data(), m_nextReadData.size());
        readLen = m_nextReadData.size();
      }

      return m_returnResultVariable.value_or(AxisProfileResult::STS_OK);
    }

    AxisProfileResult readSingleParamSync(const ParamReqDataBase& /*readReq*/, std::uint8_t* bufPtr, std::size_t bufLen) override
    {
      ++m_fixedLengthReadCount;
      if (bufPtr == nullptr || bufLen < m_nextReadData.size())
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      if (!m_returnResultFixed.has_value())
      {
        std::memcpy(bufPtr, m_nextReadData.data(), m_nextReadData.size());
      }

      return m_returnResultFixed.value_or(AxisProfileResult::STS_OK);
    }

    AxisProfileResult readSingleParamSync(const ParamReqDataBase& /*readReq*/, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      ++m_variableLengthReadCount;
      if (bufPtr == nullptr || bufLen < m_nextReadData.size())
      {
        return AxisProfileResult::ERR_WRONGDATASIZE;
      }

      if (!m_returnResultVariable.has_value())
      {
        std::memcpy(bufPtr, m_nextReadData.data(), m_nextReadData.size());
        readLen = m_nextReadData.size();
      }

      return m_returnResultVariable.value_or(AxisProfileResult::STS_OK);
    }

    AxisProfileResult readSingleParamAsync(ParameterResponseCallbackData* const /*responseCallbackData*/, const ParamReqDataBase& /*readData*/) override
    {
      return AxisProfileResult::ERR_NOTIMPLEMENTED;
    }

    AxisProfileResult writeSingleParamAsync(ParameterResponseCallbackData* /*responseCallbackData*/, const ParamReqDataBase& /*writeData*/) override
    {
      return AxisProfileResult::ERR_NOTIMPLEMENTED;
    }

    AxisProfileResult writeSingleParamSync(const ParamReqDataBase& /*writeData*/) override
    {
      return AxisProfileResult::ERR_NOTIMPLEMENTED;
    }

    std::vector<std::uint8_t> m_nextReadData {};
    std::optional<AxisProfileResult> m_returnResultFixed {};
    std::optional<AxisProfileResult> m_returnResultVariable {};
    std::uint32_t m_fixedLengthReadCount {};
    std::uint32_t m_variableLengthReadCount {};
  };

  class TestBootParamSnapshotBaseImpl : public BootParamSnapshotBase
  {
  public:
    explicit TestBootParamSnapshotBaseImpl(ProfileBasicInfo& profileBasicInfo) : BootParamSnapshotBase(profileBasicInfo, BootSnapshotProtocol::SERCOS)
    {
    }

    AxisProfileResult getParameter(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromDrive = false)
    {
      return getParameterImpl(BootParamId(idn), bufPtr, bufLen, forceReadFromDrive);
    }

    void setDescriptor(std::uint16_t idn, BootParamValueType type, std::size_t valueLen)
    {
      m_descriptors[idn] = {type, valueLen};
    }

    void setGenerateReqDataNull(bool enabled)
    {
      m_generateReqDataNull = enabled;
    }

  private: // methods

    std::string parameterToString(const BootParamId& paramId) const override
    {
      const auto* idn = paramId.getIdn();
      return idn ? idnToString(*idn) : std::string("invalid-parameter-id");
    }

    const BootParamDescriptorBase* getBootParamDescriptor(const BootParamId& paramId) const override
    {
      const auto* idn = paramId.getIdn();
      if (idn == nullptr)
      {
        return nullptr;
      }

      const auto it = m_descriptors.find(*idn);
      if (it == m_descriptors.end())
      {
        return nullptr;
      }

      m_descriptorCache = it->second;
      return &m_descriptorCache;
    }

    std::unique_ptr<ParamReqDataBase> generateReqData(const BootParamId& /*paramId*/) const override
    {
      if (m_generateReqDataNull)
      {
        return nullptr;
      }

      return std::make_unique<ParamReqDataBase>();
    }

  private: // data

    std::map<std::uint16_t, BootParamDescriptorBase> m_descriptors {};
    bool m_generateReqDataNull {false};
  };

  ProfileBasicInfo createProfileBasicInfo(test::MockFieldbusAdapter& adapter)
  {
    ProfileBasicInfo profileBasicInfo {};
    profileBasicInfo.m_deviceAddress = 1;
    profileBasicInfo.m_driveNumber = 0;
    profileBasicInfo.m_fieldbusAdapter = &adapter;
    profileBasicInfo.m_profileName = "testProfile";
    return profileBasicInfo;
  }
}

HRESULT TestBootParamSnapshotBase::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the boot parameter snapshot base class");
  log(L"\n----------------------------------------------------------------\n");

  HRESULT hresult = S_OK;
  hresult |= testDriveReadDoesNotImplicitlyEnableCache();
  hresult |= testGenerateReqDataFailure();
  hresult |= testUnsupportedDescriptorType();
  hresult |= testByteArrayReadUsesActualReadLength();
  return hresult;
}

HRESULT TestBootParamSnapshotBase::testDriveReadDoesNotImplicitlyEnableCache()
{
  log(L"\n<b>Test drive reads do not implicitly enable cache reuse.</b>");

  MockFieldbusAdapterSnapshotBase mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  TestBootParamSnapshotBaseImpl testClass {profileBasicInfo};

  const std::uint16_t idn = IDN_P(0, 117);
  testClass.setDescriptor(idn, BootParamValueType::UNSIGNED16, sizeof(std::uint16_t));

  std::uint16_t value {};
  std::uint16_t firstReadValue = 0x1111;
  mockFieldbusAdapter.m_nextReadData.assign(reinterpret_cast<std::uint8_t*>(&firstReadValue), reinterpret_cast<std::uint8_t*>(&firstReadValue) + sizeof(firstReadValue));

  auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for first drive read.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nUnexpected value for first drive read.", 0x1111);

  std::uint16_t secondReadValue = 0x2222;
  mockFieldbusAdapter.m_nextReadData.assign(reinterpret_cast<std::uint8_t*>(&secondReadValue), reinterpret_cast<std::uint8_t*>(&secondReadValue) + sizeof(secondReadValue));

  result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for second drive read.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(value, L"\nSecond access should still read from drive until external snapshot is configured.", 0x2222);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_fixedLengthReadCount, L"\nDrive should be accessed twice.", 2U);

  const auto& snapshot = testClass.getBootParamSnapshot();
  std::uint16_t persistedValue {};
  LOG_TEST_RESULT_ANY(getParamU16(snapshot, idnToString(idn), persistedValue), L"\nPersisted value should exist after drive read.");
  LOG_TEST_RESULT(persistedValue, L"\nPersisted value should be refreshed by latest drive read.", 0x2222);

  return S_OK;
}

HRESULT TestBootParamSnapshotBase::testGenerateReqDataFailure()
{
  log(L"\n<b>Test generate request data failure path.</b>");

  MockFieldbusAdapterSnapshotBase mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  TestBootParamSnapshotBaseImpl testClass {profileBasicInfo};

  const std::uint16_t idn = IDN_P(0, 118);
  testClass.setDescriptor(idn, BootParamValueType::UNSIGNED16, sizeof(std::uint16_t));
  testClass.setGenerateReqDataNull(true);

  std::uint16_t value {};
  const auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nNull request data should be rejected.", AxisProfileResult::ERR_INVALID_ARGUMENT);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_fixedLengthReadCount, L"\nDrive should not be accessed when request generation fails.", 0U);

  return S_OK;
}

HRESULT TestBootParamSnapshotBase::testUnsupportedDescriptorType()
{
  log(L"\n<b>Test unsupported descriptor value type path.</b>");

  MockFieldbusAdapterSnapshotBase mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  TestBootParamSnapshotBaseImpl testClass {profileBasicInfo};

  const std::uint16_t idn = IDN_P(0, 119);
  testClass.setDescriptor(idn, static_cast<BootParamValueType>(0xFFFF), sizeof(std::uint16_t));

  std::uint16_t readValue = 0x1234;
  mockFieldbusAdapter.m_nextReadData.assign(reinterpret_cast<std::uint8_t*>(&readValue), reinterpret_cast<std::uint8_t*>(&readValue) + sizeof(readValue));

  std::uint16_t value {};
  const auto result = testClass.getParameter(idn, reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
  RETURN_TEST_RESULT(result, L"\nUnsupported descriptor value type should be rejected.", AxisProfileResult::ERR_INVALID_ARGUMENT);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_fixedLengthReadCount, L"\nDrive should be accessed before value type rejection.", 1U);

  return S_OK;
}

HRESULT TestBootParamSnapshotBase::testByteArrayReadUsesActualReadLength()
{
  log(L"\n<b>Test byte-array read stores only the actual read length.</b>");

  MockFieldbusAdapterSnapshotBase mockFieldbusAdapter {};
  auto profileBasicInfo = createProfileBasicInfo(mockFieldbusAdapter);
  TestBootParamSnapshotBaseImpl testClass {profileBasicInfo};

  const std::uint16_t idn = IDN_P(0, 120);
  testClass.setDescriptor(idn, BootParamValueType::BYTEARRAY, 1U);
  mockFieldbusAdapter.m_nextReadData = {0x10, 0x20, 0x30, 0x40};

  std::uint8_t valueBuffer[8] {};
  const auto result = testClass.getParameter(idn, valueBuffer, sizeof(valueBuffer));
  RETURN_TEST_RESULT(result, L"\nUnexpected return value for byte-array read.", AxisProfileResult::STS_OK);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_variableLengthReadCount, L"\nByte-array reads should use the variable-length fieldbus path.", 1U);
  LOG_TEST_RESULT(mockFieldbusAdapter.m_fixedLengthReadCount, L"\nByte-array reads should not use the fixed-length fieldbus path.", 0U);
  LOG_TEST_RESULT_ANY(std::memcmp(valueBuffer, mockFieldbusAdapter.m_nextReadData.data(), mockFieldbusAdapter.m_nextReadData.size()) == 0,
                      L"\nUnexpected raw byte-array read result.");

  const auto& snapshot = testClass.getBootParamSnapshot();
  std::vector<std::uint8_t> persistedValue {};
  LOG_TEST_RESULT_ANY(getParamByteArray(snapshot, idnToString(idn), persistedValue), L"\nPersisted byte-array value should exist.");
  LOG_TEST_RESULT_ANY(persistedValue == mockFieldbusAdapter.m_nextReadData, L"\nPersisted byte-array should match actual read length and data.");

  return S_OK;
}
