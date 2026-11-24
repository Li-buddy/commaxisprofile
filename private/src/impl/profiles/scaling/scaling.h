#pragma once

// ---------------------------------------------------------------------------
//
//! @file       scaling.h
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
//! base class for scaling.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_scaling.h"
#include "profiles/profile_trace.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "profiles/profile_defs.h"
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestScaling;
class TestCanopenScaling;
class TestSercosScaling;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief This structure keeps track of the number of times a value
  /// has been clamped and whether a diagnostic has been reported.
  struct ClampState
  {
    std::uint32_t count {};
    bool reported {};

    /// @brief Clear the clamp count and allow diagnostics to be reported again.
    void reset()
    {
      count = 0;
      reported = false;
    }
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  //! @brief structure of calculating LSB weight
  typedef struct CalcScalingLSBWeightsMethod
  {
    const double* const m_ptrScalingLSBWeight;         //!< pointer to scaling weight
    const std::string* const m_methodName;             //!< for trace output
    const std::function<AxisProfileResult()> m_fnct;   //!< method to call

    //! @brief  Constructor
    //! @param[in]   ptrScalingLSBWeight   Pointer to ScalingLSBWeight
    //! @param[in]   methodName            Pointer to the methodName
    //! @param[in]   fnct                  Pointer to the callback
    CalcScalingLSBWeightsMethod(const double* const ptrScalingLSBWeight, const std::string* const methodName, const std::function<AxisProfileResult()> &fnct)
      : m_ptrScalingLSBWeight(ptrScalingLSBWeight),
      m_methodName(methodName),
      m_fnct(fnct)
    {
    };
  } CalcScalingLSBWeightsMethod;                       //!< type name

  //! @class Scaling
  //! This is the base class for scaling.
  class Scaling : public IScaling
  {
  public: // methods

    //! @brief Constructor
    //! @param[in]   profileBasicInfo   Profile information
    explicit Scaling(ProfileBasicInfo& profileBasicInfo);

    //! @brief destructor
    ~Scaling() noexcept override = default;

    double getAccScalingLSBWeight() const final;
    double getAccScalingLSBWeight_inv() const final;
    TorqueScalingType getTorqueType() const final {return m_torqueType;};
    AxisProfileResult checkScalingUniform() const final;
    AxisProfileDeviceScalingType getDeviceScalingType(const AxisProfileDeviceValueType valueType) const final;
    void setHomeOffset(double offset) final;
    double getHomeOffset() const final;
    double convertPositionRawToScaled(std::int32_t rawTelegramValue) const override;
    std::int32_t convertPositionScaledToRaw(double scaledInternalValue) const override;
    double convertVelocityRawToScaled(std::int32_t rawTelegramValue) const final;
    std::int32_t convertVelocityScaledToRaw(double scaledInternalValue) const final;
    double convertTorqueRawToScaled(std::int16_t rawTelegramValue) const final;
    std::int16_t convertTorqueScaledToRaw(double scaledInternalValue) const final;
    AxisProfilePosDataFormat getPosDataFormat() const final;
    AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) override;
    double getModValue() const final;
    double getCycleTime() const final;
    void setCycleTime(double cycleTime) final;
    AxisProfileResult setPosDataFormat(AxisProfilePosDataFormat format) final;
    void initModuloPosCmp(std::int32_t rawActPos) override;
    double getRateTorqueForce() const final;
    AxisProfileResult booting() final;
    std::uint32_t getOutOfRangeErrCnt(const ScalingDataCategory& category) const final;
    void setOutOfRangeErrLimit(std::uint32_t maxTimes) final;
    std::uint32_t getOutOfRangeErrLimit() const final;

    /// @brief This method reports a clamping error if the value has been clamped.
    /// @tparam T      the type of the raw telegram value (e.g., std::int16_t, std::int32_t)
    /// @param[in]     rawValue         the raw value before clamping
    /// @param[in]     rawTelegramValue the raw telegram value after clamping
    /// @param[in,out] state            the ClampState object that keeps track of the clamping count and reporting status
    /// @param[in]     valueName        the name of the value being clamped
    template<typename T>
    void reportClampError(double rawValue, T rawTelegramValue, ClampState& state, const char* valueName) const;

  protected: // methods

    //! @brief This method execute calcScalingLSBWeights
    //! @return      Return value indicating success or error.
    AxisProfileResult executeCalcScalingLSBWeights();

    //! @brief This method returns the position scaling LSB (Least Significant Bit) weight.
    //! @return    The position scaling factor
    double getPosScalingLSBWeight() const;

    //! @brief This method returns the inverse position scaling LSB (Least Significant Bit) weight.
    //! @return    The position inverse scaling factor
    double getPosScalingLSBWeight_inv() const;

    //! @brief This method returns the velocity scaling LSB (Least Significant Bit) weight.
    //! @return    The velocity scaling factor
    double getVelScalingLSBWeight() const;

    //! @brief This method returns the inverse velocity scaling LSB (Least Significant Bit) weight.
    //! @return    The velocity inverse scaling factor
    double getVelScalingLSBWeight_inv() const;

    //! @brief This method returns the torque scaling LSB (Least Significant Bit) weight.
    //! @return    The torque scaling factor
    double getTorqueScalingLSBWeight() const;

    //! @brief This method returns the inverse torque scaling LSB (Least Significant Bit) weight.
    //! @return    The torque inverse scaling factor
    double getTorqueScalingLSBWeight_inv() const;

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

  /// @brief Convert a raw value to an integer, saturating at the limits of T.
  /// @tparam T Raw telegram type: std::int16_t or std::int32_t.
  /// @param[in] value Raw value in telegram counts.
  /// @param[out] clamped True for NaN or an out-of-range value; false otherwise.
  /// @return The in-range value truncated toward zero, the nearest bound if out of range, or zero for NaN.
    template<typename T>
    static T clampCast(double value, bool& clamped) noexcept
    {
      static_assert(std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::int32_t>);

      constexpr double MAX_VAL = static_cast<double>(std::numeric_limits<T>::max());
      constexpr double MIN_VAL = static_cast<double>(std::numeric_limits<T>::min());

      clamped = false;

      if (std::isnan(value))
      {
        clamped = true;
        return T {0};
      }

      if (value > MAX_VAL)
      {
        clamped = true;
        return std::numeric_limits<T>::max();
      }

      if (value < MIN_VAL)
      {
        clamped = true;
        return std::numeric_limits<T>::min();
      }

      return static_cast<T>(value);
    }

    /// @brief Update clamping counter.
    /// @return True when the reporting threshold is reached.
    bool updateClampCounter(ClampState& counter) const;

  private: // methods

    /// @brief Convert a scaled value to raw telegram counts.
    ///        Report one diagnostic per run of consecutive clamped conversions that reaches the configured threshold.
    /// @tparam T Raw telegram type: std::int16_t or std::int32_t.
    /// @param[in] scaledInternalValue Value in internal units.
    /// @param[in] inverseWeight Raw counts per internal unit.
    /// @param[in,out] state Clamping history for this quantity; reset on an in-range conversion.
    /// @param[in] valueName Quantity name used in the diagnostic.
    /// @return Raw telegram value produced by clampCast().
    template<typename T>
    T convertScaledToRaw(double scaledInternalValue, double inverseWeight, ClampState& state, const char* valueName) const;

  protected: // Data

    friend class ::TestScaling;                                            //!< unit test of Scaling
    friend class ::TestCanopenScaling;                                     //!< unit test of Canopen Scaling
    friend class ::TestSercosScaling;                                      //!< unit test of Sercos Scaling

    std::vector<CalcScalingLSBWeightsMethod> m_scalingLSBWeightsMethods{}; //!< vector contains scaling methods
    ProfileBasicInfo& m_profileBasicInfo;                                  ///< Profile basic information

    AxisProfilePosDataFormat m_posDataFormat {AxisProfilePosDataFormat::UNDEFINED}; ///< Processing format  0: Absolute format 1 : Modulo format
    TimeUnit m_timeUnit{TimeUnit::MINUTE};                          //!< default: minute
    bool m_torqueMetrication{true};                                 //!< true: metric unit, false: imperial unit
    bool m_posMetrication{true};                                    //!< true: metric unit, false: imperial unit
    bool m_velMetrication{true};                                    //!< true: metric unit, false: imperial unit
    bool m_accMetrication{true};                                    //!< true: metric unit, false: imperial unit
    VelScalingType m_velocityType{VelScalingType::UNDEFINED };      //!< scaling type of velocity
    PosScalingType m_positionType{PosScalingType::UNDEFINED };      //!< scaling type of position
    AccScalingType m_accelerationType{AccScalingType::UNDEFINED };  //!< scaling type of acceleration
    TorqueScalingType m_torqueType{TorqueScalingType::UNDEFINED};   //!< current torque scaling type configuration

    // Note: LSB is Least Significant Bit, which refers to the physical value represented by a single digital count (one LSB)
    // E.g., if the scaling factor of position is 0.01, then LSB of S-0-0051 represents 0.01m
    double m_posScalingLSBWeight {1.0};                 //!< Position scaling LSB weight, raw --> SI
    double m_posScalingLSBWeight_inv {1.0};             //!< Position scaling LSB weight inverse
    double m_velScalingLSBWeight {1.0};                 //!< Velocity scaling LSB weight, raw --> SI
    double m_velScalingLSBWeight_inv {1.0};             //!< Velocity scaling LSB weight inverse
    double m_accScalingLSBWeight {1.0};                 //!< Acceleration scaling LSB weight, raw --> SI
    double m_accScalingLSBWeight_inv {1.0};             //!< Acceleration scaling LSB weight inverse
    double m_torqueScalingLSBWeight {1.0};              //!< Torque scaling LSB weight, raw --> SI
    double m_torqueScalingLSBWeight_inv {1.0};          //!< Torque scaling LSB weight inverse
    double m_moduloValue {1.0};                         //!< Modulo value
    double m_rateTorqueForce {1.0};                     //!< Rate torque/force of load

    const static std::string POSITION;          //!< position scaling
    const static std::string VELOCITY;          //!< velocity scaling
    const static std::string TORQUE;            //!< torque scaling
    const static std::string ACCELERATION;      //!< acceleration scaling

    IAxisProfileSharingHandler* m_sharingHandler {};    ///< mediator for sharing data

  private: // data

    double m_offset {0};                        ///< Offset between profile and device
    double m_cycleTime {STD_CYCLE_TIME};        ///< default cycle time

    mutable ClampState m_positionClamp {};      ///< counter for position clamp
    mutable ClampState m_velocityClamp {};      ///< counter for velocity clamp
    mutable ClampState m_torqueClamp {};        ///< counter for torque clamp
    mutable std::uint32_t m_allowedMaxOutOfRangeTimes; ///< the maximum number of times that the value can be out of range before reporting an error
  };
}
