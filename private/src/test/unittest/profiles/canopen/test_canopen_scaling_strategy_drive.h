#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_drive.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/category/drive/category_adapter_drive.h"
#include "stdafx.h"
#include <functional>
#include <string>
#include <vector>

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include "mockup/mock_profile_trace.h"

using namespace comm::axisprofile;
using namespace comm::axisprofile::canopen;

/// @brief Test class for the can scaling class.
class TestCanopenScalingStrategyDrive : public xTST_Test2
{
public:// type

  struct TestData
  {
    std::function<HRESULT(const TestData*)> m_testFunctional{};   ///< specific tested function
    void* m_testScalingData{};                                    ///< unit scaling data
    comm::axisprofile::AxisProfileResult m_expResult{};           ///< expected return code of the axis-profile
    const wchar_t* m_testName{};                                  ///< name of the test
  };

public: // methods
  explicit TestCanopenScalingStrategyDrive() = default;
  ~TestCanopenScalingStrategyDrive() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.Drive_ScalingStrategy");
  TEST_DECLARE_DESCRIPTION("Unittest of the Scaling strategy unit object.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  HRESULT doInit() final;
  HRESULT doTest() final;

private: // methods
  HRESULT testReadScalingParamsSync();
  HRESULT testReadScalingParamsSyncFailed();
  HRESULT testCalcScalingLSBWeights() ;
  HRESULT testCalcPosScalingLSBWeight(const TestData* testData);
  HRESULT testCalcVelScalingLSBWeight(const TestData* testData) ;
  HRESULT testCalcAccScalingLSBWeight(const TestData* testData) ;
  HRESULT testCalcTorqueScalingLSBWeight(const TestData* testData);
  HRESULT testLoadLSBWeightsMethods();

private: // data

  std::vector<TestData> m_vecTestDataValid {};       ///< store the valid test case
  std::vector<TestData> m_vecTestDataWrong {};       ///< store the wrong test case
  std::vector<TestData> m_vecTestDataInvalid {};     ///< store the invalid test case
  std::vector<TestData> m_vecTestDataSpecialWrong {};///< store the special wrong test case
  const std::string m_profileName = "testProfile";   ///< name of the test axis profile
  std::uint32_t m_deviceAddress {1};                      ///< device address          
  ProfileBasicInfo m_profileBasicInfo;               ///< basic information of the profile

  std::shared_ptr<test::MockAxisProfileTrace>                  m_mockProfileTrace{};      ///< mock trace
  std::shared_ptr<test::MockFieldbusAdapterCoE>                m_mockFieldbusAdapter{};   ///< fieldbus adapter
  std::shared_ptr<test::MockDatalayerClient>                   m_mockClient{};            ///< mock client
  std::shared_ptr<test::MockEthercatMasterSDORequestNodeCoE>   m_sdoRequestNodeMockCoE{}; ///< SDO request node ptr
  std::unique_ptr<ScalingStrategyCoEDrive>                     m_testObject{};            ///< the tested class object
  std::unique_ptr<test::MockAxisProfileMappingHandler>         m_pMappingHandlerMockup {};///< pointer to mapping handler
  std::unique_ptr<CategoryAdapterDrive>                        m_mockCategory {};         ///< 
  std::unique_ptr<AxisProfileSharingHandler>                   m_sharingHandler {};      ///< pointer to sharing handler

  CanScalingData m_unitPosLinear {};                 ///< unit position linear
  CanScalingData m_unitPosRotary {};                 ///< unit position rotary
  CanScalingData m_unitVelLinear {};                 ///< unit velocity linear
  CanScalingData m_unitVelRotary {};                 ///< unit velocity rotary
  CanScalingData m_unitAccLinear {};                 ///< unit acceleration linear
  CanScalingData m_unitAccRotary {};                 ///< unit acceleration rotary
  TorqueScalingData m_unitTorqueLinear {};           ///< unit torque linear
  TorqueScalingData m_unitTorqueRotary {};           ///< unit torque rotary
  TorqueScalingData m_unitTorquePercent {};          ///< unit torque percent

  CanScalingData m_unitPosUndefine {};               ///< unit position undefined
  CanScalingData m_unitVelUndefine {};               ///< unit velocity undefined
  CanScalingData m_unitAccUndefine {};               ///< unit acceleration undefined
  TorqueScalingData m_unitTorqueUndefine {};         ///< unit torque undefined
  TorqueScalingData m_unitTorqueErrorMath {};        ///< unit torque error math
};
