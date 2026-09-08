#pragma once

#include "stdafx.h"
#include "profiles/category/encoder/encoder_category_adapter_coe.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "mockup/mock_fieldbus_adapter_coe.h"
#include <memory>

//
// test class
//
class TestAxisProfileCyclicDataEncoder : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestAxisProfileCyclicDataEncoder() = default;
  /// @brief Destructor
  virtual ~TestAxisProfileCyclicDataEncoder() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.CyclicData.Encoder");
  TEST_DECLARE_DESCRIPTION("Test of AxisProfileCyclicDataEncoder class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  std::shared_ptr<comm::axisprofile::ProfileBasicInfo> m_profileInfo {};             ///< the basic profile information  
  std::shared_ptr<comm::axisprofile::AcknowledgeTelegram> m_telegramAT {};           ///< Pointer to the Acknowledge Telegram (AT)
  std::shared_ptr<comm::axisprofile::MasterDataTelegram> m_telegramMDT {};           ///< Pointer to the Master Data Telegram (MDT)
  std::shared_ptr<comm::axisprofile::AxisProfileSharingHandler> m_sharingHandler {}; ///< sharing handler
  std::unique_ptr<comm::axisprofile::EncoderCategoryAdapterCoE> m_category {};       ///< the category object
  std::unique_ptr<test::MockFieldbusAdapterCoE> m_mockAdapter {};                    ///< the mock fieldbus adapter
};
