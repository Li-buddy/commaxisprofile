#pragma once

// ---------------------------------------------------------------------------
//
//! @file       unit_defs.h
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
//! This file is part of the motion library and defines the constants and structures for the
//! unit helper.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cstdint>      // for std::uint32_t
#include <cstddef>      // for std::size_t
#include <cstring>      // for std::strcmp()
#include <array>

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

    constexpr double SEMICIRCLE_DEG = 180.0;            //!< 180 (degrees)
    constexpr double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406L;                     //!< PI 
    constexpr double HALF_PI = PI * 0.5;                //!< 0.5 * PI
    constexpr double RAD_TO_DEG = SEMICIRCLE_DEG / PI;  //!< conversion radiant to degrees
    constexpr double DEG_TO_RAD = PI / SEMICIRCLE_DEG;  //!< conversion degrees to radiant

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    //! @brief enum for all basic data types, that require units
    //! Extend this always at the end before COUNT (to stay compatible)!
    //! Extend UNIT_VALUE_TYPE_STR too.
    enum class UnitValueType: std::uint32_t
    {
      POSITION = 0,   //!< position type (mm; °; inch; ...)
      VELOCITY,       //!< velocity type (m/s; rpm; ...)
      ACCELERATION,   //!< acceleration type (mm/s²; ...)
      JERK,           //!< jerk type (m/s³; ...)
      FORCE,          //!< force type (N)
      TORQUE,         //!< torque type (N*m)
      TIME,           //!< time type (s; min; ms)

      COUNT           //!< this must alway be the last entry
    };

    //! @brief strings for all the basic data types, that require units; must have the same size and sequence as UnitValueType
    const std::array<const char*, static_cast<std::size_t>(UnitValueType::COUNT)> UNIT_VALUE_TYPE_STR{
      "position+length",
      "velocity",
      "acceleration",
      "jerk",
      "force",
      "torque",
      "time",
    };

    //! @brief enum for all motion object types, that require different units
    //! Extend this always at the end before COUNT (to stay compatible)!
    enum class UnitObjType : std::uint32_t
    {
      LINEAR,         //!< linear units motion object type (linear axis, xyz of a kinematics)
      ROTATIONAL,     //!< rotational units motion object type (rotational axis, Euler angles of a PCS)
      ANY,            //!< units that can be used on linear and rotational motion objects
      COUNT           //!< this must alway be the last entry
    };

    //! @brief enum for all known units for faster access
    //! Extend this always at the end or replace RESERVED placeholder (to stay compatible)!
    enum class Unit : std::uint32_t
    {
      // --------------- position/length units linear ----------------------
      KILOMETER,                  //!< km
      METER,                      //!< m
      CENTIMETER,                 //!< cm
      MILLIMETER,                 //!< mm
      MICROMETER,                 //!< µm

      RESERVED01,                 //!< reserved for later usage
      RESERVED02,                 //!< reserved for later usage
      RESERVED03,                 //!< reserved for later usage
      RESERVED04,                 //!< reserved for later usage
      RESERVED05,                 //!< reserved for later usage

      MILE,                       //!< mile
      YARD,                       //!< yd (use international definition)
      FEET,                       //!< ft
      INCH,                       //!< in
      THOUSANDTH_OF_AN_INCH,      //!< thou
      THOUSANDTH_OF_AN_INCH_US,   //!< mil

      RESERVED06,                 //!< reserved for later usage
      RESERVED07,                 //!< reserved for later usage
      RESERVED08,                 //!< reserved for later usage
      RESERVED09,                 //!< reserved for later usage
      RESERVED10,                 //!< reserved for later usage

      // ---------------------- position/length units rotational ----------------------
      RADIANS,                    //!< rad
      MILLIRADIANS,               //!< mrad
      DEGREE,                     //!< °
      MILLIDEGREE,                //!< m°
      GRADIANS,                   //!< grad
      MILLIGRADIANS,              //!< mgrad

      ARC_MINUTE,                 //!< ' (equals 1/60°)
      ARC_SECOND,                 //!< '' (equals 1/60')

      RESERVED11,                 //!< reserved for later usage
      RESERVED12,                 //!< reserved for later usage
      RESERVED13,                 //!< reserved for later usage
      RESERVED14,                 //!< reserved for later usage
      RESERVED15,                 //!< reserved for later usage

      TURN,                       //!< tr  (same as revolution)
      REVOLUTION,                 //!< rev (same as turn)

      RESERVED16,                 //!< reserved for later usage
      RESERVED17,                 //!< reserved for later usage
      RESERVED18,                 //!< reserved for later usage
      RESERVED19,                 //!< reserved for later usage
      RESERVED20,                 //!< reserved for later usage

      // ---------------------- velocity units linear ----------------------
      METER_PER_SECOND,           //!< m/s
      METER_PER_MINUTE,           //!< m/min
      MILLIMETER_PER_SECOND,      //!< mm/s
      MILLIMETER_PER_MINUTE,      //!< mm/min
      KILOMETER_PER_HOUR,         //!< km/h

      RESERVED21,                 //!< reserved for later usage
      RESERVED22,                 //!< reserved for later usage
      RESERVED23,                 //!< reserved for later usage
      RESERVED24,                 //!< reserved for later usage
      RESERVED25,                 //!< reserved for later usage

      INCH_PER_SECOND,            //!< in/s
      INCH_PER_MINUTE,            //!< in/min
      FEET_PER_SECOND,            //!< ft/s
      FEET_PER_MINUTE,            //!< ft/min
      MILES_PER_HOUR,             //!< mph

      RESERVED26,                 //!< reserved for later usage
      RESERVED27,                 //!< reserved for later usage
      RESERVED28,                 //!< reserved for later usage
      RESERVED29,                 //!< reserved for later usage
      RESERVED30,                 //!< reserved for later usage

      // ---------------------- velocity units rotational ----------------------
      RADIANS_PER_SECOND,         //!< rad/s
      RADIANS_PER_MINUTE,         //!< rad/min
      DEGREE_PER_SECOND,          //!< °/s
      DEGREE_PER_MINUTE,          //!< °/min
      GRADIANS_PER_SECOND,        //!< grad/s
      GRADIANS_PER_MINUTE,        //!< grad/min
      ARC_MINUTE_PER_SECOND,      //!< '/s (equals 1/60°)
      ARC_MINUTE_PER_MINUTE,      //!< '/min (equals 1/60°)
      ARC_SECOND_PER_SECOND,      //!< ''/s (equals 1/60')
      ARC_SECOND_PER_MINUTE,      //!< ''/min (equals 1/60')
      REVOLUTIONS_PER_SECOND,     //!< rev/s
      REVOLUTIONS_PER_MINUTE,     //!< rpm

      RESERVED31,                 //!< reserved for later usage
      RESERVED32,                 //!< reserved for later usage
      RESERVED33,                 //!< reserved for later usage
      RESERVED34,                 //!< reserved for later usage
      RESERVED35,                 //!< reserved for later usage

      // ---------------------- acceleration units linear ----------------------
      METER_PER_SECOND_SQARED,        //!< m/s²
      METER_PER_MINUTE_SQARED,        //!< m/min²
      CENTIMETER_PER_SECOND_SQARED,   //!< cm/s² == Gal
      MILLIMETER_PER_SECOND_SQARED,   //!< mm/s²
      MILLIMETER_PER_MINUTE_SQARED,   //!< mm/min²
      STANDARD_GRAVITY,               //!< g

      RESERVED36,                 //!< reserved for later usage
      RESERVED37,                 //!< reserved for later usage
      RESERVED38,                 //!< reserved for later usage
      RESERVED39,                 //!< reserved for later usage
      RESERVED40,                 //!< reserved for later usage

      INCH_PER_SECOND_SQARED,         //!< in/s²
      FEET_PER_SECOND_SQARED,         //!< ft/s²

      RESERVED41,                 //!< reserved for later usage
      RESERVED42,                 //!< reserved for later usage
      RESERVED43,                 //!< reserved for later usage
      RESERVED44,                 //!< reserved for later usage
      RESERVED45,                 //!< reserved for later usage

      // ---------------------- acceleration units rotational ----------------------

      RADIANS_PER_SECOND_SQARED,         //!< rad/s²
      RADIANS_PER_MINUTE_SQARED,         //!< rad/min²
      DEGREE_PER_SECOND_SQARED,          //!< °/s²
      DEGREE_PER_MINUTE_SQARED,          //!< °/min²
      GRADIANS_PER_SECOND_SQARED,        //!< grad/s²
      GRADIANS_PER_MINUTE_SQARED,        //!< grad/min²
      ARC_MINUTE_PER_SECOND_SQARED,      //!< '/s² (equals 1/60°)
      ARC_MINUTE_PER_MINUTE_SQARED,      //!< '/min² (equals 1/60°)
      ARC_SECOND_PER_SECOND_SQARED,      //!< ''/s² (equals 1/60')
      ARC_SECOND_PER_MINUTE_SQARED,      //!< ''/min² (equals 1/60')
      REVOLUTIONS_PER_SECOND_SQARED,     //!< rev/s²
      REVOLUTIONS_PER_MINUTE_SQARED,     //!< rev/min²

      RESERVED46,                 //!< reserved for later usage
      RESERVED47,                 //!< reserved for later usage
      RESERVED48,                 //!< reserved for later usage
      RESERVED49,                 //!< reserved for later usage
      RESERVED50,                 //!< reserved for later usage

      // ---------------------- jerk units linear ----------------------
      METER_PER_SECOND_QUBED,        //!< m/s³
      METER_PER_MINUTE_QUBED,        //!< m/min³
      MILLIMETER_PER_SECOND_QUBED,   //!< mm/s³
      MILLIMETER_PER_MINUTE_QUBED,   //!< mm/min³
      STANDARD_GRAVITY_PER_SECOND,   //!< g/s

      RESERVED51,                 //!< reserved for later usage
      RESERVED52,                 //!< reserved for later usage
      RESERVED53,                 //!< reserved for later usage
      RESERVED54,                 //!< reserved for later usage
      RESERVED55,                 //!< reserved for later usage

      INCH_PER_SECOND_QUBED,         //!< in/s³
      FEET_PER_SECOND_QUBED,         //!< ft/s³

      RESERVED56,                 //!< reserved for later usage
      RESERVED57,                 //!< reserved for later usage
      RESERVED58,                 //!< reserved for later usage
      RESERVED59,                 //!< reserved for later usage
      RESERVED60,                 //!< reserved for later usage

      // ---------------------- jerk units rotational ----------------------
      RADIANS_PER_SECOND_QUBED,         //!< rad/s³
      RADIANS_PER_MINUTE_QUBED,         //!< rad/min³
      DEGREE_PER_SECOND_QUBED,          //!< °/s³
      DEGREE_PER_MINUTE_QUBED,          //!< °/min³
      GRADIANS_PER_SECOND_QUBED,        //!< grad/s³
      GRADIANS_PER_MINUTE_QUBED,        //!< grad/min³
      ARC_MINUTE_PER_SECOND_QUBED,      //!< '/s³ (equals 1/60°)
      ARC_MINUTE_PER_MINUTE_QUBED,      //!< '/min³ (equals 1/60°)
      ARC_SECOND_PER_SECOND_QUBED,      //!< ''/s³ (equals 1/60')
      ARC_SECOND_PER_MINUTE_QUBED,      //!< ''/min³ (equals 1/60')
      REVOLUTIONS_PER_SECOND_QUBED,     //!< rev/s³
      REVOLUTIONS_PER_MINUTE_QUBED,     //!< rev/min³

      RESERVED61,                 //!< reserved for later usage
      RESERVED62,                 //!< reserved for later usage
      RESERVED63,                 //!< reserved for later usage
      RESERVED64,                 //!< reserved for later usage
      RESERVED65,                 //!< reserved for later usage

      // ---------------------- force units ----------------------
      NEWTON,                     //!< N
      MILLI_NEWTON,               //!< mN
      KILO_NEWTON,                //!< kN
      MEGA_NEWTON,                //!< MN

      RESERVED66,                 //!< reserved for later usage
      RESERVED67,                 //!< reserved for later usage
      RESERVED68,                 //!< reserved for later usage
      RESERVED69,                 //!< reserved for later usage
      RESERVED70,                 //!< reserved for later usage

      DYNE,                       //!< dyn
      POUND_FORCE,                //!< lbf
      POUNDAL,                    //!< pdl

      RESERVED71,                 //!< reserved for later usage
      RESERVED72,                 //!< reserved for later usage
      RESERVED73,                 //!< reserved for later usage
      RESERVED74,                 //!< reserved for later usage
      RESERVED75,                 //!< reserved for later usage

      // ---------------------- torque units ----------------------
      NEWTON_METERS,              //!< N*m
      KILO_NEWTON_METERS,         //!< kN*m

      RESERVED77,                 //!< reserved for later usage
      RESERVED78,                 //!< reserved for later usage
      RESERVED79,                 //!< reserved for later usage
      RESERVED80,                 //!< reserved for later usage

      POUND_FOOT,                 //!< lbf*ft
      POUND_INCH,                 //!< lbf*in

      RESERVED81,                 //!< reserved for later usage
      RESERVED82,                 //!< reserved for later usage
      RESERVED83,                 //!< reserved for later usage
      RESERVED84,                 //!< reserved for later usage
      RESERVED85,                 //!< reserved for later usage

      // ---------------------- time units ----------------------
      HOUR,                       //!< h
      MINUTE,                     //!< min
      SECOND,                     //!< s
      MILLISECOND,                //!< ms
      MICROSECOND,                //!< µs

      RESERVED86,                 //!< reserved for later usage
      RESERVED87,                 //!< reserved for later usage
      RESERVED88,                 //!< reserved for later usage
      RESERVED89,                 //!< reserved for later usage
      RESERVED90,                 //!< reserved for later usage

      COUNT                       //!< this must alway be the last entry
    };




    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @brief a structure for a general (unit) configuration of a motion object
    struct ObjUnitConfig
    {
      Unit units[static_cast<std::size_t>(UnitValueType::COUNT)]{Unit::COUNT};    //!< default units of the motion object, invalid initialized
    };

    // ---------------------------------------------------------------------------
    // Declaration of functions
    // ---------------------------------------------------------------------------

    //! @brief get a string from a UnitValueType
    //! @param[in]  type   the UnitValueType to be stringified
    //! @return string from the given UnitValueType
    inline const char* unitValueTypeToString(UnitValueType type)
    {
      // return (static_cast<std::size_t>(type) >= UNIT_VALUE_TYPE_STR.size()) ? "invalid" : UNIT_VALUE_TYPE_STR[static_cast<std::size_t>(type)];
      if (static_cast<std::size_t>(type) >= UNIT_VALUE_TYPE_STR.size())
      {
        return "invalid";
      }
      else
      {
        const char* str = UNIT_VALUE_TYPE_STR[static_cast<std::size_t>(type)];
        return str;
      }
    }

    //! @brief get a UnitValueType from a string
    //! @param[in]  typeStr   the UnitValueType as string
    //! @return UnitValueType from the given string (UnitValueType::COUNT on error)
    inline UnitValueType stringToUnitValueType(const char* typeStr)
    {
      for (std::size_t i = 0; i < UNIT_VALUE_TYPE_STR.size(); ++i)
      {
        if (!std::strcmp(typeStr, UNIT_VALUE_TYPE_STR[i]))
        {
          return static_cast<UnitValueType>(i);
        }
      }
      return UnitValueType::COUNT;
    }

    //! @brief get a string from a UnitObjType
    //! @param[in]  type   the UnitObjType to be stringified
    //! @return string from the given UnitObjType
    inline const char* unitObjTypeToString(UnitObjType type)
    {
      switch (type)
      {
      case UnitObjType::LINEAR: return "linear";
      case UnitObjType::ROTATIONAL: return "rotational";
      case UnitObjType::ANY: return "linear+rotational";
      default: return "invalid";
      }
    }
  }
}
