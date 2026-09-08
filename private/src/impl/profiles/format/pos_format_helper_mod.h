#pragma once

/// @file
///
/// This file is part of the axisprofile library and implements a position data format helper to
/// provide calculation function for modulo type axis.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cstdint>
// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::format
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

  /// @brief This is a helper class to provide calculation function for modulo type axis
  class PosFormatHelperMod
  {
  public: // data

    /// @brief This type struct modulo value pair
    struct ModValuePair
    {
      std::int64_t turns;        ///< number of full turns (signed)
      double value;         ///< value within one modulo range
    };

  public: // methods
    /// @brief Constructor. (you have to call init afterwards)
    PosFormatHelperMod() = default;

    /// @brief Destructor.
    virtual ~PosFormatHelperMod() noexcept = default;

    /// @brief initialization of members (to be called, when the simple constructor is used)
    /// @param[in] moduloRange   Modulo range, all incoming position values are in the interval [0.0; moduloRange)
    /// @param[in] limitHigh     High limit of linear value
    /// @param[in] limitLow      Low limit of linear value
    /// @param[in] initialTurns  OPTIONAL: initial number of complete turns (signed). default is 0
    /// @param[in] initialPos    OPTIONAL: initial position value. default is 0.0
    void init(double moduloRange, double limitHigh = 0.0, double limitLow = 0.0, std::int64_t initialTurns = 0, double initialPos = 0.0);

    /// @brief reset the internal values to the given ones
    /// @param[in] initialPos     OPTIONAL: initial position value. default is 0.0
    /// @param[in] initialTurns   OPTIONAL: initial number of complete turns (signed). default is 0
    void reset(double initialPos = 0.0, std::int64_t initialTurns = 0);

    /// @brief Reset cache of calculation of modulo to linear, used before power on axis.
    /// @param[in] linPos         input linear position
    void resetModToLin(double linPos);

    // in some future, there would be resetLinToMod(double modPos), add only when needed.

    /// @brief Convert given modulo value to linearized position value
    ///
    /// This function determines the turn direction on base of the difference of the given value and the last given value. It is assumed, that
    /// the difference between two consecutively values is always less than half of the modulo range.
    /// 
    /// Example: When you have a rotational axis with a modulo range of 360° and getting position values in 2 milliseconds, this function can
    ///          work up to a velocity of 15000rpm.
    ///          Limit velocity (in this case) = 180°/0.002s = 15000rpm
    /// 
    /// Because full turns are counted as integer, you have no increasing rounding error. The error only is increasing because of the limited
    /// precision of great numbers of the double type (15 significant decimal digits). This means, e.g. a value of 360°*1000=360000° has an error
    /// of +-10E-9° or 1 nanodegree. And when you have 1 million rotations, you have an error of 1 millidegree (or 10E-6°).
    ///
    /// The cross boarder jump of the linearized position value also need be considered. Every time cross boarder happens offset will be added to
    /// turns and modulo position
    /// @param[in]  newModuloPos   new modulo position
    /// @param[out] newLinPosPair  new linear position value pair
    /// @return true on success
    bool convertModToLin(double newModuloPos, ModValuePair& newLinPosPair);

    /// @brief Convert given linear value to modulo value
    /// @param[in]  newLinPos   new linear position
    /// @return new modulo value
    double convertLinToMod(double newLinPos);

    /// @brief Modulo function
    /// @param[in] inValue  input Value.
    /// @param[in] modValue modulo factor
    /// @return Return the modulo value of the input value.
    double calcModulo(const double inValue, const double modValue);

  private: //method

    /// @brief Convert input values to Modulo value pair based on given modulo range
    /// @param[in] inValue  input Value.
    /// @param[in] moduloRange modulo range
    /// @return modulo value pair
    ModValuePair convertToModValuePair(const double inValue, const double moduloRange);

    /// @brief calculate and update limit for linear process
    /// @param[in] limitHigh    High Limit of linear value
    /// @param[in] limitLow     Low  Limit of linear value
    void updateLinLimit(const double limitHigh, const double limitLow);

  private: // data
    double m_moduloRange {360.0};   ///< modulo range, all incoming position values are in the interval [0.0; moduloRange)
    double m_lastModuloPos {};      ///< last received position (in the modulo range)
    double m_lastLinearPos {};      ///< last received linear position
    std::int64_t m_fullTurns {};         ///< number of turns of full modulo range (signed)
    ModValuePair m_linMaxPair {};   ///< max linear value pair for internal use
    ModValuePair m_linMinPair {};   ///< min linear value pair for internal use
    double m_linMax {};             ///< max linear value
    double m_linMin {};             ///< min linear value 
    double m_cbOffset {};           ///< offset for cross limit boarder during linear process(unit in turns)
    double m_cbOffsetRev {};        ///< offset for cross limit boarder during modulo process(unit in turns)
    std::int64_t m_cbLimitTurns {};      ///< number of turns of cross limit board (signed)
  };
}

