#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_scheduler.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a mockup
//! of the scheduler interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "common/scheduler/i_scheduler2.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockScheduler
  //! Helper class to simulate the scheduler in test
  class MockScheduler : public common::scheduler::IScheduler2
  {
  public:
    MockScheduler() = default;
    virtual ~MockScheduler() = default;
    //! @brief mock registerCallableFactory
    //! @return SchedStatus
    common::scheduler::SchedStatus registerCallableFactory(const std::shared_ptr<common::scheduler::ICallableFactory>& /*factory*/, const std::string& /*name*/) override { return m_returnResult; };
    common::scheduler::SchedStatus registerCallableFactory(const std::shared_ptr<common::scheduler::ICallableFactory2>& /*factory*/, const std::string& /*name*/) override { return m_returnResult; };
    //! @brief mock unregisterCallableFactory
    //! @return SchedStatus    
    common::scheduler::SchedStatus unregisterCallableFactory(const std::shared_ptr<common::scheduler::ICallableFactory>& /*factory*/) override { return m_returnResult; };
    common::scheduler::SchedStatus unregisterCallableFactory(const std::shared_ptr<common::scheduler::ICallableFactory2>& /*factory*/) override { return m_returnResult; };
    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(common::scheduler::SchedStatus result) { m_returnResult = result; }

  private:
    common::scheduler::SchedStatus m_returnResult = common::scheduler::SchedStatus::SCHED_S_OK;   //!< Result of member functions
  };
}


