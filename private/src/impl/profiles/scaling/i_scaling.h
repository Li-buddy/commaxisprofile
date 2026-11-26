#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_scaling.h
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
//! This header file is part of the axisprofile library and implements the
//! scaling interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/i_axisprofile_state_switch.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
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
    // Forward references
    // ---------------------------------------------------------------------------
    
    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class IScaling
    //! This is the scaling interface.
    class IScaling :public IAxisProfileSharing, public IAxisProfileStateSwitch
    {
    public: // methods

      //! @brief destructor
      virtual ~IScaling() noexcept override = default;

      //! @brief This method calculates all scaling data LSB (Least Significant Bit) weights.
      //! @return    Return value indicating success or error.
      virtual AxisProfileResult calcScalingLSBWeights() = 0;

      //! @brief This method returns the acceleration scaling LSB (Least Significant Bit) weight.
      //! @return    The velocity scaling factor
      virtual double getAccScalingLSBWeight() const = 0;

      //! @brief This method returns the inverse acceleration scaling LSB (Least Significant Bit) weight.
      //! @return    The velocity scaling factor
      virtual double getAccScalingLSBWeight_inv() const = 0;      

      //! @brief This method converts the given raw integer position value (from device) to double.
      //! @param[in]   rawTelegramValue      Pointer to the value that should be converted.
      //! @return      Returns the converted double value.
      virtual double convertPositionRawToScaled(std::int32_t rawTelegramValue) const = 0;
      
      //! @brief This method converts the given double position value to raw integer for the device.
      //! @param[in] scaledInternalValue   Pointer to the value that should be converted.
      //! @return    Returns the converted raw integer value.
      virtual std::int32_t convertPositionScaledToRaw(double scaledInternalValue) const = 0;

      //! @brief This method converts the given raw velocity integer value (from device) to double.
      //! @param[in]   rawTelegramValue      Pointer to the value that should be converted.
      //! @return      Returns the converted double value.
      virtual double convertVelocityRawToScaled(std::int32_t rawTelegramValue) const = 0;

      //! @brief This method converts the given double velocity value to raw integer for the device.
      //! @param[in]   scaledInternalValue   Pointer to the value that should be converted.
      //! @return      Returns the converted raw integer value.
      virtual std::int32_t convertVelocityScaledToRaw(double scaledInternalValue) const = 0;

      //! @brief This method converts the given raw torque/force integer value (from device) to double.
      //! @param[in]   rawTelegramValue      Pointer to the value that should be converted.
      //! @return      Returns the converted double value.
      virtual double convertTorqueRawToScaled(std::int16_t rawTelegramValue) const = 0;

      //! @brief This method converts the given double torque/force value to raw integer for the device.
      //! @param[in]   scaledInternalValue   Pointer to the value that should be converted.
      //! @return      Returns the converted raw integer value.
      virtual std::int16_t convertTorqueScaledToRaw(double scaledInternalValue) const = 0;

      //! @brief This method returns the torque scaling type.
      //! @return    Torque scaling type
      virtual TorqueScalingType getTorqueType() const = 0;

      //! @brief This method checks scaling uniform.
      //! @return    Return value indicating success or error.
      virtual AxisProfileResult checkScalingUniform() const  = 0;

      //! @brief This method gets the actual scaling type of the connected device for the given value type
      //! @param[in] valueType   Defines the value type for the scaling type request.
      //! @return    Returns the scaling type of the device
      virtual AxisProfileDeviceScalingType getDeviceScalingType(const AxisProfileDeviceValueType valueType) const = 0;

      //! @brief This method set offset between control and device
      //! @param[in] offset offset value
      //! @return    Offset between profile and device
      virtual void setHomeOffset(double offset) = 0;

      //! @brief Get offset between profile and device.
      //! @return    Return profile reference position.
      virtual double getHomeOffset() const = 0;

      /// @brief Get position data format.
      /// @return The current position data format.
      virtual AxisProfilePosDataFormat getPosDataFormat() const = 0;

      /// @brief This method check and set modulo value of the device
      /// @param[in]  modValue        Modulo value that to be checked (when driver has build modulo conversion) or set (when axisprofile handles modulo conversion)
      /// @param[out] dynErrTxt       Dynamic error text when error returns, used for diagnosis of the caller
      /// @return     Return value indicating success or error.
      virtual AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) = 0;
      
      /// @brief This method get the modulo value in IS unit
      /// @return Modulo value in IS unit
      virtual double getModValue() const = 0;

      /// @return The system cycle time
      virtual double getCycleTime() const = 0;

      /// @brief This method set new APR system cycle time, unit in second.
      /// @param[in] cycleTime   Cycle time in second
      virtual void setCycleTime(double cycleTime) = 0;

      /// @brief Set position data format.
      /// @param[in] format position data format
      /// @return    Return value indicating success or error.
      virtual AxisProfileResult setPosDataFormat(AxisProfilePosDataFormat format) = 0;

      /// @brief Init modulo position with the raw actual position value from device
      /// @param[in] rawActPos raw actual position value from device
      virtual void initModuloPosCmp(std::int32_t rawActPos) = 0;

      /// @brief This method get rate torque force of the load in IS unit
      /// @return Rate torque for rotary axis or rate force for linear axis in IS unit
      virtual double getRateTorqueForce() const = 0;

      /// @brief This method gets the number of times the data was out of range for the given scaling data category
      /// @param[in] category The scaling data category to query
      /// @return The number of times the data was out of range
      virtual std::uint32_t getOutOfRangeErrCnt(const ScalingDataCategory& category) const = 0;

      /// @brief This method sets the maximum number of times the data can be out of range.
      /// @param[in] maxTimes The maximum number of times the data can be out of range before an error is reported.
      /// @return true if the limit was set successfully, false otherwise.
      virtual bool setOutOfRangeErrLimit(std::uint32_t maxTimes) = 0;

      /// @brief This method gets the maximum number of times the data can be out of range.
      /// @return the maximum number of times
      virtual std::uint32_t getOutOfRangeErrLimit() const = 0;
    };
  }
}
