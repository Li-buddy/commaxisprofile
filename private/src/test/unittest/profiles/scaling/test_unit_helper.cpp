#include <cmath>
#include "profiles/scaling/unit_helper.h"

#include "test_unit_helper.h"


REGISTER_TEST(TestUnitHelper);
REGISTER_TEST_TO_GROUP(TestUnitHelper, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestUnitHelper, 2_UnitTest);

using namespace comm::axisprofile;

constexpr double STD_IPO_EPS = 1.0E-10;        //!< default epsilon for interpolation calculations

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestUnitHelper::doTest()
{
  log(L"\n<BLUE><b>------- Unit helper Test -------</b></BLUE>");

  HRESULT res = S_OK;
  res |= runValidTestCase();
  res |= runErrorTestCase();

  DummyUnit dummy;
  dummy.runTest(this);

  log(L"\n\n<BLUE><b>------- Test end -------</b></BLUE>");

  return res;
}


HRESULT TestUnitHelper::runValidTestCase()
{
  log(L"<b>\n---Run valid test case ---</b>");

  HRESULT res = S_OK;
  res |= runTestCaseVelocityLinear();
  res |= runTestCaseVelocityRotational();
  res |= runTestCaseStringConvert();
  res |= runTestCaseBrowse();
  res |= runTestCaseDefaults();

  return res;
}

HRESULT TestUnitHelper::runTestCaseVelocityLinear()
{
  const double VELOCITY_IN_INCH_PER_SECOND = 7.5;
  const double VELOCITY_IN_M_PER_SECOND = 0.1905;
  double velocityInMetersPerSecond;
  log(L"\nConvert %f inch/s to SI units (expect %f m/s)...", VELOCITY_IN_INCH_PER_SECOND, VELOCITY_IN_M_PER_SECOND);
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_INCH_PER_SECOND, Unit::INCH_PER_SECOND, velocityInMetersPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n  received %f m/s...", velocityInMetersPerSecond);
  checkDoubleValue(velocityInMetersPerSecond , L"\nUnexpected value from conversion returned!", VELOCITY_IN_M_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with unit abbreviation \'in/s\'...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_INCH_PER_SECOND, "in/s", velocityInMetersPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f m/s...", velocityInMetersPerSecond);
  checkDoubleValue(velocityInMetersPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_M_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with URI unit abbreviation \'in-per-s\'...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_INCH_PER_SECOND, std::string("in-per-s"), velocityInMetersPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f m/s...", velocityInMetersPerSecond);
  checkDoubleValue(velocityInMetersPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_M_PER_SECOND, STD_IPO_EPS);

  double velocityInInchPerSecond;
  log(L"\nConvert %f SI units (m/s) to inch/s (expect %f in/s)...", VELOCITY_IN_M_PER_SECOND, VELOCITY_IN_INCH_PER_SECOND);
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_M_PER_SECOND, Unit::INCH_PER_SECOND, velocityInInchPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n  received %f m/s...", velocityInInchPerSecond);
  checkDoubleValue(velocityInInchPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_INCH_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with unit abbreviation \'in/s\'...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_M_PER_SECOND, "in/s", velocityInInchPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f m/s...", velocityInInchPerSecond);
  checkDoubleValue(velocityInInchPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_INCH_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with URI unit abbreviation \'in-per-s\'...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, VELOCITY_IN_M_PER_SECOND, std::string("in-per-s"), velocityInInchPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f m/s...", velocityInInchPerSecond);
  checkDoubleValue(velocityInInchPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_INCH_PER_SECOND, STD_IPO_EPS);

  log(L"\n\nValidate some units...");
  checkBoolResult(UnitHelper::validateUnit(UnitObjType::LINEAR, UnitValueType::VELOCITY, Unit::INCH_PER_SECOND), L"\nExpected success!", true);
  checkBoolResult(UnitHelper::validateUnit(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, Unit::INCH_PER_SECOND), L"\nExpected error!", false);
  checkBoolResult(UnitHelper::validateUnit(UnitObjType::LINEAR, UnitValueType::VELOCITY, Unit::METER), L"\nExpected error!", false);


  return S_OK;
}

HRESULT TestUnitHelper::runTestCaseVelocityRotational()
{
  const double VELOCITY_IN_RPM = 12.7;
  const double VELOCITY_IN_DEG_PER_SECOND = 76.2;
  double velocityInRadPerSecond;
  log(L"\n\nConvert %f rpm to SI units (expect %f deg/s)...", VELOCITY_IN_RPM, VELOCITY_IN_DEG_PER_SECOND);
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_RPM, Unit::REVOLUTIONS_PER_MINUTE, velocityInRadPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n  received %f deg/s...", velocityInRadPerSecond);
  checkDoubleValue(velocityInRadPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_DEG_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with unit abbreviation \'rpm\'...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_RPM, "rpm", velocityInRadPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f deg/s...", velocityInRadPerSecond);
  checkDoubleValue(velocityInRadPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_DEG_PER_SECOND, STD_IPO_EPS);

  log(L"\n  Convert with URI unit abbreviation \'rpm\'...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_RPM, std::string("rpm"), velocityInRadPerSecond), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f deg/s...", velocityInRadPerSecond);
  checkDoubleValue(velocityInRadPerSecond, L"\nUnexpected value from conversion returned!", VELOCITY_IN_DEG_PER_SECOND, STD_IPO_EPS);


  double velocityInRPM;
  log(L"\nConvert %f SI units (deg/s) to rpm (expect %f rpm)...", VELOCITY_IN_DEG_PER_SECOND, VELOCITY_IN_RPM);
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_DEG_PER_SECOND, Unit::REVOLUTIONS_PER_MINUTE, velocityInRPM), L"\nUnexpected conversion error!", true);
  log(L"\n  received %f deg/s...", velocityInRPM);
  checkDoubleValue(velocityInRPM, L"\nUnexpected value from conversion returned!", VELOCITY_IN_RPM, STD_IPO_EPS);

  log(L"\n  Convert with unit abbreviation \'rpm\'...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_DEG_PER_SECOND, "rpm", velocityInRPM), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f deg/s...", velocityInRPM);
  checkDoubleValue(velocityInRPM, L"\nUnexpected value from conversion returned!", VELOCITY_IN_RPM, STD_IPO_EPS);

  log(L"\n  Convert with URI unit abbreviation \'rpm\'...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::ROTATIONAL, UnitValueType::VELOCITY, VELOCITY_IN_DEG_PER_SECOND, std::string("rpm"), velocityInRPM), L"\nUnexpected conversion error!", true);
  log(L"\n    received %f deg/s...", velocityInRPM);
  checkDoubleValue(velocityInRPM, L"\nUnexpected value from conversion returned!", VELOCITY_IN_RPM, STD_IPO_EPS);

  return S_OK;
}

HRESULT TestUnitHelper::runTestCaseStringConvert()
{
  log(L"\n\nSimple enum-string convert");
  Unit unit;
  log(L"\n  Convert URI string to enum (ft)...");
  checkBoolResult(UnitHelper::uriStringToEnum("ft", unit), L"\nUnexpected conversion error!", true);
  checkUnit(unit, L"\nUnexpected conversion error!", Unit::FEET);
  log(L"\n  Convert URI string to enum (km-per-h)...");
  checkBoolResult(UnitHelper::uriStringToEnum("km-per-h", unit), L"\nUnexpected conversion error!", true);
  checkUnit(unit, L"\nUnexpected conversion error!", Unit::KILOMETER_PER_HOUR);

  log(L"\n  Convert string to enum (m/s^2)...");
  checkBoolResult(UnitHelper::stringToEnum("m/s^2", unit), L"\nUnexpected conversion error!", true);
  checkUnit(unit, L"\nUnexpected conversion error!", Unit::METER_PER_SECOND_SQARED);
  log(L"\n  Convert string to enum (ms)...");
  checkBoolResult(UnitHelper::stringToEnum("ms", unit), L"\nUnexpected conversion error!", true);
  checkUnit(unit, L"\nUnexpected conversion error!", Unit::MILLISECOND);


  log(L"\n  Get data of a valid unit...");
  UnitObjType objType;
  UnitValueType valueType;
  checkBoolResult(UnitHelper::getUnitData(Unit::CENTIMETER_PER_SECOND_SQARED, objType, valueType), L"\nUnexpected error!", true);
  if(objType != UnitObjType::LINEAR)
  {
    error(L"\nUnexpected result!");
  }
  if (valueType != UnitValueType::ACCELERATION)
  {
    error(L"\nUnexpected result!");
  }
  log(L"\n  Get data of an invalid unit...");
  checkBoolResult(UnitHelper::getUnitData(Unit::RESERVED90, objType, valueType), L"\nUnexpected success!", false);

  return S_OK;
}

HRESULT TestUnitHelper::runTestCaseBrowse()
{
  log(L"\n\nBrowse all (uni) units for linear jerk:");
  auto units = UnitHelper::getAvailableUnitsURI(UnitObjType::LINEAR, UnitValueType::JERK);
  for (const auto& name : units)
  {
    Unit unit;
    UnitHelper::uriStringToEnum(name, unit);
    auto unitName = UnitHelper::unitToNameString(unit);
    auto unitAbbrName = UnitHelper::unitToString(unit);
    auto unitURIName = UnitHelper::unitToURIString(unit);
    if (!unitName || !unitAbbrName || !unitURIName)
    {
      error(L"\nCould not convert the unit!");
    }
    else
    {
      log(L"\n  Unit: %ls    Abbreviation: %ls     URI: %ls", test::StringHelper::utf8ToWide(unitName).c_str(), test::StringHelper::utf8ToWide(unitAbbrName).c_str(), test::StringHelper::utf8ToWide(unitURIName).c_str());
    }
  }

  return S_OK;
}

HRESULT TestUnitHelper::runTestCaseDefaults()
{
  log(L"\n\nInit an object unit configuration with default values...");
  ObjUnitConfig dest;
  log(L"\n  try linear object...");
  UnitHelper::initObjUnitConfig(UnitObjType::LINEAR, dest);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::POSITION)], L"\nUnexpected unit found!", Unit::MILLIMETER);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::VELOCITY)], L"\nUnexpected unit found!", Unit::MILLIMETER_PER_MINUTE);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::ACCELERATION)], L"\nUnexpected unit found!", Unit::METER_PER_SECOND_SQARED);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::JERK)], L"\nUnexpected unit found!", Unit::METER_PER_SECOND_QUBED);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::FORCE)], L"\nUnexpected unit found!", Unit::NEWTON);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::TORQUE)], L"\nUnexpected unit found!", Unit::NEWTON_METERS);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::TIME)], L"\nUnexpected unit found!", Unit::SECOND);

  log(L"\n  try rotational object...");
  UnitHelper::initObjUnitConfig(UnitObjType::ROTATIONAL, dest);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::POSITION)], L"\nUnexpected unit found!", Unit::DEGREE);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::VELOCITY)], L"\nUnexpected unit found!", Unit::REVOLUTIONS_PER_MINUTE);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::ACCELERATION)], L"\nUnexpected unit found!", Unit::REVOLUTIONS_PER_MINUTE_SQARED);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::JERK)], L"\nUnexpected unit found!", Unit::REVOLUTIONS_PER_MINUTE_QUBED);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::FORCE)], L"\nUnexpected unit found!", Unit::NEWTON);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::TORQUE)], L"\nUnexpected unit found!", Unit::NEWTON_METERS);
  checkUnit(dest.units[static_cast<std::size_t>(UnitValueType::TIME)], L"\nUnexpected unit found!", Unit::SECOND);

  return S_OK;
}

bool TestUnitHelper::checkDoubleValue(double value, const WCHAR* comment, double expected, double epsilon)
{
  if (((value >= (expected)) && (value - (expected) > epsilon)) ||
      ((value < (expected)) && ((expected)-value > epsilon)))
  {
    error(L"\n%ls (result %0.4f != %0.4f).", comment, value, expected);
    errorSilent();
    return false;
  }
  return true;
}

bool TestUnitHelper::checkBoolResult(bool value, const WCHAR* comment, bool expected)
{
  if (value != expected)
  {
    error(L"\n%ls:", comment);
    errorSilent();
    return false;
  }
  log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");
  return true;
}

bool TestUnitHelper::checkUnit(Unit value, const WCHAR* comment, Unit expected)
{
  if (value != expected)
  {
    error(L"\n%ls:", comment);
    errorSilent();
    return false;
  }
  return true;
}

HRESULT TestUnitHelper::runErrorTestCase()
{
  log(L"\n\n<b>---Run error test case ---</b>");

  double dummy;
  log(L"\nTry conversion with invalid unit...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, static_cast<Unit>(12345), dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with non fitting unit type...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, Unit::FEET, dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with non fitting object type...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, Unit::RADIANS_PER_SECOND, dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with invalid URI string...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, std::string("Krabumbel"), dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with invalid abbreviation string...");
  checkBoolResult(UnitHelper::convertToSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, "Krabumbel", dummy), L"\nUnexpected conversion success!", false);

  log(L"\nTry conversion with invalid unit...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, static_cast<Unit>(12345), dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with non fitting unit type...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, Unit::FEET, dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with non fitting object type...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, Unit::RADIANS_PER_SECOND, dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with invalid URI string...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, std::string("Krabumbel"), dummy), L"\nUnexpected conversion success!", false);
  log(L"\nTry conversion with invalid abbreviation string...");
  checkBoolResult(UnitHelper::convertFromSI(UnitObjType::LINEAR, UnitValueType::VELOCITY, 1.0, "Krabumbel", dummy), L"\nUnexpected conversion success!", false);

  log(L"\nTry conversion enum to string with invalid enum...");
  if (UnitHelper::unitToString(static_cast<Unit>(12345)) != nullptr)
  {
    error(L"\nUnexpected conversion success!");
  }

  return S_OK;
}


HRESULT TestUnitHelper::DummyUnit::runTest(xTST_Test* base)
{
  base->log(L"\n\n<b>---Run protected class test case ---</b>");

  double tmp;
  base->log(L"\nCall findFactor() with valid unit..");
  if (!findFactor(Unit::METER, tmp))
  {
    base->error(L"\nUnexpected result!");
  }

  base->log(L"\nCall findFactor() with invalid unit..");
  if (findFactor(static_cast<Unit>(12345), tmp))
  {
    base->error(L"\nUnexpected result!");
  }

  return S_OK;
}

