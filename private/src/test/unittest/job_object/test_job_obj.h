#pragma once

#include "common/semaphore.h"
#include "job_object/job_obj.h"
#include "stdafx.h"
#include <atomic>
#include <memory>
#include <thread>

///< @brief Helper type for easy coding
using AprResult = comm::axisprofile::AxisProfileResult;
namespace test
{
  class MyJobObj;
}

//
// test class
//
class TestJobObj : public xTST_Test2
{
public: // methods

  /// @brief Constructor
  TestJobObj() = default;
  /// @brief Destructor
  virtual ~TestJobObj() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Jobs.JobObj");
  TEST_DECLARE_DESCRIPTION("Test of job object class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // methods

  /// @brief Test newRequest of statemachine
  /// @return  S_OK, when the test was successful
  HRESULT testNewRequest();

  /// @brief Function that executes in test thread.
  void funcThread();

  /// @brief This method create a test thread
  void createThread();

  /// @brief This method delete the test thread
  void deleteThread();

  /// @brief Test the base class of job object
  /// @return S_OK, when the test was successful
  HRESULT testJobBase();

private: // data

  std::thread* m_thread {nullptr};          ///< Thread for testing
  std::atomic_bool m_abort {false};         ///< Abort execution of thread
  const std::uint32_t CYCLE_TIME {2};       ///< Dummy cycle time of thread
  std::uint32_t m_threadTickCnt {0};        ///< Thread tick counter
  comm::axisprofile::Semaphore m_sema;      ///< Sleep down the thread
  std::unique_ptr<test::MyJobObj> m_jobObj; ///< Job object for testing
};

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  /// @brief This is class of my job object
  class MyJobObj : public ::comm::axisprofile::JobObj
  {
  public: // methods

    /// @brief Construct
    /// @param[in] profileInfo  
    MyJobObj() {};
    
    /// @brief Destructor
    virtual ~MyJobObj() noexcept override = default;

    AprResult onSetArgument(::comm::axisprofile::IContainer* arg) override
    {
      ::comm::axisprofile::JobObj::onSetArgument(arg);
      return m_retSetArgument;
    }

    AprResult onJobEnterActivating() override
    {
      ::comm::axisprofile::JobObj::onJobEnterActivating();
      return m_retEnterActive;
    }

    AprResult onJobActivating() override
    {
      ::comm::axisprofile::JobObj::onJobActivating();
      return m_retActivating;
    }
    AprResult onJobEnterAborting() override
    {
      ::comm::axisprofile::JobObj::onJobEnterAborting();
      return m_retEnterAbort;
    }
    AprResult onJobAborting() override
    {
      ::comm::axisprofile::JobObj::onJobAborting();
      return m_retAborting;
    }
    void onReset() override
    {
      ::comm::axisprofile::JobObj::onReset();
    }

  public: // data

    AprResult m_retSetArgument {};   ///< Return value of set argument
    AprResult m_retEnterActive {};   ///< Return value of entering active
    AprResult m_retActivating {};    ///< Return value of activating
    AprResult m_retEnterAbort {};    ///< Return value of entering abort
    AprResult m_retAborting {};      ///< Return value of aborting
  };
}
