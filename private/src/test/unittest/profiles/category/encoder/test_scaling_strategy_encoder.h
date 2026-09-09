#pragma once

#include "stdafx.h"
#include "profiles/category/encoder/encoder_category_adapter_coe.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "mockup/mock_axisprofile_mapping_handler.h"

//
// test class
//
class TestScalingStrategyEncoder : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestScalingStrategyEncoder() = default;
  /// @brief Destructor
  virtual ~TestScalingStrategyEncoder() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.ScalingStrategy.Encoder");
  TEST_DECLARE_DESCRIPTION("Test of ScalingStrategyEncoder class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  std::shared_ptr<comm::axisprofile::ProfileBasicInfo> m_profileInfo {};             ///< the basic profile information
  std::shared_ptr<comm::axisprofile::AcknowledgeTelegram> m_telegramAT {};           ///< Pointer to the Acknowledge Telegram (AT)
  std::shared_ptr<comm::axisprofile::MasterDataTelegram> m_telegramMDT {};           ///< Pointer to the Master Data Telegram (MDT)
  std::shared_ptr<comm::axisprofile::AxisProfileSharingHandler> m_sharingHandler {}; ///< sharing handler
  std::unique_ptr<comm::axisprofile::EncoderCategoryAdapterCoE> m_category {};       ///< the category object
  std::shared_ptr<test::MockAxisProfileMappingHandler> m_mapping {};                 ///< the mock mapping handler
};
