#include "helper/result_helper.h"
#include "job_object/job_data_type.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_job_obj.h"
#include "mockup/mock_profile_trace.h"
#include "profiles/job_mgr/job_mgr.h"
#include "test_job_mgr.h"
#include <memory>

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestJobMgr);
REGISTER_TEST_TO_GROUP(TestJobMgr, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestJobMgr, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestJobMgr, JobTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test
DECLARE_NAMED_TYPE(MyTestDouble, double);

HRESULT TestJobMgr::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestJobMgr");
  log(L"\n----------------------------------------------------------------\n");

  res |= doTestCase();

  return res;
}

HRESULT TestJobMgr::doTestCase()
{
  AxisProfileResult result {AxisProfileResult::ERR_NOK};
  test::MockFieldbusAdapterSoE adapter {};
  test::MockAxisProfileTrace trace {};
  ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT,
                                "dummy", 0, 0, &adapter, trace};

  std::string goodJobID = "MyJob";
  std::string unknownJobID = "UnknownJob";
  std::unique_ptr<IJobObj> goodJob = std::make_unique<MockJobObj>(goodJobID);
  std::unique_ptr<IJobObj> emptyJob; // holds empty pointer by default
  std::unique_ptr<IJobObj> emptyNameJob = std::make_unique<MockJobObj>("");

  log(L"\nCreate a new job manager object...");
  std::unique_ptr<IJobMgr> jobMgr;
  jobMgr.reset(new JobMgr(profileInfo));

  log(L"\nTest registerJob method...");

  log(L"\n  Register an empty job...");
  result = jobMgr->registerJob(emptyJob);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n  Register an empty name job...");
  result = jobMgr->registerJob(emptyNameJob);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_JOB_EMPTY_NAME);

  log(L"\n  Register a normal job...");
  auto goodJobPtr = goodJob.get(); // catch the pointer, since goodJob will be swapped to nullptr dewing registration
  result = jobMgr->registerJob(goodJob);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\n  Check the registered job...");
  auto job = jobMgr->getJobObj(goodJobID);
  LOG_TEST_RESULT_PTR(job, L"\nGet an unexpected result!", goodJobPtr);

  log(L"\n  Register a normal job twice (duplicated name)...");
  std::unique_ptr<IJobObj> goodJobDuplicatedName = std::make_unique<MockJobObj>(goodJobID);
  result = jobMgr->registerJob(goodJobDuplicatedName);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_JOB_EXIST);

  log(L"\n  Unregister all jobs, should always success...");
  jobMgr->unregisterAllJobs();

  log(L"\nTest sendJobCmd method...");
  log(L"\n  Recreate a new job manager object and job objects...");
  jobMgr.reset(new JobMgr(profileInfo));
  goodJob = std::make_unique<MockJobObj>(goodJobID);
  goodJobPtr = goodJob.get();

  log(L"\n  Register a normal job...");
  jobMgr->registerJob(goodJob);

  log(L"\n  Check the registered job...");
  job = jobMgr->getJobObj(goodJobID);
  LOG_TEST_RESULT_PTR(job, L"\nGet an unexpected result!", goodJobPtr);

  log(L"\n  Request a non-exist job...");
  result = jobMgr->sendJobCmd(unknownJobID, CmdReq::REQ_UNKNOWN);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_JOB_NOT_EXIST);

  log(L"\n  Request a wrong command...");
  static_cast<MockJobObj*>(goodJobPtr)->m_retValue = AxisProfileResult::ERR_INVALID_ARGUMENT;
  result = jobMgr->sendJobCmd(goodJobID, CmdReq::REQ_UNKNOWN);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::ERR_INVALID_ARGUMENT);

  log(L"\n  Request a normal command...");
  MyTestDouble myAgr(0.0);
  static_cast<MockJobObj*>(goodJobPtr)->m_retValue = AxisProfileResult::STS_PENDING;
  result = jobMgr->sendJobCmd(goodJobID, CmdReq::REQ_START, &myAgr);
  LOG_TEST_RESULT(result, L"\nGet an unexpected result!", AxisProfileResult::STS_OK);

  log(L"\nTest getJobState at exiting job...");
  CmdState expState {CmdState::STATE_ACTIVE};
  static_cast<MockJobObj*>(goodJobPtr)->m_objState = expState;
  CmdState actState = jobMgr->getJobState(goodJobID);
  LOG_TEST_STATE(expState, L"\nGet an unexpected state!", actState, cmdStateToString);

  log(L"\nTest getJobState at not exiting job...");
  actState = jobMgr->getJobState(unknownJobID);
  expState = CmdState::STATE_UNKNOWN;
  LOG_TEST_STATE(expState, L"\nGet an unexpected state!", actState, cmdStateToString);

  log(L"\nDummy test on runAllJobs method...");
  static_cast<MockJobObj*>(goodJobPtr)->m_retValue = AxisProfileResult::STS_OK;
  jobMgr->runAllJobs();
  log(L"\nTest with a error return from job...");
  static_cast<MockJobObj*>(goodJobPtr)->m_retValue = AxisProfileResult::ERR_AOX_THREAD;
  jobMgr->runAllJobs();

  jobMgr.reset(nullptr);
  return S_OK;
}
