#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_boot_manager.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and implements a mockup
//! of the boot manager.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "configuration/i_boot_mgr.h"

// ---------------------------------------------------------------------------
// Forward references
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
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockBootManager
  //! @brief This class implements a mockup of the boot manager.
  class MockBootManager : public comm::axisprofile::IBootMgr
  {
  public: // methods

    //! @brief Constructor
    MockBootManager() = default;
    //! @brief Destructor
    virtual ~MockBootManager() = default;

    comm::axisprofile::AxisProfileResult init(comm::axisprofile::IAxisProfileComponent* /*component*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult bootSystem() override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult stop() override { return m_returnValue; }
    void abortBooting() override {}
    void getBootingState(std::uint32_t& /*actStep*/, std::uint32_t& /*maxSteps*/, std::string& /*txt*/) const final {}
    double getMaxBootingTimeout() const override { return m_maxBootingTimeout; }
    bool isInitBooting() { return false; };

    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

    //! @brief This method can be used by tests to change maximum booting timeout value
    //! @param[in]    timeout   New maximum booting timeout
    void setMaxBootingTimeout(double timeout) { m_maxBootingTimeout = timeout; }

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions
    double m_maxBootingTimeout {comm::axisprofile::STD_CYCLE_TIME};                                      //!< Maximum timeout, default by one cycle
  };
}
