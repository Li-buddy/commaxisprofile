#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_scaling.h
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
//! the scaling interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/scaling/i_scaling.h"
#include <cstring>


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

  //! @class MockScaling
  //! @brief This class implements a mockup for the scaling interface.
  class MockScaling : public comm::axisprofile::IScaling
  {
  public: // methods

    //! @brief Constructor
    MockScaling() = default;
    //! @brief Destructor
    virtual ~MockScaling() noexcept override = default;

    comm::axisprofile::AxisProfileResult calcScalingLSBWeights() override { return m_returnValue;}
    double getAccScalingLSBWeight() const override { return 1.0;}
    double getAccScalingLSBWeight_inv() const override { return 1.0;}
    comm::axisprofile::TorqueScalingType getTorqueType() const override {return m_torqueType;};
    comm::axisprofile::AxisProfileResult checkScalingUniform() const override {return comm::axisprofile::AxisProfileResult::STS_OK;};
    comm::axisprofile::AxisProfileDeviceScalingType getDeviceScalingType(const comm::axisprofile::AxisProfileDeviceValueType /*valueType*/) const override { return comm::axisprofile::AxisProfileDeviceScalingType::UNDEFINED; };
    void setHomeOffset(double /*offset*/) override { };
    double getHomeOffset() const override { return m_homeOffset;}
    double convertPositionRawToScaled(const std::int32_t /*rawTelegramValue*/) const override { return 1.0;};
    std::int32_t convertPositionScaledToRaw(const double /*scaledInternalValue*/) const override { return 1;};
    double convertVelocityRawToScaled(std::int32_t /*rawTelegramValue*/) const override { return 1.0; };
    std::int32_t convertVelocityScaledToRaw(double /*scaledInternalValue*/) const override { return 1; };
    double convertTorqueRawToScaled(std::int16_t /*rawTelegramValue*/) const override { return 1.0; };
    std::int16_t convertTorqueScaledToRaw(double /*scaledInternalValue*/) const override { return 1; };
    comm::axisprofile::AxisProfilePosDataFormat getPosDataFormat() const override {return m_dataFormat;};
    double getCycleTime() const override { return m_cycleTime; };
    comm::axisprofile::AxisProfileResult checkAndSetModulo(double /*modValue*/, std::string& /*dynErrTxt*/) override { return m_returnValue;}
    double getModValue() const override { return m_moduleValue; }
    void setCycleTime(double cycleTime) override { m_cycleTime = cycleTime; };
    comm::axisprofile::AxisProfileResult setPosDataFormat(comm::axisprofile::AxisProfilePosDataFormat format)
    {
      m_dataFormat = format;
      return comm::axisprofile::AxisProfileResult::STS_OK;
    };
    void initModuloPosCmp(std::int32_t /*rawActPos*/) override {};
    double getRateTorqueForce() const override {return m_rateTorqueForce;};
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* handler) override
    {
      m_pSharingHandler = handler;
    };
    void loadBootingTasks() override {}
    comm::axisprofile::AxisProfileResult booting() override {return m_returnValue;}
    std::uint32_t getOutOfRangeErrCnt(const comm::axisprofile::ScalingDataCategory& /*category*/) const override{return m_clampCount;}
    void setOutOfRangeErrLimit(std::uint32_t /*maxTimes*/) override {}
    std::uint32_t getOutOfRangeErrLimit() const override {return 0;}

  public: // Data

    double m_TorqueScalingLSBWeight{1.0};  //!< torque weight
    comm::axisprofile::TimeUnit m_timeUnit{comm::axisprofile::TimeUnit::MINUTE};                          //!< default minute
    bool m_torqueMetrication{true};                                                                       //!< true: metric unit, false: imperial unit
    bool m_posMetrication{true};                                                                          //!< true: metric unit, false: imperial unit
    bool  m_velMetrication{true};                                                                         //!< true: metric unit, false: imperial unit
    bool m_accMetrication{true};                                                                          //!< true: metric unit, false: imperial unit     
    double m_homeOffset {1};                                                                              //!< reference offset;
    double m_cycleTime {1.0};                                                                             ///< default cycle time
    double m_moduleValue {0.0};                                                                           //!< modulo value
    double m_rateTorqueForce {1.0};                                                                       //!< rate torque/force
    comm::axisprofile::VelScalingType m_velocityType{comm::axisprofile::VelScalingType::UNDEFINED};       //!< scaling type of velocity
    comm::axisprofile::PosScalingType m_positionType{comm::axisprofile::PosScalingType::UNDEFINED};       //!< scaling type of position
    comm::axisprofile::AccScalingType m_accelerationType{comm::axisprofile::AccScalingType::UNDEFINED};   //!< scaling type of acceleration      
    comm::axisprofile::TorqueScalingType m_torqueType{comm::axisprofile::TorqueScalingType::UNDEFINED};   //!< current torque scaling type configuration
    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;    //!< Return value of member functions
    comm::axisprofile::AxisProfilePosDataFormat m_dataFormat {comm::axisprofile::AxisProfilePosDataFormat::FORMAT_ABSOLUTE};  ///< position data format
    comm::axisprofile::IAxisProfileSharingHandler* m_pSharingHandler {};    ///< mediator for sharing data
    std::uint32_t m_clampCount {0};      ///< counter for position clamp
  };
}
