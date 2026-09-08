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
#include "unit_helper_init.h"

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

// we create an instance of the initializer which will run the constructor, that fills the static member in UnitHelper with data
UnitHelperInit unitInitializer;


// ---------------------------------------------------------------------------
// Implementation of classes and structures
// ---------------------------------------------------------------------------

UnitHelperInit::UnitHelperInit()
{
  defineLinearUnits();
  defineRotationalUnits();
  defineForceUnits();
  defineTorqueUnits();
  defineTimeUnits();
}

void UnitHelperInit::defineUnit(Unit unit, const UnitHelper::ConversionData& data)
{
  s_convData[static_cast<std::size_t>(unit)] = data;
}

void UnitHelperInit::defineLinearUnits()
{
  defineLinearPositionUnits();
  defineLinearVelocityUnits();
  defineLinearAccelerationUnits();
  defineLinearJerkUnits();
}

void UnitHelperInit::defineRotationalUnits()
{
  defineRotationalPositionUnits();
  defineRotationalVelocityUnits();
  defineRotationalAccelerationUnits();
  defineRotationalJerkUnits();
}

void UnitHelperInit::defineLinearPositionUnits()
{
  // --------------- position/length units linear ----------------------
  defineUnit(Unit::KILOMETER,                 {   KILO,         "kilometer",                "km",     "km",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::METER,                     {    1.0,         "meter",                    "m",      "m",      UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::CENTIMETER,                {  CENTI,         "centimeter",               "cm",     "cm",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::MILLIMETER,                {  MILLI,         "millimeter",               "mm",     "mm",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::MICROMETER,                {  MICRO,         "micrometer",               "um",     "um",     UnitObjType::LINEAR,    UnitValueType::POSITION });

  defineUnit(Unit::MILE,                      {   MILE,         "mile",                     "mi",     "mi",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::YARD,                      {   YARD,         "yard",                     "yd",     "yd",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::FEET,                      {   FEET,         "feet",                     "ft",     "ft",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::INCH,                      {   INCH,         "inch",                     "in",     "in",     UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::THOUSANDTH_OF_AN_INCH,     { MILLI * INCH,   "thousandth of an inch",    "thou",   "thou",   UnitObjType::LINEAR,    UnitValueType::POSITION });
  defineUnit(Unit::THOUSANDTH_OF_AN_INCH_US,  { MILLI * INCH,   "thousandth of an inch",    "mil",    "mil",    UnitObjType::LINEAR,    UnitValueType::POSITION });
}

void UnitHelperInit::defineLinearVelocityUnits()
{
  // --------------- velocity units linear ----------------------
  defineUnit(Unit::METER_PER_SECOND,          {    1.0,         "meter per second",         "m/s",    "m-per-s",    UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::METER_PER_MINUTE,          {  1.0 / MINUTE,  "meter per minute",         "m/min",  "m-per-min",  UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::MILLIMETER_PER_SECOND,     {  MILLI,         "millimeter per second",    "mm/s",   "mm-per-s",   UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::MILLIMETER_PER_MINUTE,     { MILLI / MINUTE, "millimeter per minute",    "mm/min", "mm-per-min", UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::KILOMETER_PER_HOUR,        { KILO / HOUR,    "kilometer per hour",       "km/h",   "km-per-h",   UnitObjType::LINEAR,    UnitValueType::VELOCITY });

  defineUnit(Unit::INCH_PER_SECOND,           {   INCH,         "inch per second",          "in/s",   "in-per-s",   UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::INCH_PER_MINUTE,           { INCH / MINUTE,  "inch per minute",          "in/min", "in-per-min", UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::FEET_PER_SECOND,           {   FEET,         "feet per second",          "ft/s",   "ft-per-s",   UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::FEET_PER_MINUTE,           { FEET / MINUTE,  "feet per minute",          "ft/min", "ft-per-min", UnitObjType::LINEAR,    UnitValueType::VELOCITY });
  defineUnit(Unit::MILES_PER_HOUR,            { MILE / HOUR,    "miles per hour",           "mph",    "mph",        UnitObjType::LINEAR,    UnitValueType::VELOCITY });
}

void UnitHelperInit::defineLinearAccelerationUnits()
{
  // --------------- acceleration units linear ----------------------
  defineUnit(Unit::METER_PER_SECOND_SQARED,       {    1.0,             "meter per second squared",       "m/s^2",    "m-per-s^2",    UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::METER_PER_MINUTE_SQARED,       {  1.0 / MINUTE_SQ,   "meter per minute squared",       "m/min^2",  "m-per-min^2",  UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::CENTIMETER_PER_SECOND_SQARED,  {  CENTI,             "centimeter per second squared",  "cm/s^2",   "cm-per-s^2",   UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::MILLIMETER_PER_SECOND_SQARED,  {  MILLI,             "millimeter per second squared",  "mm/s^2",   "mm-per-s^2",   UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::MILLIMETER_PER_MINUTE_SQARED,  { MILLI / MINUTE_SQ,  "millimeter per minute squared",  "mm/min^2", "mm-per-min^2", UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::STANDARD_GRAVITY,              { GRAVITY,            "standard gravity",               "g",        "g",            UnitObjType::LINEAR,    UnitValueType::ACCELERATION });

  defineUnit(Unit::INCH_PER_SECOND_SQARED,        {    INCH,            "inch per second squared",        "in/s^2",   "in-per-s^2",   UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
  defineUnit(Unit::FEET_PER_SECOND_SQARED,        {    FEET,            "feet per second squared",        "ft/s^2",   "ft-per-s^2",   UnitObjType::LINEAR,    UnitValueType::ACCELERATION });
}

void UnitHelperInit::defineLinearJerkUnits()
{
  // --------------- jerk units linear ----------------------
  defineUnit(Unit::METER_PER_SECOND_QUBED,        {    1.0,             "meter per second qubed",         "m/s^3",    "m-per-s^3",    UnitObjType::LINEAR,    UnitValueType::JERK });
  defineUnit(Unit::METER_PER_MINUTE_QUBED,        {  1.0 / MINUTE_QU,   "meter per minute",               "m/min^3",  "m-per-min^3",  UnitObjType::LINEAR,    UnitValueType::JERK });
  defineUnit(Unit::MILLIMETER_PER_SECOND_QUBED,   {  MILLI,             "millimeter per second",          "mm/s^3",   "mm-per-s^3",   UnitObjType::LINEAR,    UnitValueType::JERK });
  defineUnit(Unit::MILLIMETER_PER_MINUTE_QUBED,   { MILLI / MINUTE_QU,  "millimeter per minute",          "mm/min^3", "mm-per-min^3", UnitObjType::LINEAR,    UnitValueType::JERK });
  defineUnit(Unit::STANDARD_GRAVITY_PER_SECOND,   { GRAVITY,            "standard gravity per second",    "g/s",      "g-per-s",      UnitObjType::LINEAR,    UnitValueType::JERK });

  defineUnit(Unit::INCH_PER_SECOND_QUBED,        {     INCH,            "inch per second qubed",          "in/s^3",   "in-per-s^3",   UnitObjType::LINEAR,    UnitValueType::JERK });
  defineUnit(Unit::FEET_PER_SECOND_QUBED,        {     FEET,            "feet per second qubed",          "ft/s^3",   "ft-per-s^3",   UnitObjType::LINEAR,    UnitValueType::JERK });
}

void UnitHelperInit::defineRotationalPositionUnits()
{
  // --------------- position/length units rotational ----------------------
  defineUnit(Unit::RADIANS,                   { RAD_TO_DEG,              "radians",             "rad",    "rad",     UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::MILLIRADIANS,              {MILLI * RAD_TO_DEG,       "milliradians",        "mrad",   "mrad",    UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::DEGREE,                    {      1.0,                "degree",              "deg",    "deg",     UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::MILLIDEGREE,               {    MILLI,                "millidegree",         "mdeg",   "mdeg",    UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::GRADIANS,                  { TURN / 400.0,            "gradians",            "grad",   "grad",    UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::MILLIGRADIANS,             {MILLI * TURN / 400.0,     "milligradians",       "mgrad",  "mgrad",   UnitObjType::ROTATIONAL, UnitValueType::POSITION });

  defineUnit(Unit::ARC_MINUTE,                {    1.0 / MINUTE,         "arcminute",           "\'",     "arcmin",  UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::ARC_SECOND,                {    1.0 / HOUR,           "arcsecond",           "\"",     "arcsec",  UnitObjType::ROTATIONAL, UnitValueType::POSITION });

  defineUnit(Unit::TURN,                      {      TURN,               "turn",                "tr",     "tr",      UnitObjType::ROTATIONAL, UnitValueType::POSITION });
  defineUnit(Unit::REVOLUTION,                {      TURN,               "revolution",          "rev",    "rev",     UnitObjType::ROTATIONAL, UnitValueType::POSITION });
}

void UnitHelperInit::defineRotationalVelocityUnits()
{
  // --------------- velocity units rotational ----------------------
  defineUnit(Unit::RADIANS_PER_SECOND,        { RAD_TO_DEG,                 "radians per second",     "rad/s",    "rad-per-s",      UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::RADIANS_PER_MINUTE,        { RAD_TO_DEG / MINUTE,        "radians per minute",     "rad/min",  "rad-per-min",    UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::DEGREE_PER_SECOND,         { 1.0,                        "degree per second",      "deg/s",    "deg-per-s",      UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::DEGREE_PER_MINUTE,         { 1.0 / MINUTE,               "degree per minute",      "deg/min",  "deg-per-min",    UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::GRADIANS_PER_SECOND,       { TURN / 400.0,               "gradians per second",    "grad/s",   "grad-per-s",     UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::GRADIANS_PER_MINUTE,       { TURN / 400.0 / MINUTE,      "gradians per minute",    "grad/min", "grad-per-min",   UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::ARC_MINUTE_PER_SECOND,     { 1.0 / MINUTE,               "arcminute per second",   "\'/s",     "arcmin-per-s",   UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::ARC_MINUTE_PER_MINUTE,     { 1.0 / MINUTE / MINUTE,      "arcminute per minute",   "\'/min",   "arcmin-per-min", UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::ARC_SECOND_PER_SECOND,     { 1.0 / HOUR,                 "arcsecond per second",   "\"/s",     "arcsec-per-s",   UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::ARC_SECOND_PER_MINUTE,     { 1.0 / HOUR / MINUTE,        "arcsecond per minute",   "\"/min",   "arcsec-per-min", UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::REVOLUTIONS_PER_SECOND,    { TURN,                       "revolutions per second", "rev/s",    "rev-per-s",      UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
  defineUnit(Unit::REVOLUTIONS_PER_MINUTE,    { TURN/ MINUTE,               "revolutions per minute", "rpm",      "rpm",            UnitObjType::ROTATIONAL, UnitValueType::VELOCITY });
}

void UnitHelperInit::defineRotationalAccelerationUnits()
{
  // --------------- acceleration units rotational ----------------------
  defineUnit(Unit::RADIANS_PER_SECOND_SQARED,     { RAD_TO_DEG,                     "radians per second squared",     "rad/s^2",   "rad-per-s^2",       UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::RADIANS_PER_MINUTE_SQARED,     { RAD_TO_DEG / MINUTE_SQ,         "radians per minute squared",     "rad/min^2", "rad-per-min^2",     UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::DEGREE_PER_SECOND_SQARED,      { 1.0,                            "degree per second squared",      "deg/s^2",   "deg-per-s^2",       UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::DEGREE_PER_MINUTE_SQARED,      { 1.0 / MINUTE_SQ,                "degree per minute squared",      "deg/min^2", "deg-per-min^2",     UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::GRADIANS_PER_SECOND_SQARED,    { TURN / 400.0,                   "gradians per second squared",    "grad/s^2",  "grad-per-s^2",      UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::GRADIANS_PER_MINUTE_SQARED,    { TURN / 400.0 / MINUTE_SQ,       "gradians per minute squared",    "grad/min^2","grad-per-min^2",    UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::ARC_MINUTE_PER_SECOND_SQARED,  { 1.0 / MINUTE,                   "arcminute per second squared",   "\'/s^2",    "arcmin-per-s^2",    UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::ARC_MINUTE_PER_MINUTE_SQARED,  { 1.0 / MINUTE / MINUTE_SQ,       "arcminute per minute squared",   "\'/min^2",  "arcmin-per-min^2",  UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::ARC_SECOND_PER_SECOND_SQARED,  { 1.0 / HOUR,                     "arcsecond per second squared",   "\"/s^2",    "arcsec-per-s^2",    UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::ARC_SECOND_PER_MINUTE_SQARED,  { 1.0 / HOUR / MINUTE_SQ,         "arcsecond per minute squared",   "\"/min^2",  "arcsec-per-min^2",  UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::REVOLUTIONS_PER_SECOND_SQARED, { TURN,                           "revolutions per second squared", "rev/s^2",   "rev-per-s^2",       UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
  defineUnit(Unit::REVOLUTIONS_PER_MINUTE_SQARED, { TURN / MINUTE_SQ,               "revolutions per minute squared", "rev/min^2", "rev-per-min^2",     UnitObjType::ROTATIONAL, UnitValueType::ACCELERATION });
}

void UnitHelperInit::defineRotationalJerkUnits()
{
  // --------------- jerk units rotational ----------------------
  defineUnit(Unit::RADIANS_PER_SECOND_QUBED,      { RAD_TO_DEG,                     "radians per second qubed",     "rad/s^3",    "rad-per-s^3",      UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::RADIANS_PER_MINUTE_QUBED,      { RAD_TO_DEG / MINUTE_QU,         "radians per minute qubed",     "rad/min^3",  "rad-per-min^3",    UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::DEGREE_PER_SECOND_QUBED,       { 1.0,                            "degree per second qubed",      "deg/s^3",    "deg-per-s^3",      UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::DEGREE_PER_MINUTE_QUBED,       { 1.0 / MINUTE_QU,                "degree per minute qubed",      "deg/min^3",  "deg-per-min^3",    UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::GRADIANS_PER_SECOND_QUBED,     { TURN / 400.0,                   "gradians per second qubed",    "grad/s^3",   "grad-per-s^3",     UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::GRADIANS_PER_MINUTE_QUBED,     { TURN / 400.0 / MINUTE_QU,       "gradians per minute qubed",    "grad/min^3", "grad-per-min^3",   UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::ARC_MINUTE_PER_SECOND_QUBED,   { 1.0 / MINUTE,                   "arcminute per second qubed",   "\'/s^3",     "arcmin-per-s^3",   UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::ARC_MINUTE_PER_MINUTE_QUBED,   { 1.0 / MINUTE / MINUTE_QU,       "arcminute per minute qubed",   "\'/min^3",   "arcmin-per-min^3", UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::ARC_SECOND_PER_SECOND_QUBED,   { 1.0 / HOUR,                     "arcsecond per second qubed",   "\"/s^3",     "arcsec-per-s^3",   UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::ARC_SECOND_PER_MINUTE_QUBED,   { 1.0 / HOUR / MINUTE_QU,         "arcsecond per minute qubed",   "\"/min^3",   "arcsec-per-min^3", UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::REVOLUTIONS_PER_SECOND_QUBED,  { TURN,                           "revolutions per second qubed", "rev/s^3",    "rev-per-s^3",      UnitObjType::ROTATIONAL, UnitValueType::JERK });
  defineUnit(Unit::REVOLUTIONS_PER_MINUTE_QUBED,  { TURN / MINUTE_QU,               "revolutions per minute qubed", "rev/min^3",  "rev-per-min^3",    UnitObjType::ROTATIONAL, UnitValueType::JERK });
}

void UnitHelperInit::defineForceUnits()
{
  // --------------- force units ----------------------
  defineUnit(Unit::NEWTON,                      {    1.0,               "Newton",             "N",    "N",     UnitObjType::ANY, UnitValueType::FORCE });
  defineUnit(Unit::MILLI_NEWTON,                {  MILLI,               "millinewtons",       "mN",   "mN",    UnitObjType::ANY, UnitValueType::FORCE });
  defineUnit(Unit::KILO_NEWTON,                 {   KILO,               "kilonewtons",        "kN",   "kN",    UnitObjType::ANY, UnitValueType::FORCE });
  defineUnit(Unit::MEGA_NEWTON,                 {   MEGA,               "meganewtons",        "MN",   "MN",    UnitObjType::ANY, UnitValueType::FORCE });

  defineUnit(Unit::DYNE,                        {    1E-5,              "dyne",               "dyn",  "dyn",   UnitObjType::ANY, UnitValueType::FORCE });
  defineUnit(Unit::POUND_FORCE,                 { POUND * GRAVITY,      "pound-force",        "lbf",  "lbf",   UnitObjType::ANY, UnitValueType::FORCE });
  defineUnit(Unit::POUNDAL,                     { 0.138254954376,       "poundal",            "pdl",  "pdl",   UnitObjType::ANY, UnitValueType::FORCE });
}

void UnitHelperInit::defineTorqueUnits()
{
  // --------------- torque units ----------------------
  defineUnit(Unit::NEWTON_METERS,               { 1.0,                      "newton-meter",       "Nm",     "Nm",     UnitObjType::ANY, UnitValueType::TORQUE });
  defineUnit(Unit::KILO_NEWTON_METERS,          { KILO,                     "kilo-newton-meter",  "kNm",    "kNm",    UnitObjType::ANY, UnitValueType::TORQUE });

  defineUnit(Unit::POUND_FOOT,                  { POUND * GRAVITY * FEET,   "pound-foot",         "lbf*ft", "lbf*ft", UnitObjType::ANY, UnitValueType::TORQUE });
  defineUnit(Unit::POUND_INCH,                  { POUND * GRAVITY * INCH,   "pound-inch",         "lbf*in", "lbf*in", UnitObjType::ANY, UnitValueType::TORQUE });
}

void UnitHelperInit::defineTimeUnits()
{
  defineUnit(Unit::SECOND,                    {    1.0,         "second",                   "s",   "s",        UnitObjType::ANY,    UnitValueType::TIME });
  defineUnit(Unit::MINUTE,                    { MINUTE,         "minute",                   "min", "min",      UnitObjType::ANY,    UnitValueType::TIME });
  defineUnit(Unit::HOUR,                      {   HOUR,         "hour",                     "h",   "h",        UnitObjType::ANY,    UnitValueType::TIME });
  defineUnit(Unit::MILLISECOND,               {  MILLI,         "millisecond",              "ms",  "ms",       UnitObjType::ANY,    UnitValueType::TIME });
  defineUnit(Unit::MICROSECOND,               {  MICRO,         "microsecond",              "us",  "us",       UnitObjType::ANY,    UnitValueType::TIME });
}

