#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines bundle helper
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "celix/utils/celix_errno.h"
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

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of classes and structures
// ---------------------------------------------------------------------------

#ifdef __cplusplus

/// @brief BundleHelper
class BundleHelper
{
private:
  celix_bundle_context_t* m_context = nullptr;
  std::string m_bundlePathHelper;


public:
  BundleHelper() = default;
  virtual ~BundleHelper() = default;

  explicit BundleHelper(DependencyManager& mng)
  {
    m_context = mng.bundleContext();
  };

  const char* getBundlePath(void)
  {
    celix_bundle_t* bundle = nullptr;

    if (nullptr == m_context)
    {
      std::cout << "Error, invalid bundle context." << std::endl;
      return nullptr;
    }
    bundle = celix_bundleContext_getBundle(m_context);
    if (nullptr == bundle)
    {
      std::cout << "Error while getting bundle context." << std::endl;
      return nullptr;
    }
    auto path = celix_bundle_getEntry(bundle, nullptr);
    m_bundlePathHelper = path;
    delete path;
    return m_bundlePathHelper.c_str();
  }

  const char* getBundleName(void)
  {
    celix_bundle_t* bundle = nullptr;

    if (nullptr == m_context)
    {
      std::cout << "Error, invalid bundle context." << std::endl;
      return nullptr;
    }
    bundle = celix_bundleContext_getBundle(m_context);
    if (nullptr == bundle)
    {
      std::cout << "Error while getting bundle context." << std::endl;
      return nullptr;
    }
    return celix_bundle_getManifestValue(bundle, "Bundle-Name");
  }
};

#endif
