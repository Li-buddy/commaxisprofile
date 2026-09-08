#pragma once

#include "stdafx.h"
#include "profiles/category/encoder/encoder_category_adapter_coe.h"
#include "profiles/data_sharing/sharing_handler.h"
#include <memory>

//
// test class
//
class TestAxisProfileProbeEncoder : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestAxisProfileProbeEncoder() = default;
  /// @brief Destructor
  virtual ~TestAxisProfileProbeEncoder() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Probe.Encoder");
  TEST_DECLARE_DESCRIPTION("Test of AxisProfileProbeEncoder class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;

private: // methods

  std::shared_ptr<comm::axisprofile::ProfileBasicInfo> m_profileInfo {};             ///< the basic profile information
  std::shared_ptr<comm::axisprofile::AcknowledgeTelegram> m_telegramAT {};           ///< Pointer to the Acknowledge Telegram (AT)
  std::shared_ptr<comm::axisprofile::MasterDataTelegram> m_telegramMDT {};           ///< Pointer to the Master Data Telegram (MDT)
  std::shared_ptr<comm::axisprofile::AxisProfileSharingHandler> m_sharingHandler {}; ///< sharing handler
  std::unique_ptr<comm::axisprofile::EncoderCategoryAdapterCoE> m_category {};       ///< the category object
};
