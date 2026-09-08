#pragma once

// ---------------------------------------------------------------------------
//
//! @file       unit_helper.h
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
//! This file is part of the motion library and defines the interface of the
//! unit helper.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <array>
#include <vector>
#include <string>

#include "unit_defs.h"

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

    //! @class UnitHelper
    //! This is the class for coversion of units.
    class UnitHelper
    {
    public: // types

      //! @brief 

    public: // methods
      //! @brief Destructor.
      virtual ~UnitHelper() noexcept = default;

      //! @brief This method returns the URIs for all available units for a given object type and value type.
      //! would be called by a browse node
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @return                       returns the URIs for all available units for a given object type and value type
      static std::vector<const char*> getAvailableUnitsURI(UnitObjType objType, UnitValueType valueType);

      //! @brief Convert a given value in a given unit to SI units.
      //! would be used to convert the values of a command in the cmdIF. unitValue comes from the general configuration
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      value         value to be converted
      //! @param[in]      valueUnit     unit of the given value
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertToSI(UnitObjType objType, UnitValueType valueType, double value, Unit valueUnit, double &dest);

      //! @brief Convert a given value in a given unit to SI units.
      //! would be used to convert configuration values, that are written in datalayer (single access). valueUnitURI is the last part of the address
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      value         value to be converted
      //! @param[in]      valueUnitURI  unit URI abbreviation of the given value
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertToSI(UnitObjType objType, UnitValueType valueType, double value, const std::string &valueUnitURI, double& dest);

      //! @brief Convert a given value in a given unit to SI units.
      //! would be used to convert configuration values, that are written in datalayer as part of a flatbuffer. valueUnit is provided in the flatbuffer too.
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      value         value to be converted
      //! @param[in]      valueUnit     unit abbreviation of the given value
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertToSI(UnitObjType objType, UnitValueType valueType, double value, const char* valueUnit, double& dest);

      //! @brief Check if the given unit fits the given unit object type and the given unit value type
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      valueUnit     unit to be checked
      //! @return                       true when the given unit fits the given unit object type and the given unit value type
      static bool validateUnit(UnitObjType objType, UnitValueType valueType, Unit valueUnit);

      //! @brief conversion of URI unit string to unit enum
      //! @param[in]      unitURI       unit string
      //! @param[out]     dest          unit as enum
      //! @return                       true on success
      static bool uriStringToEnum(const char* unitURI, Unit& dest);

      //! @brief conversion of unit abbreviation string to unit enum
      //! can be used by a bundle, the converts the units
      //! @param[in]      unit          unit string
      //! @param[out]     dest          unit as enum
      //! @return                       true on success
      static bool stringToEnum(const char* unit, Unit& dest);

      //! @brief Convert a given value in SI units to requested units.
      //! would be used to convert the values that are reading from states (in sysIF). unitValue comes from the general configuration
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      siValue       value in SI units to be converted
      //! @param[in]      valueUnit     requested unit
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, Unit valueUnit, double& dest);

      //! @brief Convert a given value in SI units to requested units.
      //! would be used to convert configuration/state values, that are read in datalayer (single access). valueUnitURI is the last part of the address
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      siValue       value in SI units to be converted
      //! @param[in]      valueUnitURI  requested unit abbreviation URI
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, const std::string& valueUnitURI, double& dest);

      //! @brief Convert a given value in SI units to requested units.
      //! would be used to convert configuration/state values, that are read in datalayer as part of a flatbuffer. valueUnit is provided in the flatbuffer too.
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      siValue       value in SI units to be converted
      //! @param[in]      valueUnit     requested unit abbreviation
      //! @param[out]     dest          destination to put the converted value
      //! @return                       true on success
      static bool convertFromSI(UnitObjType objType, UnitValueType valueType, double siValue, const char* valueUnit, double& dest);

      //! @brief conversion of unit enum to URI unit string
      //! @param[in]      unit          unit enum
      //! @return                       returns the URI string of the given unit
      static const char* unitToURIString(Unit unit);

      //! @brief conversion of unit enum to unit abbreviation string
      //! @param[in]      unit          unit enum
      //! @return                       returns the abbreviation string of the given unit
      static const char* unitToString(Unit unit);

      //! @brief conversion of unit enum to unit name string
      //! could be used to get the real name of a unit (provided in datalayer). can be shown as tooltip in HMI.
      //! @param[in]      unit          unit enum
      //! @return                       returns the name string of the given unit
      static const char* unitToNameString(Unit unit);

      //! @brief get the type data of a given unit
      //! @param[in]      unit          unit enum
      //! @param[out]     objType       object type (linear/rotational) for the given unit
      //! @param[out]     valueType     value type (position/acceleration/time/...) for the given unit
      //! @return                       returns true, when the unit was found
      static bool getUnitData(Unit unit, UnitObjType &objType, UnitValueType &valueType);


      //! @brief initialize an object unit configuration with defaults fitting to the unit object type
      //! @param[in]      objType       object type (linear/rotational) for the given unit
      //! @param[out]     dest          destination to put the converted value
      static void initObjUnitConfig(UnitObjType objType, ObjUnitConfig &dest);


      //! @brief This method returns names of all unit types (equal to the elements in the general unit configuration).
      //! would be called by a browse node
      //! @return                       returns names of all unit types
      static std::vector<const char*> getUnitTypes();

    protected: // types

      //! @brief data structure for one single unit
      struct ConversionData
        {
        double m_toSIFactor{};        //!< factor to convert the unit to SI units
        const char* m_unitStr{};      //!< name of the unit
        const char* m_unitAbbr{};     //!< abbreviation of the unit
        const char* m_unitAbbrURI{};  //!< abbreviation of the unit as URI
        UnitObjType m_objType{};      //!< object type (linear/rotational); used to validate
        UnitValueType m_valueType{};  //!< value type (position/velocity/time ...); used to validate
      };

    protected: // member

      //! @brief find the conversion factor for a given unit
      //! @param[in]      objType       object type
      //! @param[in]      valueType     value type
      //! @param[in]      valueUnit     requested unit
      //! @param[out]     factorToSI    destination to put the conversion factor
      //! @return                       true on success
      static bool findFactor(UnitObjType objType, UnitValueType valueType, Unit valueUnit, double& factorToSI);

      //! @brief find the conversion factor for a given unit without checks for the correct object type and unit type
      //! @param[in]      valueUnit     requested unit
      //! @param[out]     factorToSI    destination to put the conversion factor
      //! @return                       true on success
      static bool findFactor(Unit valueUnit, double& factorToSI);

      //! @brief find the conversion data for a given unit (no check for ObjType and UnitType)
      //! @param[in]      valueUnit     requested unit abbreviation
      //! @return                       conversion data for a given unit or nullptr, when not found
      static const ConversionData* findData(Unit valueUnit);

    protected: // data

      //! data of all supported units
      static std::array<ConversionData, static_cast<std::size_t>(Unit::COUNT)> s_convData;
    };
  }
}
