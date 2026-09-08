// ---------------------------------------------------------------------------
//
//! @file       unit_helper.cpp
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
//! This file is part of the motion library and implements the
//! unit helper.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "unit_helper.h"


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Implementation of data types
// ---------------------------------------------------------------------------

std::array<UnitHelper::ConversionData, static_cast<std::size_t>(Unit::COUNT)> UnitHelper::s_convData{};

// ---------------------------------------------------------------------------
// Implementation of classes and structures
// ---------------------------------------------------------------------------

std::vector<const char*> UnitHelper::getAvailableUnitsURI(UnitObjType objType, UnitValueType valueType)
{
  std::vector<const char*> res;
  for (const auto& data : s_convData)
  {
    if (data.m_unitAbbrURI && ((data.m_objType == objType) || (data.m_objType == UnitObjType::ANY)) && (data.m_valueType == valueType))
    {
      res.push_back(data.m_unitAbbrURI);
    }
  }
  return res;
}

bool UnitHelper::findFactor(UnitObjType objType, UnitValueType valueType, Unit valueUnit, double& factorToSI)
{
  auto unitT = static_cast<std::size_t>(valueUnit);
  if ((unitT >= static_cast<std::size_t>(Unit::COUNT)) ||
    ((s_convData[unitT].m_objType != objType) && (s_convData[unitT].m_objType != UnitObjType::ANY) && (objType != UnitObjType::ANY)) ||
    (s_convData[unitT].m_valueType != valueType))
  {
    return false;
  }
  factorToSI = s_convData[unitT].m_toSIFactor;
  return true;
}

bool UnitHelper::findFactor(Unit valueUnit, double& factorToSI)
{
  auto unitT = static_cast<std::size_t>(valueUnit);
  if ((unitT >= static_cast<std::size_t>(Unit::COUNT)) || !s_convData[unitT].m_unitStr)
  {
    return false;
  }
  factorToSI = s_convData[unitT].m_toSIFactor;
  return true;
}

bool UnitHelper::convertToSI(UnitObjType objType, UnitValueType valueType, double value, Unit valueUnit, double& dest)
{
  double factor;
  if (!findFactor(objType, valueType, valueUnit, factor))
  {
    return false;
  }
  dest = value * factor;
  return true;
}

bool UnitHelper::convertToSI(UnitObjType objType, UnitValueType valueType, double value, const std::string& valueUnitURI, double& dest)
{
  Unit unit;
  if (!uriStringToEnum(valueUnitURI.c_str(), unit))
  {
    return false;
  }
  return convertToSI(objType, valueType, value, unit, dest);
}

bool UnitHelper::convertToSI(UnitObjType objType, UnitValueType valueType, double value, const char* valueUnit, double& dest)
{
  Unit unit;
  if (!stringToEnum(valueUnit, unit))
  {
    return false;
  }
  return convertToSI(objType, valueType, value, unit, dest);
}

bool UnitHelper::validateUnit(UnitObjType objType, UnitValueType valueType, Unit valueUnit)
{
  auto unitT = static_cast<std::size_t>(valueUnit);
  return ((unitT < static_cast<std::size_t>(Unit::COUNT)) &&
    ((s_convData[unitT].m_objType == objType) || (s_convData[unitT].m_objType == UnitObjType::ANY) || (objType == UnitObjType::ANY)) &&
    (s_convData[unitT].m_valueType == valueType));
}

bool UnitHelper::uriStringToEnum(const char* unitURI, Unit& dest)
{
  std::string find{ unitURI };
  for (std::size_t i=0; i<static_cast<std::size_t>(Unit::COUNT); ++i)
  {
    if (s_convData[i].m_unitAbbrURI && (find == s_convData[i].m_unitAbbrURI))
    {
      dest = static_cast<Unit>(i);
      return true;
    }
  }

  return false;
}

bool UnitHelper::stringToEnum(const char* unit, Unit& dest)
{
  std::string find{ unit };
  for (std::size_t i = 0; i < static_cast<std::size_t>(Unit::COUNT); ++i)
  {
    if (s_convData[i].m_unitAbbr && (find == s_convData[i].m_unitAbbr))
    {
      dest = static_cast<Unit>(i);
      return true;
    }
  }

  return false;
}

bool UnitHelper::convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, Unit valueUnit, double& dest)
{
  double factor;
  if (!findFactor(objType, valueType, valueUnit, factor))
  {
    return false;
  }
  dest = siValue / factor;
  return true;
}

bool UnitHelper::convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, const std::string& valueUnitURI, double& dest)
{
  Unit unit;
  if (!uriStringToEnum(valueUnitURI.c_str(), unit))
  {
    return false;
  }
  return convertFromSI(objType, valueType, siValue, unit, dest);
}

bool UnitHelper::convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, const char* valueUnit, double& dest)
{
  Unit unit;
  if (!stringToEnum(valueUnit, unit))
  {
    return false;
  }
  return convertFromSI(objType, valueType, siValue, unit, dest);
}

const UnitHelper::ConversionData* UnitHelper::findData(Unit valueUnit)
{
  auto unitT = static_cast<std::size_t>(valueUnit);
  if (unitT >= static_cast<std::size_t>(Unit::COUNT))
  {
    return nullptr;
  }
  return &s_convData[unitT];
}

const char* UnitHelper::unitToURIString(Unit src)
{
  auto data = findData(src);
  return data ? data->m_unitAbbrURI : nullptr;
}

const char* UnitHelper::unitToString(Unit src)
{
  auto data = findData(src);
  return data ? data->m_unitAbbr : nullptr;
}

const char* UnitHelper::unitToNameString(Unit src)
{
  auto data = findData(src);
  return data ? data->m_unitStr : nullptr;
}

void UnitHelper::initObjUnitConfig(UnitObjType objType, ObjUnitConfig& dest)
{
  dest = { Unit::COUNT };
  switch (objType)
  {
  case UnitObjType::LINEAR:
  case UnitObjType::ANY:
  case UnitObjType::COUNT:
    dest.units[static_cast<std::size_t>(UnitValueType::POSITION)] = Unit::MILLIMETER;
    dest.units[static_cast<std::size_t>(UnitValueType::VELOCITY)] = Unit::MILLIMETER_PER_MINUTE;
    dest.units[static_cast<std::size_t>(UnitValueType::ACCELERATION)] = Unit::METER_PER_SECOND_SQARED;
    dest.units[static_cast<std::size_t>(UnitValueType::JERK)] = Unit::METER_PER_SECOND_QUBED;
    dest.units[static_cast<std::size_t>(UnitValueType::FORCE)] = Unit::NEWTON;
    dest.units[static_cast<std::size_t>(UnitValueType::TORQUE)] = Unit::NEWTON_METERS;
    dest.units[static_cast<std::size_t>(UnitValueType::TIME)] = Unit::SECOND;
    break;

  case UnitObjType::ROTATIONAL:
    dest.units[static_cast<std::size_t>(UnitValueType::POSITION)] = Unit::DEGREE;
    dest.units[static_cast<std::size_t>(UnitValueType::VELOCITY)] = Unit::REVOLUTIONS_PER_MINUTE;
    dest.units[static_cast<std::size_t>(UnitValueType::ACCELERATION)] = Unit::REVOLUTIONS_PER_MINUTE_SQARED;
    dest.units[static_cast<std::size_t>(UnitValueType::JERK)] = Unit::REVOLUTIONS_PER_MINUTE_QUBED;
    dest.units[static_cast<std::size_t>(UnitValueType::FORCE)] = Unit::NEWTON;
    dest.units[static_cast<std::size_t>(UnitValueType::TORQUE)] = Unit::NEWTON_METERS;
    dest.units[static_cast<std::size_t>(UnitValueType::TIME)] = Unit::SECOND;
    break;
  }
}

std::vector<const char*> UnitHelper::getUnitTypes()
{
  std::vector<const char*> res;
  for (std::size_t i=0; i<static_cast<std::size_t>(UnitValueType::COUNT); ++i)
  {
    res.push_back(unitValueTypeToString(static_cast<UnitValueType>(i)));
  }
  return res;
}

bool UnitHelper::getUnitData(Unit src, UnitObjType& objType, UnitValueType& valueType)
{
  auto unitT = static_cast<std::size_t>(src);
  if ((unitT >= static_cast<std::size_t>(Unit::COUNT)) || !s_convData[unitT].m_unitStr)
  {
    return false;
  }
  objType = s_convData[unitT].m_objType;
  valueType = s_convData[unitT].m_valueType;
  return true;
}
