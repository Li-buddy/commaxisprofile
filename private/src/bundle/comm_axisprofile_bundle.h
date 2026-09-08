#pragma once
/// @file
///
/// @brief This file is part of the axisprofile library and declares the main
/// class of this celix bundle.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "celix/framework/celix/dm/DmActivator.h"
#include "celix/utils/celix_errno.h"
#include "component/axisprofile_cmp.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------
  #define _CLASS_NAME_(x)   (           # x )
  #define CLASS_NAME(x)     (_CLASS_NAME_(x))

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the celix bundle main class of the axisprofile. It implements the
  /// celix interface methods for life cycle management of the bundle. It is a child
  /// class of the component to be able to make the component interfaces available
  /// to other bundles.
  class AxisProfileBundle : public AxisProfileCmp
  {
  public: // methods

    /// @brief Constructor
    AxisProfileBundle() = default;
    /// @brief Destructor
    virtual ~AxisProfileBundle() = default;

    /// This method returns the class name.
    /// @return                      Return class name.
    static const char* getClassName(void)
    {
      return (CLASS_NAME(comm::axisprofile::AxisProfileBundle));
    }

    // --------------------------------------- Celix Methods -----------------------------------------

    celix_status_t init() final;
    celix_status_t start() final;
    celix_status_t stop() final;
    celix_status_t deinit() final;

    // ------------------------------------- Bundle Information --------------------------------------

    /// This method is used to set bundle context from activator, the context is
    /// e.g. needed for determination of bundle path.
    void setDependencyManager(bundle_context_pt context)
    {
      m_context = context;
    };

  private: // data

    bundle_context_pt m_context;   ///< bundle context pointer
  };
}
