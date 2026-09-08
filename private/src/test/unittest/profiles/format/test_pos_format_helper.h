#pragma once

#include "stdafx.h"
#include "profiles/format/pos_format_helper_mod.h"
#include "profiles/format/pos_format_helper_abs.h"
#include <functional>


//
// test class
//
class TestPosFormatHelper : public xTST_Test2
{
public: // types

  /// @brief type for the generator functions for the input position
  /// input is the time (and some generic parameters) and output the generated position.
  /// returns true as long as the test case runs
  using LinPosGenerator = std::function<bool(double time, double a, double b, double c, double& pos)>;

  /// @brief test data struct
  struct TestCase
  {
    const wchar_t* m_name {};                   ///< name of the test case
    LinPosGenerator m_generator;                ///< generator function
    bool m_modulo {};                           ///< whether the axis format is modulo 
    double m_a {};                              ///< first parameter for the generator function
    double m_b {};                              ///< second parameter for the generator function
    double m_c {};                              ///< third parameter for the generator function
    bool m_expSuccess {true};                   ///< expecte success over the whole test?
    bool m_plot {false};                        ///< do some plots
  };

public: // methods

  /// @brief Constructor
  TestPosFormatHelper() = default;
  /// @brief Destructor
  virtual ~TestPosFormatHelper() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Format.PosFormatHelper ");
  TEST_DECLARE_DESCRIPTION("Unittest of the position format helper class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLJ);

  HRESULT doTest() final;

private: // methods

  /// @brief Test function for calcModulo
  /// @return S_OK, when the test was successful
  HRESULT runTest(const TestCase& test);

private: // data

  static const double DEFAULT_MODULO;                 ///< default modulo range 
  static const double DEFAULT_LIN_MAX;                ///< default max value of linear position
  static const double DEFAULT_LIN_MIN;                ///< default min value of linear position
  static const double DEFAULT_TIME_INCREMENT;         ///< default system cycle time

  static const std::vector<TestCase> s_testCases;     ///< test data
  static const LinPosGenerator s_linear;              ///< generator for linear interpolation
  static const LinPosGenerator s_linearTurn;          ///< generator for linear interpolation with direction change
  static const LinPosGenerator s_increaseVel;         ///< generator for quadratic interpolation (pos = (time/c)² * b + a)
};
