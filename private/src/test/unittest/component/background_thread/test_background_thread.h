#pragma once

#include "stdafx.h"
#include <memory>

#include "component/background_thread/i_background_thread.h"

//
// test class
//
class TestBackgroundThread : public xTST_Test2
{
public: // methods

  /// @brief Constructor
  TestBackgroundThread() = default;
  /// @brief Destructor
  virtual ~TestBackgroundThread() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.BackgroundThread");
  TEST_DECLARE_DESCRIPTION("Test of background thread");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private: // methods

  /// @brief Simple test the class
  /// @return  S_OK, when the test was successful
  HRESULT doSimpleTest();

  /// @brief Callback test the class
  /// @return  S_OK, when the test was successful
  HRESULT doCallbackTest();

  /// @brief Callback function
  static void callbackFun();

private: // data

  const std::uint32_t CYCLE_TIME {5};   ///< Dummy cycle time of thread
  const std::uint64_t SLEEP_TIME {50};  ///< Sleep time in test
  std::unique_ptr<comm::axisprofile::IBackgroundThread> m_bgThread {};  ///< Pointer against background thread
};
