#include "test_aux_job_handler.h"
#include "profiles/profile_trace.h"


using namespace comm::axisprofile;

REGISTER_TEST(TestAuxJobHandler);
REGISTER_TEST_TO_GROUP(TestAuxJobHandler, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAuxJobHandler, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAuxJobHandler, Profiles);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
TestAuxJobHandler::TestAuxJobHandler()
{}


TestAuxJobHandler::~TestAuxJobHandler(void)
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAuxJobHandler::doTest()
{
  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the AuxJobHandler class");
  log(L"\n----------------------------------------------------------------\n");

  testrunAllAuxJobs();

  return S_OK;
}


HRESULT TestAuxJobHandler::testrunAllAuxJobs()
{
  log(L"\n\tRun testcase: RunAllAuxJobs");

  HRESULT hresult = S_OK;
  AuxJobHandler testAuxJobHandler;
  auxJobs job;
  job.m_jobCyclic = [this] ()->AxisProfileResult
  {
    return AxisProfileResult::STS_OK;
  };

  testAuxJobHandler.addAuxJob(job);
  testAuxJobHandler.runAllAuxJobs();
  log(L"... <DARK_GREEN>OK</DARK_GREEN>");

  return hresult;
}
