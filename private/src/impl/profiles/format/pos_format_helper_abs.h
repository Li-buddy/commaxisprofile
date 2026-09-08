#pragma once

/// @file
///
/// This file is part of the axisprofile library and implements a position data format helper to
/// provide calculation for absolute type axis
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

  /// @brief This is a helper class to provide calculation function for absolute type axis
  class PosFormatHelperAbs
  {
  public: // methods
    /// @brief Constructor. (you have to call init afterwards)
    PosFormatHelperAbs() = default;

    /// @brief Destructor.
    virtual ~PosFormatHelperAbs() noexcept = default;

    /// @brief initialization of members (to be called, when the simple constructor is used)
    /// @param[in] limitHigh     High limit of linear value
    /// @param[in] limitLow      Low limit of linear value
    /// @param[in] initialPos    OPTIONAL: initial position value. default is 0.0
    void init(double limitHigh = 0.0, double limitLow = 0.0, double initialPos = 0.0);

    /// @brief reset the internal values to the given ones
    /// @param[in] initialPos     OPTIONAL: initial position value. default is 0.0
    void reset(double initialPos = 0.0);

    /// @brief Adjust position to make it within limit range
    /// This function is normally used for process target position from motion and generate valid position data for device
    /// @param[in]  pos input position 
    /// @return adjusted position
    double adjustForOverflow(double pos);

    /// @brief Convert given position within limit range to linear value
    /// This function is normally used for linearing actual position from device  
    /// @param[in]  newPos new input position  
    /// @return restored linear position
    double restoreLinValue(double newPos);

  private: // data

    double m_lastPos {};            ///< last received position for restore process
    std::int64_t m_overflowTurns {};     ///< number of turns of position overflow times
    double m_posMax {};             ///< max position value
    double m_posMin {};             ///< min position value 
    double m_cbOffset {};           ///< offset for cross limit boarder during adjust overflow process(unit in turns)
    double m_cbOffsetRev {};        ///< offset for cross limit boarder during restore process(unit in turns)
    std::int64_t m_cbLimitTurns {};      ///< number of turns of cross limit board (signed)
  };
}

