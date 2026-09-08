#pragma once

/// @file      mock_axisprofile_probe.h
///
/// This file is part of the axisprofile library and implements a mockup for
/// the axis profile probe interface.
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_probe_internal.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

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
  /// @brief This class implements a mockup for the axis profile probe interface.
  class MockAxisProfileProbe : public comm::axisprofile::IAxisProfileProbeInternal
  {
  public: // methods
    /// @brief Constructor
    MockAxisProfileProbe() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileProbe() noexcept override = default;

    const IAxisProfileProbeC* getCInterface() const override { return nullptr; }

    AxisProfileResult probeBaseConfig(ProbeIndex /*probeIdx*/, ProbeBaseConfig /*probeConfig*/, bool /*isAutoActivated*/) override
    {
      return m_returnValue;
    }
        
    AxisProfileResult probeExtConfig(ProbeIndex /*probeIdx*/, ProbeExtConfig /*probeExtendConfig*/, bool /*isStateMonitActivated*/) override
    {
      return m_returnValue;
    }
                                                        
    AxisProfileResult probeStatusCtrl(ProbeIndex /*probeIdx*/, bool /*enable*/) override
    {
      return m_returnValue;
    }
      
    AxisProfileResult getProbeActualValuesRT(ProbeIndex /*probeIdx*/, ProbeActualValues* /*actualValueProbe*/)override
    {
      return m_returnValue;
    }
    
    AxisProfileResult onlineChangeParams(ProbeIndex /*probeIdx*/, ProbeUpdateMeasurement /*updateValue*/) override
    {
      return m_returnValue;
    }
    
    AxisProfileResult getProbeValues(ProbeIndex /*probeIdx*/, ProbeBaseConfig* /*probeConfig*/, ProbeExtConfig* /*probeExtendConfig*/) override
    {
      return m_returnValue;
    }
                                          
    AxisProfileResult abortAndResetProbe() override
    {
      return m_returnValue;
    }

    AxisProfileResult isProbeSupported() override
    {
      return m_returnValue;
    }

    AxisProfileResult booting() override
    {
      return m_returnValue;
    }
    
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};
    void loadBootingTasks() override {}
    
    // -------------------------------- Test methods ------------------------------------

    /// @brief This method can be used by tests to change the behavior of the member functions
    ///        by changing the returned value.
    ///
    /// @param[in]    result   New return code
    virtual void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }
  private: // data

    AxisProfileResult m_returnValue = AxisProfileResult::STS_OK;
  };
}
