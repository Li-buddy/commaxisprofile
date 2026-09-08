#pragma once

#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "profiles/axisprofile_param_access.h"
#include "stdafx.h"

//
// test class
//
class TestPorfileParamAccess : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestPorfileParamAccess() = default;
  /// @brief Destructor
  virtual ~TestPorfileParamAccess() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Parameter.Access");
  TEST_DECLARE_DESCRIPTION("Test of parameter access class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;

private: // methods

  /// @brief Test error cases
  /// @return  S_OK, when the test was successful
  HRESULT testErrorCase();

private: // data

  const char* m_profile {"test"};             ///< profile name
  std::uint16_t m_devAdd {1001};              ///< device address
  std::uint8_t m_devNum {0};                  ///< device number
  test::MockFieldbusAdapterSoE m_adapter {};  ///< mockup fieldbus adapter
  test::MockAxisProfileTrace m_trace {};      ///< mockup trace
};
