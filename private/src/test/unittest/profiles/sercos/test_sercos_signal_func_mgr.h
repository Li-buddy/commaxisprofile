#pragma once

#include "helper/result_helper.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/sercos/drive/sercos_device_param_drive.h"
#include "profiles/sercos/drive/sercos_signal_func_handler.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "profiles/sercos/sercos_defs.h"

#include "fieldbuses/fieldbus_helper.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"

#include <algorithm>
#include <cstring>
#include <unordered_map>

using namespace comm::axisprofile;

namespace
{
  class RawListFieldbusAdapterSoE : public test::MockFieldbusAdapterSoE
  {
  public:
    template <typename T>
    void setListResponse(std::uint16_t idn, const std::vector<T>& values)
    {
      std::vector<std::uint8_t> rawData {};
      FieldbusHelper::paramListToRawSoE(values, rawData);
      m_rawResponses[idn] = rawData;
    }

    AxisProfileResult readSingleParamSync(const ParamReqDataBase& readData, std::uint8_t* bufPtr, std::size_t bufLen, std::size_t& readLen) override
    {
      if (aprStatusFailed(m_returnValue))
      {
        return m_returnValue;
      }

      const auto* readDataSoE = static_cast<const ParamReqDataSoE*>(&readData);
      const auto itResponse = m_rawResponses.find(readDataSoE->idn);
      if (itResponse == m_rawResponses.end())
      {
        readLen = 0;
        return AxisProfileResult::ERR_FIELDBUS;
      }

      readLen = std::min(bufLen, itResponse->second.size());
      std::memcpy(bufPtr, itResponse->second.data(), readLen);
      return AxisProfileResult::STS_OK;
    }

  private:
    std::unordered_map<std::uint16_t, std::vector<std::uint8_t>> m_rawResponses {};
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

      m_deviceParam = std::make_unique<sercos::SercosDeviceParamDrive>(profileBasicInfo, nullptr, nullptr);
      m_bootParamSnapshot = std::make_unique<sercos::SercosBootParamSnapshot>(profileBasicInfo);

      auto& profileAssignedInfo = mappingHandler.getProfileAssignedInfo();
      profileAssignedInfo.m_input.m_signalStatusWord = true;
      profileAssignedInfo.m_output.m_signalControlWord = true;
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandler);
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, &cyclicData);
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, m_bootParamSnapshot.get());
      sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, m_deviceParam.get());
    }

    void setSignalWordResponses(bool signalControlWordMapped)
    {
      using namespace comm::axisprofile::sercos;

      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0016_CONFIG_LIST_AT, {S_0_0144_SIGNAL_STATUS_WORD});
      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0024_CONFIG_LIST_MDT,
                                                     signalControlWordMapped ? std::vector<std::uint16_t> {S_0_0145_SIGNAL_CONTROL_WORD}
      : std::vector<std::uint16_t> {S_0_0134_MASTER_CONTROL_WORD});
      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0026_CONFIG_LIST_SIGNAL_STATUS_WORD,
                                                     {S_0_0403_POSITION_FEEDBACK_VALUE_STATUS, S_0_0099_RESET_CLASS_1_DIAGNOSTIC});
      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0027_CONFIG_LIST_SIGNAL_CONTROL_WORD,
                                                     {S_0_0148_DRIVE_CONTROLLED_HOMING, S_0_0099_RESET_CLASS_1_DIAGNOSTIC});
      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0328_ASSIGN_LIST_SIGNAL_STATUS_WORD, {0, 0});
      fieldbusAdapter.setListResponse<std::uint16_t>(S_0_0329_ASSIGN_LIST_SIGNAL_CONTROL_WORD, {0, 0});
    }

    test::MockAxisProfileTrace profileTrace {};
    ProfileBasicInfo profileBasicInfo {};
    RawListFieldbusAdapterSoE fieldbusAdapter {};
    test::MockAxisProfileMappingHandler mappingHandler {};
    test::MockAxisProfileCyclicData cyclicData {};
    AxisProfileSharingHandler sharingHandler;
    std::unique_ptr<sercos::SercosDeviceParamDrive> m_deviceParam {};
    std::unique_ptr<sercos::SercosBootParamSnapshot> m_bootParamSnapshot {};
    comm::axisprofile::sercos::SercosSignalFuncHandler testClass;
  };
}

class TestSercosSignalFuncHandler : public xTST_Test2
{
public:
  TestSercosSignalFuncHandler() = default;
  ~TestSercosSignalFuncHandler() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.SignalFuncHandler");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos signal function handler class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() override;

private:

  /// @brief Test the supported signal functions
  /// @return S_OK, if test was successful 
  HRESULT testSupportedSignalFunctions();

  /// @brief Test the signal control word omitted from MDT
  /// @return S_OK, if test was successful 
  HRESULT testSignalControlWordNotInMdt();

  /// @brief Test the signal word mapping required
  /// @return S_OK, if test was successful 
  HRESULT testSignalWordMappingRequired();

  /// @brief Test the error handling for unsupported functions
  /// @return S_OK, if test was successful 
  HRESULT testErrorHandling();
};
