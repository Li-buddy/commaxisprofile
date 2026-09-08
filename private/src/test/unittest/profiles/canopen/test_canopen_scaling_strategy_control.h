#pragma once
// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/canopen/drive/canopen_scaling_strategy_control.h"
#include "profiles/category/drive/category_adapter_drive.h"
#include "profiles/data_sharing/sharing_handler.h"
#include <vector>
#include <string>
#include <functional>
#include "profiles/canopen/canopen_defs.h"

// Mockups
#include "mockup/external_bundles/mock_datalayer_client.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_fieldbus_adapter_coe.h"

using namespace comm::axisprofile::canopen;
//! @class TestCanopenScalingStrategyControl
//! Test class for the can scaling class.
class TestCanopenScalingStrategyControl : public xTST_Test2
{
  struct TestData
  {
    std::function<HRESULT(const TestData*)> m_testFunctional{};
    comm::axisprofile::AxisProfileScalingStrategyCfg m_scalingStrategyCfg{ };
    comm::axisprofile::AxisProfileResult m_expResult{};     //!< Expected return code of the axis-profile
    const wchar_t* m_testName{};                            //!< name of the test
  };

public:
  explicit TestCanopenScalingStrategyControl();
  ~TestCanopenScalingStrategyControl() noexcept override = default;
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.Control_ScalingStrategy");
  TEST_DECLARE_DESCRIPTION("Unittest of the Scaling strategy control.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd()  final;

private:
  HRESULT testCalcScalingLSBWeights();
  HRESULT testCalcPosScalingLSBWeight(const TestData* testData);
  HRESULT testCalcVelScalingLSBWeight(const TestData* testData) ;
  HRESULT testCalcAccScalingLSBWeight(const TestData* testData) ;
  HRESULT testCalcTorqueScalingLSBWeight() ;
  HRESULT testLoadLSBWeightsMethods();
  HRESULT testFor_setter();
  HRESULT testFor_supportAxis();

private:
  std::vector<TestData> m_vecTestData{};
  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
  std::uint32_t deviceAddress{ 1 };

  comm::axisprofile::ProfileBasicInfo           m_profileBasicInfo;               ///< basic information of the profile
  std::shared_ptr<test::MockAxisProfileTrace>   m_mockProfileTrace{};
  std::shared_ptr<test::MockFieldbusAdapterCoE> m_mockFieldbusAdapter{};
  std::shared_ptr<test::MockDatalayerClient>    m_mockClient{};
  std::shared_ptr<test::MockEthercatMasterSDORequestNodeCoE> m_sdoRequestNodeMockCoE{};
  std::unique_ptr<ScalingStrategyCoEControl>    m_testObject{};
  std::unique_ptr<comm::axisprofile::CategoryAdapterDrive>       m_mockCategory {};
  std::unique_ptr<comm::axisprofile::AxisProfileSharingHandler>    m_psharingHandler {};      ///< pointer to sharing handler
};
