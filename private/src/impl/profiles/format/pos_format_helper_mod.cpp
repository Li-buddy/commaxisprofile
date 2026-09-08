
/// @file
///
/// This file is part of the axisprofile library and implements a position data format helper to
/// provide modularize and linearize function for modulo axis.
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
#include <limits>
#include "pos_format_helper_mod.h"
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

void PosFormatHelperMod::init(double moduloRange, double limitHigh, double limitLow, std::int64_t initialTurns, double initialPos)
{
  m_moduloRange = moduloRange;
  reset(initialPos, initialTurns);
  updateLinLimit(limitHigh, limitLow);
}

void PosFormatHelperMod::reset(double initialPos, std::int64_t initialTurns)
{
  m_lastModuloPos = initialPos;
  m_fullTurns = initialTurns;
  m_lastLinearPos = initialTurns * m_moduloRange + initialPos;
  m_cbLimitTurns = 0;
  m_cbOffset = 0;
  m_cbOffsetRev = 0;
}

bool PosFormatHelperMod::convertModToLin(double newModuloPos, ModValuePair& newLinPosPair)
{
  double dif = newModuloPos - m_lastModuloPos;
  if (dif > m_moduloRange * 0.5)
  {
    // we turn in negative direction and the new value just passed the 0
    --m_fullTurns;
  }
  else if (dif < -m_moduloRange * 0.5)
  {
    // we turn in positive direction and the new value just passed the 0
    ++m_fullTurns;
  }
  if (newModuloPos + m_cbOffset + m_fullTurns * m_moduloRange > m_linMax)
  {
    m_fullTurns += m_linMinPair.turns - m_linMaxPair.turns;
    m_cbOffset += m_linMinPair.value - m_linMaxPair.value;
  }
  if (newModuloPos + m_cbOffset + m_fullTurns * m_moduloRange < m_linMin)
  {
    m_fullTurns += m_linMaxPair.turns - m_linMinPair.turns;
    m_cbOffset += m_linMaxPair.value - m_linMinPair.value;
  }
  
  m_fullTurns += static_cast<std::int64_t>(m_cbOffset / m_moduloRange);
  m_cbOffset = fmod(m_cbOffset, m_moduloRange);

  m_lastModuloPos = newModuloPos;
  newLinPosPair.turns = m_fullTurns;
  newLinPosPair.value = newModuloPos + m_cbOffset;
  return true;
}

double PosFormatHelperMod::convertLinToMod(double newLinPos)
{
  double dif = newLinPos - m_lastLinearPos;
  if (dif > (m_linMax - m_linMin) * 0.5)
  {
    // we turn in negative direction and the new value just passed the 0
    --m_cbLimitTurns;
    m_cbOffsetRev += m_linMaxPair.value - m_linMinPair.value;
  }
  else if (dif < (m_linMin - m_linMax) * 0.5)
  {
    // we turn in positive direction and the new value just passed the 0
    ++m_cbLimitTurns;
    m_cbOffsetRev += m_linMinPair.value - m_linMaxPair.value;
  }
  m_cbOffsetRev = fmod(m_cbOffsetRev, m_moduloRange);
  m_lastLinearPos = newLinPos;
  double tempPos = newLinPos - m_cbOffsetRev;
  return calcModulo(tempPos, m_moduloRange);
}

double PosFormatHelperMod::calcModulo(double inValue, double modValue)
{
  double outValue = fmod(inValue, modValue);
  if (outValue < 0.0)
  {
    outValue += modValue;
  }
  return outValue;
}

PosFormatHelperMod::ModValuePair PosFormatHelperMod::convertToModValuePair(const double inValue, const double moduloRange)
{
  ModValuePair modValuePair;
  modValuePair.turns = static_cast<std::int64_t>(inValue / moduloRange);
  modValuePair.value = std::fmod(inValue, moduloRange);
  if (modValuePair.value < 0)
  {
    modValuePair.value += moduloRange;
    --modValuePair.turns;
  }
  return modValuePair;
}

void PosFormatHelperMod::updateLinLimit(const double limitHigh, const double limitLow)
{
  m_linMax = limitHigh;
  m_linMin = limitLow;
  m_linMaxPair = convertToModValuePair(limitHigh, m_moduloRange);
  m_linMinPair = convertToModValuePair(limitLow, m_moduloRange);
}

void PosFormatHelperMod::resetModToLin(double linPos)
{
  std::int64_t turns = static_cast<std::int64_t>(linPos / m_moduloRange);
  double modPos = fmod(linPos, m_moduloRange);
  if (modPos < 0)
  {
    modPos += m_moduloRange;
    --turns;
  }
  reset(modPos, turns);
}
