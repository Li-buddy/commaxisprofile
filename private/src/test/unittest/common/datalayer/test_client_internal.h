#pragma once

#include "stdafx.h"
#include <memory>
#include <map>
#include "comm/datalayer/datalayer_system.h"
#include "mockup/mock_background_thread.h"
#include "mock_nodes_helper.h"

//
// test class
//
class TestClientInternal : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestClientInternal() = default;
  /// @brief Destructor
  virtual ~TestClientInternal() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Common.ClientInternal");
  TEST_DECLARE_DESCRIPTION("Test of wrapped data client");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Test create function
  /// @param[in]  enableBkThread    Enable background thread behavior or not
  /// @return  S_OK, when the test was successful
  HRESULT doFunctionTestCase(bool enableBkThread);

  /// @brief Test error cases
  /// @return  S_OK, when the test was successful
  HRESULT doTestErrorCase();

private: // data

  comm::datalayer::DatalayerSystem m_dlSystem {};             ///< The datalayer system for testing
  std::unique_ptr<test::MockNodesHelper> m_mockNodes{};       ///< Mockup for testing
  std::unique_ptr<test::MockBackgroundThread2> m_bkThread {}; ///< The background mockup
};
