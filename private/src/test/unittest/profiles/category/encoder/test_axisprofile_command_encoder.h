#pragma once

#include "stdafx.h"
#include "profiles/category/encoder/encoder_category_adapter_coe.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "profiles/aux_job/aux_job_handler.h"
#include <memory>

//
// test class
//
class TestAxisProfileCommandEncoder : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestAxisProfileCommandEncoder() = default;
  /// @brief Destructor
  virtual ~TestAxisProfileCommandEncoder() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Command.Encoder");
  TEST_DECLARE_DESCRIPTION("Test of AxisProfileCommandEncoder class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;

private: // methods

  std::shared_ptr<comm::axisprofile::ProfileBasicInfo> m_profileInfo {};             ///< the basic profile information
  std::unique_ptr<comm::axisprofile::EncoderCategoryAdapterCoE> m_category {};       ///< the category object
  std::shared_ptr<comm::axisprofile::AcknowledgeTelegram> m_telegramAT {};           ///< Pointer to the Acknowledge Telegram (AT)
  std::shared_ptr<comm::axisprofile::MasterDataTelegram> m_telegramMDT {};           ///< Pointer to the Master Data Telegram (MDT)
  std::shared_ptr<comm::axisprofile::AxisProfileSharingHandler> m_sharingHandler {}; ///< sharing handler
  std::shared_ptr<comm::axisprofile::AuxJobHandler> m_auxHandler {};                 ///< aux handler
};
