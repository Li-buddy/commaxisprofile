#pragma once

#include "helper/result_helper.h"
#include "profiles/canopen/canopen_defs.h"
#include "profiles/canopen/drive/canopen_signal_func_handler.h"
#include "profiles/data_sharing/sharing_handler.h"

#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"

#include <algorithm>
#include <cstring>
#include <unordered_map>

using namespace comm::axisprofile;

namespace
{
  std::uint32_t signalWordParam(const ObjectDirEntry& objEntry)
  {
    return (static_cast<std::uint32_t>(objEntry.index) << 16) | (static_cast<std::uint32_t>(objEntry.subIndex) << 8);
  }

  class RawListFieldbusAdapterCoE : public test::MockFieldbusAdapterCoE
  {
  public:
    template <typename T>
    void setListResponse(const ObjectDirEntry& objEntry, const std::vector<T>& values)
    {
      std::vector<std::uint8_t> rawData(values.size() * sizeof(T));
      if (!values.empty())
      {
        std::memcpy(rawData.data(), values.data(), rawData.size());
      }
      m_rawResponses[toKey(objEntry)] = rawData;
    }

    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen) override
    {
      std::size_t readLen {};
      return readSingleParamSync(readReq, bufPtr, bufLen, readLen);
    }

    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readReq, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      if (aprStatusFailed(m_returnValue))
      {
        return m_returnValue;
      }

      const auto* readDataCoE = static_cast<const ParamReqDataCoE*>(&readReq);
      m_readObjects.emplace_back(readDataCoE->objEntry);
      const auto itResponse = m_rawResponses.find(toKey(readDataCoE->objEntry));
      if (itResponse == m_rawResponses.end())
      {
        readLen = 0;
        return AxisProfileResult::ERR_FIELDBUS;
      }

      readLen = std::min(bufLen, itResponse->second.size());
      std::memcpy(bufPtr, itResponse->second.data(), readLen);
      return AxisProfileResult::STS_OK;
    }

    std::size_t getReadCount(const ObjectDirEntry& objEntry) const
    {
      return static_cast<std::size_t>(std::count_if(m_readObjects.begin(),
                                                   m_readObjects.end(),
                                                   [&objEntry] (const ObjectDirEntry& readObj)
                                                   {
                                                     return readObj == objEntry;
                                                   }));
    }

  private:
    static std::uint32_t toKey(const ObjectDirEntry& objEntry)
    {
      return (static_cast<std::uint32_t>(objEntry.index) << 8) | objEntry.subIndex;
    }

    std::unordered_map<std::uint32_t, std::vector<std::uint8_t>> m_rawResponses {};
    std::vector<ObjectDirEntry> m_readObjects {};
  };

  struct TestContext
  {
    TestContext()
      : sharingHandler(&profileTrace), testClass(profileBasicInfo)
    {
      profileBasicInfo.m_deviceAddress = 1;
      profileBasicInfo.m_driveNumber = 0;
      profileBasicInfo.m_fieldbusAdapter = &fieldbusAdapter;
      profileBasicInfo.m_profileName = "testProfile";

      auto& profileAssignedInfo = mappingHandler.getProfileAssignedInfo();
      profileAssignedInfo.m_input.m_signalStatusWord = true;
      profileAssignedInfo.m_output.m_signalControlWord = true;
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandler);
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);
    }

    ObjectDirEntry getObj(const ObjectDirEntry& baseObj) const
    {
      return canopen::getObjDir(baseObj, profileBasicInfo.m_driveNumber);
    }

    void setStatusWordResponses(const std::vector<std::uint32_t>& params, const std::vector<std::uint16_t>& assignBits)
    {
      using namespace comm::axisprofile::canopen;

      fieldbusAdapter.setListResponse<std::uint32_t>(getObj(SIGNAL_STATUS_WORD_CONFIG_LIST), params);
      fieldbusAdapter.setListResponse<std::uint16_t>(getObj(SIGNAL_STATUS_WORD_ASSIGN_LIST), assignBits);
    }

    void setControlWordResponses(const std::vector<std::uint32_t>& params = {signalWordParam(comm::axisprofile::canopen::SET_ABSOLUTE_POSITION_CONTROL)},
                                 const std::vector<std::uint16_t>& assignBits = {0})
    {
      using namespace comm::axisprofile::canopen;

      fieldbusAdapter.setListResponse<std::uint32_t>(getObj(SIGNAL_CONTROL_WORD_CONFIG_LIST), params);
      fieldbusAdapter.setListResponse<std::uint16_t>(getObj(SIGNAL_CONTROL_WORD_ASSIGN_LIST), assignBits);
    }

    void setSupportedSignalWordResponses()
    {
      using namespace comm::axisprofile::canopen;

      setControlWordResponses();
      setStatusWordResponses({signalWordParam(STATUS_WORD_CTRLX_DRIVE), signalWordParam(HOMING_FEEDBACK_VALUE_STATUS)}, {0, 0});
    }

    test::MockAxisProfileTrace profileTrace {};
    ProfileBasicInfo profileBasicInfo {};
    RawListFieldbusAdapterCoE fieldbusAdapter {};
    test::MockAxisProfileMappingHandler mappingHandler {};
    test::MockAxisProfileCyclicData cyclicData {};
    AxisProfileSharingHandler sharingHandler;
    comm::axisprofile::canopen::CanopenSignalFuncHandler testClass;
  };
}

class TestCanopenSignalFuncMgr : public xTST_Test2
{
public:
  TestCanopenSignalFuncMgr() = default;
  ~TestCanopenSignalFuncMgr() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.SignalFuncHandler");
  TEST_DECLARE_DESCRIPTION("Unittest of the Canopen signal function handler class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() override;

private:
  /// @brief Test the supported signal functions.
  /// @return S_OK, if test was successful.
  HRESULT testSupportedSignalFunctions();

  /// @brief Test unsupported and unregistered signal functions.
  /// @return S_OK, if test was successful.
  HRESULT testUnsupportedSignalFunctions();

  /// @brief Test the signal word mapping required.
  /// @return S_OK, if test was successful.
  HRESULT testSignalWordMappingRequired();

  /// @brief Test that CoE list reads only parse the actual response length.
  /// @return S_OK, if test was successful.
  HRESULT testReadParamListUsesActualReadLength();

  /// @brief Test configuration read error handling.
  /// @return S_OK, if test was successful.
  HRESULT testConfigurationReadErrors();
};
