/// @file
///
/// @brief This file is part of the axisprofile library and implements the main
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
#include "comm/axisprofile/bundle_version.h"
#include "comm_axisprofile_bundle.h"
#include <iostream>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of constants
  // ---------------------------------------------------------------------------
    
  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------
    
  // ---------------------------------------------------------------------------
  // Forward function references
  // ---------------------------------------------------------------------------
    
  // ---------------------------------------------------------------------------
  // Class implementation
  // ---------------------------------------------------------------------------
  celix_status_t AxisProfileBundle::init()
  {
    std::cout << "call of '" << m_bundleName.c_str() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
    std::cout << "Initializing Comm.AxisProfile " << comm::axisprofile::s_version.VERSION_STRING << std::endl;
    return AxisProfileCmp::init();
  }
    
  celix_status_t AxisProfileBundle::start()
  {
    std::cout << "call of '" << m_bundleName.c_str() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
    return AxisProfileCmp::start();
  }
    
  celix_status_t AxisProfileBundle::stop()
  {
    std::cout << "call of '" << m_bundleName.c_str() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
    return AxisProfileCmp::stop();
  }
    
  celix_status_t AxisProfileBundle::deinit()
  {
    std::cout << "call of '" << m_bundleName.c_str() << ": " << this->getClassName() << "::" << __func__ << "'..." << std::endl;
    return AxisProfileCmp::deinit();
  }
}
