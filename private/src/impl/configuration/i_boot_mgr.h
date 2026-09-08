#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_boot_mgr.h
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
//! This file is part of the axisprofile library and defines the interface
//! of the boot manager.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "component/i_axisprofile_component.h"


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

    //! @class IBootMgr
    //! @brief This is the interface for the boot manager.
    class IBootMgr
    {
    public: // methods
      IBootMgr() = default;
      virtual ~IBootMgr() noexcept = default;

      //! This must be called after the axisprofile state machine has been created!
      //! @param[in]   component   Pointer to the component
      //! @return Return value indicating success or error.
      virtual AxisProfileResult init(IAxisProfileComponent* component) = 0;

      //! @brief Startup the axisprofile system
      //! @return Return value indicating success or error.
      virtual AxisProfileResult bootSystem() = 0;

      //! @brief Stop the axisprofile system
      //! @return Return value indicating success or error.
      virtual AxisProfileResult stop() = 0;

      //! @brief This method is used to abort the booting procedure when it stuck.
      virtual void abortBooting() = 0;

      //! @brief Get the current booting state
      //! @param[out]  actStep             actual booting step (starting with 0, booting is finished when actStep==maxSteps)
      //! @param[out]  maxSteps            number of booting steps
      //! @param[out]  txt                 text of the actual booting step
      virtual void getBootingState(std::uint32_t& actStep, std::uint32_t& maxSteps, std::string& txt) const = 0;

      /// @brief Get component booting maximum timeout in seconds
      /// @return component booting maximum timeout
      virtual double getMaxBootingTimeout() const = 0;

      /// @brief Indicates component is initial booting
      /// @return TRUE indicates component it initial booting (load configuration)
      virtual bool isInitBooting() = 0;
    };
  }
}
