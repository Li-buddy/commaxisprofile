#include "test_pos_format_helper.h"
#include "helper/result_helper.h"
#include "motion/test/dynamics_helper.h"

using namespace comm::axisprofile;

REGISTER_TEST(TestPosFormatHelper);
REGISTER_TEST_TO_GROUP(TestPosFormatHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestPosFormatHelper, 2_UnitTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

const double TestPosFormatHelper::DEFAULT_MODULO = 360.0;
const double TestPosFormatHelper::DEFAULT_LIN_MAX = 999;
const double TestPosFormatHelper::DEFAULT_LIN_MIN = -1000;
const double TestPosFormatHelper::DEFAULT_TIME_INCREMENT = 0.02;

// linear interpolation (from a° to b°) over c seconds
const TestPosFormatHelper::LinPosGenerator TestPosFormatHelper::s_linear{
  [](double time, double a, double b, double c, double& pos)
  {
    pos = (b - a) / c * time + a;
    return time <= c;
  }
};

// linear interpolation with direction change (from a° to b° to a°) over c seconds
const TestPosFormatHelper::LinPosGenerator TestPosFormatHelper::s_linearTurn{
  [](double time, double a, double b, double c, double& pos)
  {
    pos = time < c * 0.5 ? (b - a) / (c * 0.5) * time + a : (b - a) / (c * 0.5) * (c - time) + a;
    return time <= c;
  }
};

// quadratic interpolation start with velocity 0.0 at position a and end at position b over c seconds
const TestPosFormatHelper::LinPosGenerator TestPosFormatHelper::s_increaseVel{
  [](double time, double a, double b, double c, double& pos)
  {
    pos = (time / c) * (time / c) * b + a;
    return time <= c;
  }
};

const std::vector<TestPosFormatHelper::TestCase> TestPosFormatHelper::s_testCases = {
  // name                                                                                   generator function    modulo?      a     b       c     expSuccess?  plot?
  { L"1 Modulo axis: linear increasing (from 0 deg to 10000 deg) over 200 sec",             s_linear,             true,        0,    10000,  200,  true,        true},
  { L"2 Modulo axis: linear increasing (from 0 deg to -10000 deg) over 10 sec",             s_linear,             true,        0,    -10000, 10,   true,        true},
  { L"3 Modulo axis: linear increasing (from 0 deg to 5000 deg and back) over 200 sec",     s_linearTurn,         true,        0,    5000,   200,  true,        true},
  { L"4 Modulo axis: quadratic increasing (from 0 deg to 8000 deg) over 10 sec",            s_increaseVel,        true,        0,    8000,   10,   true,        true},
  { L"5 Absolute axis: linear increasing (from 0 deg to 10000 deg) over 200 sec",           s_linear,             false,       0,    10000,  200,  true,        true},
  { L"6 Absolute axis: linear increasing (from 0 deg to -10000 deg) over 10 sec",           s_linear,             false,       0,    -10000, 10,   true,        true},
  { L"7 Absolute axis: linear increasing (from 0 deg to 5000 deg and back) over 200 sec",   s_linearTurn,         false,       0,    5000,   200,  true,        true},
  { L"8 Absolute axis: quadratic increasing (from 0 deg to 8000 deg) over 10 sec",          s_increaseVel,        false,       0,    8000,   10,   true,        true},
};

HRESULT TestPosFormatHelper::doTest()
{
  HRESULT result = S_OK;

  log( L"\n\n----------------------------------------------------------------" );
  log( L"\n  Test the Modulo and Absolute class" );
  log( L"\n----------------------------------------------------------------\n" );

  for (const auto& test : s_testCases)
  {
    log(L"\n\n<BLUE>Running test case %ls</BLUE>", test.m_name);
    result |= runTest(test);
  }
  return result;
}

HRESULT TestPosFormatHelper::runTest(const TestCase& test)
{
  std::vector<double> inPosVec;
  std::vector<double> inPosModuloVec;
  std::vector<double> linearPosVec;
  std::vector<double> feedbackPosVec;
  std::vector<double> adjustPosVec;

  [[maybe_unused]] std::uint64_t time{};
  bool result = true;
  double pos;
  if (test.m_modulo)
  {
    format::PosFormatHelperMod modulo;
    modulo.init(DEFAULT_MODULO, DEFAULT_LIN_MAX, DEFAULT_LIN_MIN);
    while (test.m_generator(DEFAULT_TIME_INCREMENT * time, test.m_a, test.m_b, test.m_c, pos))
    {
      auto moduloPos = modulo.calcModulo(pos, DEFAULT_MODULO);
      format::PosFormatHelperMod::ModValuePair valuePair;
      double feedbackPos;
      result &= modulo.convertModToLin(moduloPos, valuePair);
      double linPos = valuePair.turns * DEFAULT_MODULO + valuePair.value;
      feedbackPos = modulo.convertLinToMod(linPos);
      inPosVec.push_back(pos);
      inPosModuloVec.push_back(moduloPos);
      linearPosVec.push_back(linPos);
      feedbackPosVec.push_back(feedbackPos);
      ++time;
    }
    log(L"\nCheck, if we expect an error...");
    LOG_TEST_RESULT(result, L"\nUnexpected error result!", test.m_expSuccess);
    log(L"\ncheck result: modulo input position must be equal to the feedback modulo position...");
    test::DynamicsHelper hlp(this);
    auto res = hlp.compareArrays(inPosModuloVec, feedbackPosVec, test.m_name, L"position", DEFAULT_TIME_INCREMENT, 0, 1E-6);
    LOG_TEST_RESULT(res, L"\nExpected same values in array!", S_OK);
    if ((res != S_OK) || test.m_plot)
    {
      log(L"\n");
      std::vector<const std::vector<double>*> values{&inPosVec, &inPosModuloVec, &linearPosVec};
      hlp.plotArray(values, L"PositionValues", DEFAULT_TIME_INCREMENT);
      log(L"\n(violett: original linear values; cyan: modulo values; red: linearized values)");
    }
  }
  else
  {
    format::PosFormatHelperAbs absolute;
    absolute.init(DEFAULT_LIN_MAX, DEFAULT_LIN_MIN);
    while (test.m_generator( DEFAULT_TIME_INCREMENT * time, test.m_a, test.m_b, test.m_c, pos))
    {
      auto adjustPos = absolute.adjustForOverflow(pos);
      double feedbackPos;
      feedbackPos = absolute.restoreLinValue(adjustPos);
      inPosVec.push_back(pos);
      adjustPosVec.push_back(adjustPos);
      feedbackPosVec.push_back(feedbackPos);
      ++time;
    }
    log(L"\nCheck, if we expect an error...");
    LOG_TEST_RESULT(result, L"\nUnexpected error result!", test.m_expSuccess);
    log(L"\ncheck result: input position must be equal to the feedback position...");
    test::DynamicsHelper hlp(this);
    auto res = hlp.compareArrays(inPosVec, feedbackPosVec, test.m_name, L"position", DEFAULT_TIME_INCREMENT, 0, 1E-6);
    LOG_TEST_RESULT(res, L"\nExpected same values in array!", S_OK);
    if ((res != S_OK) || test.m_plot)
    {
      log(L"\n");
      std::vector<const std::vector<double>*> values{&inPosVec, &adjustPosVec, &feedbackPosVec};
      hlp.plotArray(values, L"PositionValues", DEFAULT_TIME_INCREMENT);
      log(L"\n(violett: original output values; cyan: adjust output values; red: feedback values)");
    }
  }
  return S_OK;
}

