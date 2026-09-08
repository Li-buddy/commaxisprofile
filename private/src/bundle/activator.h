#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and declares the celix
/// bundle activator class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "bundle_helper.hpp"
#include "celix/framework/celix/dm/DmActivator.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------
#define _CLASS_NAME_(x)   (           # x )
#define CLASS_NAME(x)     (_CLASS_NAME_(x))

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of classes and structures
// ---------------------------------------------------------------------------

/// @brief The activator implements a "normal" celix bundle activator and depends on four functions which needs to be implemented by the user of the Depedency Manager:
class Activator : public DmActivator, public BundleHelper {
private:

public: // methods

  /// @brief constructor
  explicit Activator(DependencyManager& mng) : DmActivator{ mng }, BundleHelper{ mng }{}

  /// @brief destructor
  virtual ~Activator() noexcept override = default;

  /// This method returns the class name.
  ///
  /// @return                      Return class name.
  static const char* getClassName(void) { return (CLASS_NAME(Activator)); }

  /// This method should be used to interact with the Dependency Manager.
  /// Here a user can  instantiate components, service dependencies and provided services.
  /// This method is called during start of bundle, after 'create'.
  virtual void init() override;

  /// This method should be used to deinitialize and deallocate objects created in the dm_create function.
  /// This method is called during stop of bundle.
  virtual void deinit() override;
};
