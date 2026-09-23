#pragma once

#include "stdafx.h"

class TestCanopenJobSetOpMode : public xTST_Test2
{
public:
  TestCanopenJobSetOpMode() = default;
  ~TestCanopenJobSetOpMode() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Canopen.cmdJobs.SetOpMode");
  TEST_DECLARE_DESCRIPTION("Unit test of the CANopen set-operation-mode job.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LLH);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  HRESULT doTest() final;
};
