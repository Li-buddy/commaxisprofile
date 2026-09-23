#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile_cyclic_data.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and implements a mockup for
//! the axis profile cyclic data interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_cyclic_data_internal.h"


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

  //! @class MockAxisProfileCyclicData
  //! @brief This class implements a mockup for the axis profile cyclic data interface.
  class MockAxisProfileCyclicData : public comm::axisprofile::IAxisProfileCyclicDataInternal
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileCyclicData() = default;
    //! @brief Destructor
    virtual ~MockAxisProfileCyclicData() noexcept override = default;

    comm::axisprofile::AxisProfileResult getActualValuesDevice(comm::axisprofile::ActualValuesDevice* const actualValues) override
    {
      if (actualValues) 
      { 
        actualValues->m_actualPosition = m_actualValues.m_actualPosition; 
        actualValues->m_actualVelocity = m_actualValues.m_actualVelocity;
        actualValues->m_actualTorqueForce = m_actualValues.m_actualTorqueForce;
      }
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult setCommandValuesDevice(comm::axisprofile::CommandValuesDevice commandValues) override
    {
      m_actualValues.m_actualPosition = commandValues.m_commandPosition;
      m_actualValues.m_actualVelocity = commandValues.m_commandVelocity;
      m_actualValues.m_actualTorqueForce = commandValues.m_commandTorqueForce;
      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult getDigitalInputValueBit(comm::axisprofile::DigitalInputValueBitType /*valueType*/, bool& inputValue) override
    {
      inputValue = m_digitalInputValueBit;
      return m_returnValue;
    }

    bool checkActualValueAvailabilityNRT(comm::axisprofile::AxisProfileValueTypeID /*valueTypeID*/, comm::axisprofile::DataAccessErrorInfo& /*errorInfo*/) override
    {      
      return true;
    }

    comm::axisprofile::AxisProfileResult getSafetyOperationMode(comm::axisprofile::SafetyOperationMode& mode) override
    {
      mode = comm::axisprofile::SafetyOperationMode::SOM_NORMAL_MODE;
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult getSingleUserPDODataFromDevice(comm::axisprofile::UserPDOID /*id*/, comm::axisprofile::UserPDODataInfo& /*actualPDOValues*/) override
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult setSingleUserPDODataToDevice(comm::axisprofile::UserPDOID /*id*/, comm::axisprofile::UserPDODataInfo /*commandValues*/) override
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    bool checkSingleUserPDOActiveFromDevice(comm::axisprofile::UserPDOID /*id*/) override
    {
      return true;
    }

    bool checkSingleUserPDOActiveToDevice(comm::axisprofile::UserPDOID /*id*/) override
    {
      return true;
    }

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    stateMachineState   stateMachineStateInterface
    comm::axisprofile::AxisProfileResult setStateMachineStatusIF(const comm::axisprofile::StateMachineState* const stateMachineState) override
    {
      m_stateMachineState = stateMachineState;
      return m_returnValue;
    }

    const comm::axisprofile::StateMachineState*& getStatemachine()
    {
      return m_stateMachineState;
    }

    const IAxisProfileCyclicDataC* getCInterface() const override { return nullptr; }

    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};

    void setActSyncTime(double absSync0Time, double fieldbusCycleTime) override
    {
      m_latchTimeOfInput = absSync0Time - fieldbusCycleTime * 2;
      m_effectiveTimeOfOutput = absSync0Time + fieldbusCycleTime * 4;
    };
    double getEffectiveTimeOfActOutput() override { return m_effectiveTimeOfOutput; };
    double getLatchTimeOfActInput() override { return m_latchTimeOfInput; };
    void resetFPUFlag() override { };
    void loadBootingTasks() override { };
    comm::axisprofile::AxisProfileResult booting() override {return m_returnValue;}
    comm::axisprofile::AxisProfileResult setNewRefPosition([[maybe_unused]] std::int32_t newRefPos) override { return comm::axisprofile::AxisProfileResult::STS_OK; };
    comm::axisprofile::AxisProfileResult activeNewRefOperation([[maybe_unused]] bool active) override { return comm::axisprofile::AxisProfileResult::STS_OK; };

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

  public: // data

    bool  m_digitalInputValueBit{}; //!< Digital input value for a single bit

  protected: // data
    const comm::axisprofile::StateMachineState* m_stateMachineState = nullptr;                           //!< pointer to statemachine state
    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions

  private: // data
    comm::axisprofile::ActualValuesDevice m_actualValues{};  //!< Actual device values
    double m_latchTimeOfInput {0.0};      ///< stores latch time of acutal input in second
    double m_effectiveTimeOfOutput {0.0}; ///< stores effective time of actual output in second
  };
}
