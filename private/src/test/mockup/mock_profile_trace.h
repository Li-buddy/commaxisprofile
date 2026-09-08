#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_profile_trace.h
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
//! of the axisprofile trace helper class.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/profile_trace.h"


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

  //! @class MockAxisProfileTrace
  //! Mockup of the axisprofile trace helper class.
  class MockAxisProfileTrace : public comm::axisprofile::ProfileTrace
  {
  public:
    //! @brief C'tor
    MockAxisProfileTrace() = default;

    //! @brief Destructor.
    virtual ~MockAxisProfileTrace() noexcept override = default;


    comm::axisprofile::AxisProfileResult setConstantProfileData(const std::string& /*profileName*/, const comm::axisprofile::AxisProfileType /*type*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult updateDeviceAddress(const std::uint32_t /*deviceAddress*/) { return m_returnValue; }
    const char* createTraceMsg(const comm::axisprofile::AxisProfileResult /*errCode = comm::axisprofile::AxisProfileResult::STS_OK*/,
                               const char* /*addInfo1 = nullptr*/, const char* /*addInfo2 = nullptr*/, const char* /*addInfo3 = nullptr*/) override { return ""; }
    const char* createTraceMsg(const char* /*messageText*/) override { return ""; }

    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions
  };
}
