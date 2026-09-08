#pragma once

// ---------------------------------------------------------------------------
//
//! @file       unit_helper_init.h
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
//! This file is part of the motion library and defines the initializations class of the
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
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class UnitHelperInit
    //! This is the initializations class for conversion of units.
    class UnitHelperInit : public UnitHelper
    {
    public: // methods
      //! @brief constructor, that initializes the basic data
      UnitHelperInit();

      //! @brief Destructor.
      virtual ~UnitHelperInit() noexcept override = default;

    private: // methods

      //! @brief little helper to define one single unit
      //! @param[in]   unit   unit to be defined
      //! @param[in]   data   conversion data of the new unit
      void defineUnit(Unit unit, const UnitHelper::ConversionData& data);

      //! @brief define all linear units here
      void defineLinearUnits();

      //! @brief define all linear position units here
      void defineLinearPositionUnits();

      //! @brief define all linear velocity units here
      void defineLinearVelocityUnits();

      //! @brief define all linear acceleration units here
      void defineLinearAccelerationUnits();

      //! @brief define all linear jerk units here
      void defineLinearJerkUnits();



      //! @brief define all rotational units here
      void defineRotationalUnits();

      //! @brief define all rotational position units here
      void defineRotationalPositionUnits();

      //! @brief define all rotational velocity units here
      void defineRotationalVelocityUnits();

      //! @brief define all rotational acceleration units here
      void defineRotationalAccelerationUnits();

      //! @brief define all rotational jerk units here
      void defineRotationalJerkUnits();





      //! @brief define all force units here
      void defineForceUnits();

      //! @brief define all torque units here
      void defineTorqueUnits();

      //! @brief define all time units here
      void defineTimeUnits();

    private: // data

      static constexpr double MICRO = 1E-6;               //!< factor for suffix micro (1E-6)
      static constexpr double MILLI = 1E-3;               //!< factor for suffix milli (1E-3)
      static constexpr double CENTI = 1E-2;               //!< factor for suffix centi (1E-2)
      static constexpr double KILO = 1E3;                 //!< factor for suffix kilo (1E3)
      static constexpr double MEGA = 1E6;                 //!< factor for suffix mega (1E6)

      static constexpr double INCH = 0.0254;              //!< factor for inch -> meter
      static constexpr double FEET = INCH * 12.0;         //!< factor for feet -> meter
      static constexpr double YARD = FEET * 3.0;          //!< factor for yard -> meter
      static constexpr double MILE = YARD * 1760.0;       //!< factor for mile -> meter

      static constexpr double TURN = 360.0;               //!< factor for a whole turn -> degree (360°)

      static constexpr double GRAVITY = 9.80665;          //!< factor for standard gravity -> m/s² (≈9.8)

      static constexpr double MINUTE = 60.0;              //!< factor for a minute (time) -> seconds (60)
      static constexpr double HOUR = MINUTE * 60.0;       //!< factor for an hour (time) -> seconds (3600)

      static constexpr double MINUTE_SQ = MINUTE * MINUTE;            //!< factor for a minute (time) squared
      static constexpr double MINUTE_QU = MINUTE * MINUTE * MINUTE;   //!< factor for a minute (time) qubed

      static constexpr double POUND = 0.453592370;        //!< factor for pound (mass) -> kilograms (≈0.45)   [used to build other units]

    };
  }
}
