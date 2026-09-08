#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface profile manager which offers creating, connecting and deleting axis profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

///  @par List of available functions in this part of the axisprofile manager.
/// <TABLE>
/// <TR><TH>           Function                         </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> createAxisProfile                </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> deleteAxisProfile                </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> connectAxisProfile               </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> profileExists                    </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getConfiguredProfileNumber       </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_axisprofile_c.h"
#include <memory>
#include <functional>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  #define XSTR(s)                                               STR(s)
  #define STR(s)                                                #s

  #define APR_CM_CPP_INTERFACE_VERSION_MAJOR             1
  #define APR_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE   2
  #define APR_CM_CPP_INTERFACE_VERSION_MINOR             2
  #define APR_CM_CPP_INTERFACE_VERSION_PATCH             0

  #define IAPR_CM_CPP_INTERFACE_VERSION_MAJOR            XSTR(APR_CM_CPP_INTERFACE_VERSION_MAJOR)
  #define IAPR_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE  XSTR(APR_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE)
  #define IAPR_CM_CPP_INTERFACE_VERSION_MINOR            XSTR(APR_CM_CPP_INTERFACE_VERSION_MINOR)
  #define IAPR_CM_CPP_INTERFACE_VERSION_PATCH            XSTR(APR_CM_CPP_INTERFACE_VERSION_PATCH)

  /// @brief Interface version of the axis profile manager bundle interface
  #define IAPR_CM_CPP_INTERFACE_VERSION           IAPR_CM_CPP_INTERFACE_VERSION_MAJOR "." IAPR_CM_CPP_INTERFACE_VERSION_MINOR "." IAPR_CM_CPP_INTERFACE_VERSION_PATCH
  /// @brief Consumer range of the axis profile manager bundle interface
  #define IAPR_CM_CPP_INTERFACE_CONSUMER_RANGE    "[" IAPR_CM_CPP_INTERFACE_VERSION_MAJOR "." IAPR_CM_CPP_INTERFACE_VERSION_MINOR ".0," IAPR_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE ".0.0)"
  /// @brief Name of the axis profile manager bundle interface
  #define IAPR_CM_CPP_INTERFACE_NAME              "com.boschrexroth.comm.axisprofile.manager"

  // backward compatibility defines
  #define APR_CM_CPP_INTERFACE_VERSION           IAPR_CM_CPP_INTERFACE_VERSION
  #define APR_CM_CPP_INTERFACE_CONSUMER_RANGE    IAPR_CM_CPP_INTERFACE_CONSUMER_RANGE
  #define APR_CM_CPP_INTERFACE_NAME              IAPR_CM_CPP_INTERFACE_NAME

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  ///  @brief Return type of the check callback
  enum class CheckResponse
  {
    OK_USE_PROFILE,            ///< no error - use the axisprofile as usual
    OK_DONT_USE_PROFILE,       ///< no error - ignore this axisprofile (don't do anything with it)
    ERR_UNKNOWN                ///< error - the given axisprofile name is not known by the check function - use the axisprofile as usual (might be related to something other)
  };

  /// @brief type of the check callback
  using CheckCallBack = std::function<CheckResponse(const std::string& profileName)>;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the interface for the axisprofile manager.
  class IAxisProfileManager
  {
  public: // methods

    virtual ~IAxisProfileManager() noexcept = default;

    /// @brief This method returns the class name.
    ///
    /// @return                      Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(IAxisProfileManager)); }

    /// @brief This method creates a new axis profile (requires configuration state!). If the desired profile already exists an error will
    ///        be returned.
    /// 
    /// @param[in]    type          Type of the new axis profile
    /// @param[in]    profileName   Name of the profile
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult createAxisProfile(const AxisProfileType type, const std::string& profileName) = 0;

    /// @brief This method deletes an existing axis profile (requires configuration state!). Only profiles that are not connected can be deleted.
    /// 
    /// @param[in]    profileName   Name of the profile
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult deleteAxisProfile(const std::string& profileName) = 0;

    /// @brief This method connects the requested axisprofile (requires configuration state!). Afterwards the profile can be used to
    ///        communicate (send commands, exchange cyclic data, ...) with the configured device. Internally the profile will be marked
    ///        as 'currently in use' and therefore can not be deleted anymore.
    ///        Important: The memory of the profile instance is owned by the axisprofile! The user is responsible to destroy his shared pointer
    ///        variables when they are not used anymore in order to be able to delete the profile afterwards by calling 'deleteAxisProfile()'.
    /// 
    /// @param[in]    profileName   Name of the profile
    /// @param[out]   profile       Profile instance shared pointer
    /// @return       Return value indicating success or error.
    virtual AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile) = 0;

    /// @brief This method checks if the given profile name exists in the system.
    /// 
    /// @param[in]    profileName   Name of the profile
    /// @return       Return value indicating if the profile exists (true) or not (false).
    virtual bool profileExists(const std::string& profileName) const = 0;

    /// @brief This method returns actual configured number of axisprofile.
    /// 
    /// @return       Return actual configured number of axisprofile
    virtual std::uint32_t getConfiguredProfileNumber() const = 0;

    /// @brief Register a check callback function to give the profile extern check result.
    /// @param[in]  checkFct    Check callback function
    /// @param[in]  bundleName  Name of the bundle, that adds the callback (required for unregisterCheckCallback())
    /// @return  Return value indicating success or error.
    virtual AxisProfileResult registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName) = 0;

    /// @brief Unregister all registered check callback function (that were registered by the given bundle) in axisprofile
    /// @param[in]  bundleName  Name of the bundle, that wants to remove all its callbacks
    /// @return  Return value indicating success or error.
    virtual AxisProfileResult unregisterAllCheckCallbacks(const std::string& bundleName) = 0;
  };
}
