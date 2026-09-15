
#include "helper/result_helper.h"
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_profile_trace.h"
#include "test_job_obj.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestJobObj);
REGISTER_TEST_TO_GROUP(TestJobObj, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestJobObj, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestJobObj, JobTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestJobObj::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestJobObj");
  log(L"\n----------------------------------------------------------------\n");

  res |= testNewRequest();

  return res;
}

HRESULT TestJobObj::doEnd()
{
  deleteThread();
  return S_OK;
}

HRESULT TestJobObj::testNewRequest()
{
  // Type for new request testing
  struct NewReqTestDataT
  {
    std::wstring m_testName;      // Test name

    JobState oldState;            // Old state before test case
    JobState oldStateGetErr;      // Old state latched error
    JobReq newReq;                // New request

    AprResult retValueSetArg;     // Setting return value of setArgument
    AprResult retValueEnterAct;   // Setting return value of onJobEnterActivating
    AprResult retValueActing;     // Setting return value of onJobActivating, STS_OK will switch state, STS_PENDING will keep in state
    AprResult retValueEnterAbrt;  // Setting return value of onJobEnterAborting
    AprResult retValueAborting;   // Setting return value of onJobAborting, STS_OK will switch state, STS_PENDING will keep in state

    JobState expState;            // Expected state
    AprResult expReqResult;       // Expected return value of newRequest
  };

  // Test data
  std::vector<NewReqTestDataT> testCase {
    // testName                                       oldState                    oldStateGetErr          newRequest            retValueSetArg      retValueEnterAct    retValueActing          retValueEnterAbrt   retValueAborting        expState                    expReqResult     
    {L"1.1 ReqStart at Idle, argument invalid",       JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::ERR_NOK, AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::ERR_INVALID_ARGUMENT},
    {L"1.1 ReqStart at Idle, reach Activating",       JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_PENDING, AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVATING, AprResult::STS_OK},
    {L"1.2 ReqStart at Idle, reach Active",           JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVE,     AprResult::STS_OK},
    {L"1.3 ReqStart at Idle, reach Error 1",          JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::ERR_NOK, AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"1.4 ReqStart at Idle, reach Error 2",          JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::ERR_NOK,     AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"1.5 ReqStart at Activating, keep Activating",  JobState::STATE_ACTIVATING, JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_PENDING, AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVATING, AprResult::STS_OK},
    {L"1.6 ReqStart at Active, keep Active",          JobState::STATE_ACTIVE,     JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVE,     AprResult::STS_OK},
    {L"1.7 ReqStart at Aborting, error",              JobState::STATE_ABORTING,   JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_PENDING, JobState::STATE_ABORTING,   AprResult::ERR_INVALID_STATE},
    {L"1.8 ReqStart at Error, error",                 JobState::STATE_ERROR,      JobState::STATE_IDLE,   JobReq::REQ_START,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::ERR_INVALID_STATE},

    {L"2.1 ReqAbort at Idle, error",                  JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::ERR_INVALID_STATE},
    {L"2.2 ReqAbort at Activating, reach Idle",       JobState::STATE_ACTIVATING, JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::STS_OK},
    {L"2.3 ReqAbort at Activating, reach Error 1",    JobState::STATE_ACTIVATING, JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::ERR_NOK, AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"2.4 ReqAbort at Activating, reach Error 2",    JobState::STATE_ACTIVATING, JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::ERR_NOK,     JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"2.5 ReqAbort at Active, reach Idle",           JobState::STATE_ACTIVE,     JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::STS_OK},
    {L"2.5 ReqAbort at Active, reach Error 1",        JobState::STATE_ACTIVE,     JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::ERR_NOK, AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"2.6 ReqAbort at Active, reach Error 2",        JobState::STATE_ACTIVE,     JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::ERR_NOK,     JobState::STATE_ERROR,      AprResult::STS_OK},
    {L"2.7 ReqAbort at Aborting, keep Aborting",      JobState::STATE_ABORTING,   JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_PENDING, JobState::STATE_ABORTING,   AprResult::STS_OK},
    {L"2.8 ReqAbort at Error, error",                 JobState::STATE_ERROR,      JobState::STATE_IDLE,   JobReq::REQ_ABORT,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ERROR,      AprResult::ERR_INVALID_STATE},

    {L"3.1 ReqReset at Idle, nothing happen",         JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::STS_OK},
    {L"3.2 ReqReset at Activating, error",            JobState::STATE_ACTIVATING, JobState::STATE_IDLE,   JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_PENDING, AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVATING, AprResult::ERR_INVALID_STATE},
    {L"3.3 ReqReset at Active, error",                JobState::STATE_ACTIVE,     JobState::STATE_IDLE,   JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_ACTIVE,     AprResult::ERR_INVALID_STATE},
    {L"3.4 ReqReset at Aborting, error",              JobState::STATE_ABORTING,   JobState::STATE_IDLE,   JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_PENDING, JobState::STATE_ABORTING,   AprResult::ERR_INVALID_STATE},
    {L"3.5 ReqReset at Error, got error in active",   JobState::STATE_ERROR,      JobState::STATE_ACTIVE, JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::STS_OK},
    {L"3.6 ReqReset at Error, got error in abort",    JobState::STATE_ERROR,      JobState::STATE_IDLE,   JobReq::REQ_RESET,    AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::STS_OK},

    {L"4.1 ErrReset, error",                          JobState::STATE_IDLE,       JobState::STATE_IDLE,   JobReq::REQ_UNKNOWN,  AprResult::STS_OK,  AprResult::STS_OK,  AprResult::STS_OK,      AprResult::STS_OK,  AprResult::STS_OK,      JobState::STATE_IDLE,       AprResult::ERR_INVALID_ARGUMENT},
  };

  for (const auto& it : testCase)
  {
    MockFieldbusAdapterSoE adapter {};
    MockAxisProfileTrace trace {};
    ProfileBasicInfo profileInfo {AxisProfileType::SERVODRIVE_OVER_ETHERCAT, "dummy", 0, 0, &adapter, trace};
    m_jobObj.reset(new MyJobObj());
    
    createThread();

    log(L"\n<BLUE>Test of %s ...</BLUE>", it.m_testName.c_str());

    m_threadTickCnt = 5; // makes test faster
    static_cast<JobObj*>(m_jobObj.get())->m_state = it.oldState;
    static_cast<JobObj*>(m_jobObj.get())->m_oldStateGetErr = it.oldStateGetErr;
    m_jobObj->m_retSetArgument = it.retValueSetArg;
    m_jobObj->m_retEnterActive = it.retValueEnterAct;
    m_jobObj->m_retActivating = it.retValueActing;
    m_jobObj->m_retEnterAbort = it.retValueEnterAbrt;
    m_jobObj->m_retAborting = it.retValueAborting;

    log(L"\n  OldState = %ls, Request = %ls, expState = %ls",
        UTF8TOWIDE(jobStateToStr(it.oldState)),
        UTF8TOWIDE(jobReqToStr(it.newReq)),
        UTF8TOWIDE(jobStateToStr(it.expState)));

    log(L"\n  Create new request on statemachine...");
    std::string errTxt {};
    auto result = m_jobObj->setNewState(it.newReq, errTxt, nullptr);
    LOG_TEST_RESULT(it.expReqResult, L"\nUnexpected return value", result);

    m_sema.notify();
    std::this_thread::sleep_for(std::chrono::milliseconds((20) * CYCLE_TIME)); // sleep for execution complete

    log(L"\n  Check state of the statemachine...");
    auto actState = m_jobObj->getState();
    LOG_TEST_STATE(it.expState, L"\nUnexpected state", actState, jobStateToStr);

    log(L"\n  Check job data...");
    auto jobData = m_jobObj->getJobData();
    LOG_TEST_RESULT_PTR(jobData, L"\nUnexpected job data", nullptr);

    log(L"\n  Check job const data...");
    const auto jobConstData = m_jobObj->getJobData();
    LOG_TEST_RESULT_PTR(jobConstData, L"\nUnexpected job data", nullptr);

    deleteThread();
  }

  return S_OK;
}

void TestJobObj::funcThread()
{
  while (!m_abort.load())
  {
    if (m_threadTickCnt > 0)
    {
      m_threadTickCnt--;
    }
    if (m_threadTickCnt == 0)
    {
      m_sema.wait();
    }

    if (m_jobObj != nullptr)
    {
      m_jobObj->mainCycle();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_TIME));
  }
  return;
}

void TestJobObj::createThread()
{
  deleteThread();
  m_threadTickCnt = 0;
  m_abort.store(false);
  log(L"\n<GREY>  Create test thread...</GREY>");
  m_thread = new std::thread(&TestJobObj::funcThread, this);
  log(L"\n<GREY>  Sleep for a while until thread sleep down...</GREY>");
  std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_TIME * 5));
}

void TestJobObj::deleteThread()
{
  m_abort.store(true);
  m_sema.notify();
  if (m_thread)
  {
    log(L"\n<GREY>  Delete exiting test thread...</GREY>");
    m_thread->join();
    delete m_thread;
    m_thread = nullptr;
  }
  m_abort.store(false);
}
