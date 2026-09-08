
/// @file
///
/// This file is part of the axisprofile library and implements a position data format helper to
/// provide calculation function for absolute type axis
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cmath>
#include "pos_format_helper_abs.h"
#include "axisprofile_internal_defs.h"
// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile::format;

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

void PosFormatHelperAbs::init(double limitHigh, double limitLow, double initialPos)
{
  m_posMax = limitHigh;
  m_posMin = limitLow;
  reset(initialPos);
}

void PosFormatHelperAbs::reset(double initialPos)
{
  m_lastPos = initialPos;
  m_overflowTurns = 0;
  m_cbLimitTurns = 0;
  m_cbOffset = 0;
  m_cbOffsetRev = 0;
}

double PosFormatHelperAbs::adjustForOverflow(double pos)
{
  if (pos + m_cbOffset > m_posMax)
  {
    m_cbOffset += m_posMin - m_posMax;
    ++m_overflowTurns;
  }
  if (pos + m_cbOffset < m_posMin)
  {
    m_cbOffset += m_posMax - m_posMin;
    --m_overflowTurns;
  }
  return pos + m_cbOffset;
}

double PosFormatHelperAbs::restoreLinValue(double newPos)
{
  double dif = newPos - m_lastPos;
  if (dif > (m_posMax - m_posMin) * 0.5)
  {
    // we turn in negative direction and the new value just passed the 0
    --m_cbLimitTurns;
    m_cbOffsetRev += m_posMax - m_posMin;
  }
  else if (dif < (m_posMin - m_posMax) * 0.5)
  {
    // we turn in positive direction and the new value just passed the 0
    ++m_cbLimitTurns;
    m_cbOffsetRev += m_posMin - m_posMax;
  }
  m_lastPos = newPos;
  return newPos - m_cbOffsetRev;
}
