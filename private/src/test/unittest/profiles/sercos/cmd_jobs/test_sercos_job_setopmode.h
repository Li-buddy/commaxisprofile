#pragma once

#include "stdafx.h"

class TestSercosJobSetOpMode : public xTST_Test2
{
public:
  TestSercosJobSetOpMode() = default;
  ~TestSercosJobSetOpMode() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.cmdJobs.SetOpMode");
  TEST_DECLARE_DESCRIPTION("Unit test of the Sercos set-operation-mode job and command wiring.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_LST);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;
};
