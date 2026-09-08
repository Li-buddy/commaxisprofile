#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and defines the interface offers methods to command the axisprofile system.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

/// @par List of available functions in this part of the axis profile system.
/// <TABLE>
/// <TR><TH>           Function                         </TH><TH> Interface version </TH><TH> Notes </TH></TR>
/// <TR><TD id="st_e"> getAxisProfileState              </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> requestAxisProfileState          </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getAxisProfileStateNames         </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> profileExists                    </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// <TR><TD id="st_e"> getMaxBootingTimeout             </TD><TD> 1.0.0             </TD><TD>       </TD></TR>
/// </TABLE>

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include <vector>
#include <string>

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

  #define APR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR             1
  #define APR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE   2
  #define APR_SYS_CM_CPP_INTERFACE_VERSION_MINOR             0
  #define APR_SYS_CM_CPP_INTERFACE_VERSION_PATCH             0

  #define IAPR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR            XSTR(APR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR)
  #define IAPR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE  XSTR(APR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE)
  #define IAPR_SYS_CM_CPP_INTERFACE_VERSION_MINOR            XSTR(APR_SYS_CM_CPP_INTERFACE_VERSION_MINOR)
  #define IAPR_SYS_CM_CPP_INTERFACE_VERSION_PATCH            XSTR(APR_SYS_CM_CPP_INTERFACE_VERSION_PATCH)

  /// @brief Interface version of the axis profile system bundle interface
  #define IAPR_SYSTEM_CM_CPP_INTERFACE_VERSION           IAPR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR "." IAPR_SYS_CM_CPP_INTERFACE_VERSION_MINOR "." IAPR_SYS_CM_CPP_INTERFACE_VERSION_PATCH
  /// @brief Consumer range of the axis profile system bundle interface
  #define IAPR_SYSTEM_CM_CPP_INTERFACE_CONSUMER_RANGE    "[" IAPR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR "." IAPR_SYS_CM_CPP_INTERFACE_VERSION_MINOR ".0," IAPR_SYS_CM_CPP_INTERFACE_VERSION_MAJOR_MAX_RANGE ".0.0)"
  /// @brief Name of the axis profile system bundle interface
  #define IAPR_SYSTEM_CM_CPP_INTERFACE_NAME              "com.boschrexroth.comm.axisprofile.system"

  // backward compatibility defines
  #define APR_SYSTEM_CM_CPP_INTERFACE_VERSION           IAPR_SYSTEM_CM_CPP_INTERFACE_VERSION
  #define APR_SYSTEM_CM_CPP_INTERFACE_CONSUMER_RANGE    IAPR_SYSTEM_CM_CPP_INTERFACE_CONSUMER_RANGE
  #define APR_SYSTEM_CM_CPP_INTERFACE_NAME              IAPR_SYSTEM_CM_CPP_INTERFACE_NAME

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the system interface for the axisprofile component.
  class IAxisProfileSystem
  {
  public: // methods

    virtual ~IAxisProfileSystem() noexcept = default;

    /// @brief This method returns the class name.
    ///
    /// @return                      Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(IAxisProfileSystem)); }

    // ------------------------------ Global statemachine ---------------------------------

    /// @brief Get the name of the current state of the axisprofile state machine
    /// 
    /// @return  name of the current state of the axisprofile state machine or nullptr in case of an error
    virtual const char *getAxisProfileState() const = 0;

    /// @brief Request a state change of the axisprofile state machine
    /// 
    /// @param[in]    stateName      name of the requested state of the axisprofile state machine
    /// @return                      was everything okay?
    virtual AxisProfileResult requestAxisProfileState(const char *stateName) = 0;

    /// @brief Get all state names of the axisprofile state machine
    /// 
    /// @param[out]   stateNames     names of all states of the axisprofile state machine
    /// @return                      was everything okay?
    virtual AxisProfileResult getAxisProfileStateNames(std::vector<std::string> &stateNames) const = 0;

    /// @brief Get axisprofile maximum booting time
    /// 
    /// @return                     booting timeout
    virtual double getMaxBootingTimeout() const = 0;
  };
}
