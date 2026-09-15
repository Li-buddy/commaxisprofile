#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_backup_restore_mgr.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and implements the
//! interface of the backup restore manager.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include <cstring>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace datalayer
  {
    class Variant;
  }
}


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
    //! This macro prints the given parameter as string.
    #define _CLASS_NAME_(x)   (           # x )
    //! This macro maps the given parameter to a macro that prints it as string.
    #define CLASS_NAME(x)     (_CLASS_NAME_(x))
    
    
    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------
    //!< load phase
    enum class LoadPhases : std::uint32_t
    {
      QUERY = 0,   //!< check if loading of the file is possible
      PREPARE,     //!< perform preparation steps for loading
      LOAD,        //!< load the given configuration
      VALIDATE,    //!< perform post processing steps
      ACTIVATE,    //!< Establish desired run state of the motion (if phases 1-4 have been finished without problems)
      ABORT,       //!< Do NOT change run state of the device, clean up if required

      COUNT        //!< this must always be the last entry
    };    

    //! @brief Get the target load phase of a persistence operation as string
    //! @param[in]    phase    the load phase of a persistence operation
    //! @return       the load phase of a persistence operation as string
    inline const char* loadPhasetoString(LoadPhases phase)
    {
      switch (phase)
      {
        case LoadPhases::QUERY:     return "QUERY";
        case LoadPhases::PREPARE:   return "PREPARE";
        case LoadPhases::LOAD:      return "LOAD";
        case LoadPhases::VALIDATE:  return "VALIDATE";
        case LoadPhases::ACTIVATE:  return "ACTIVATE";
        case LoadPhases::ABORT:     return "ABORT";
        case LoadPhases::COUNT:     return "UNDEF";
      }
      return "UNDEF";
    }

    //! @brief Get the load phase of a persistence operation from a string (default is QUERY)
    //! Change input string to uppercase since Configuration sends request in lowercase.
    //! @param[in]    phase   the load phase of a persistence operation as string
    //! @return                      the load phase of a persistence operation
    inline LoadPhases stringToLoadPhases(const char* phase)
    {
      std::string phaseStr = phase;
      for (auto& c : phaseStr)
      {
        c = std::toupper(c);
      }
      if (phaseStr == "QUERY")
      {
        return LoadPhases::QUERY;
      }
      if (phaseStr == "PREPARE")
      {
        return LoadPhases::PREPARE;
      }
      if (phaseStr == "LOAD")
      {
        return LoadPhases::LOAD;
      }
      if (phaseStr == "VALIDATE")
      {
        return LoadPhases::VALIDATE;
      }
      if (phaseStr == "ACTIVATE")
      {
        return LoadPhases::ACTIVATE;
      }
      if (phaseStr == "ABORT")
      {
        return LoadPhases::ABORT;
      }
      return LoadPhases::QUERY;
    }
    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------
    
    //! @class IBackupRestoreMgr
    //! This is the interface of the backup/restore manager class.
    class IBackupRestoreMgr
    {
    public: // methods
      virtual ~IBackupRestoreMgr() noexcept = default;

      //! @brief This method returns the class name.
      //!
      //! @return                      Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(IBackupRestoreMgr)); }

      //! @brief Load a single axis profile configuration file.
      //! @param[in]   profileName   Name of the axis profile that should be loaded.
      //! @param[in]   dlCfgUri      Datalayer URI of the configuration node of the desired axis profile.
      //! @param[in]   filePath      File path of the axis profile config JSON file (can be an empty string).
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult load(const std::string& profileName, const std::string& dlCfgUri, const std::string& filePath) = 0;

      //! @brief Store a single axis profile configuration into a JSON file.
      //! @param[in]   profileName   Name of the requested axis profile that should be saved.
      //! @param[in]   dlCfgUri      Datalayer URI of the configuration node of the desired axis profile.
      //! @param[in]   filePath      Target path where the axis profile config JSON file should be stored (can be an empty string).
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult save(const std::string& profileName, const std::string& dlCfgUri, const std::string& filePath) = 0;

      //! @brief Load a complete system configuration including all existing profiles.
      //! @param[in]  folderPath   Folder from which to read the config files (can be an empty string)
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult loadSystemCfg(const std::string& folderPath) = 0;

      //! @brief Store the complete system configuration including all existing profiles.
      //! @param[in]  folderPath   Folder from which to read the config files (can be an empty string)
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult saveSystemCfg(const std::string& folderPath) = 0;

      //! @brief Load a complete system configuration including all existing profiles.
      //! @param[in]  persistenceParam   variant object that holds information for loading 
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult loadSystemCfg(const comm::datalayer::Variant& persistenceParam) = 0;      

      //! @brief Store the complete axisprofile system configuration including all existing axisprofiles.
      //! @param[in]  persistenceParam   variant object that holds information for saving 
      //! @return  Return value indicating success or error.
      virtual AxisProfileResult saveSystemCfg(const comm::datalayer::Variant& persistenceParam) = 0;

      /// @brief Make the changes of the given axisprofile object persistent (e.g. object have been deleted)
      /// @param[in]  persistenceParam    variant object that holds information for saving
      /// @param[in]  axisprofileName     Name of the axisprofile object thats changes should be made persistent.
      /// @return  Return value indicating success or error.
      virtual AxisProfileResult makePersistent(const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName) = 0;
    };
  }
}
