#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines mockups for the axis profile manager interface.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_manager.h"
#include "mock_axisprofile.h"
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

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

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief struct to hold the necessary information for a profile in the MockAxisProfileManager
  struct MockProfileInfo
  {
    AxisProfileMockHandle m_axisProfileHandle{};          ///< Helper class used as handle
    std::shared_ptr<IAxisProfileC> m_mockProfileShared;   ///< Shared pointer for external usage
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This class implements a mockup for the axis profile manager interface.
  class MockAxisProfileManager : public IAxisProfileManager
  {
  public: // methods

    /// @brief Constructor
    MockAxisProfileManager() = default;
    /// @brief Destructor
    virtual ~MockAxisProfileManager()override
    {
      for (auto& it : m_mapAxisProfiles)
      {
        it.second.m_mockProfileShared.reset();
      }

      m_mapAxisProfiles.clear();
    }

    AxisProfileResult createAxisProfile(const AxisProfileType /*type*/, const std::string& profileName) override
    {
      auto iter = m_mapAxisProfiles.find(profileName);
      if (iter != m_mapAxisProfiles.end()) { return AxisProfileResult::ERR_PROFILE_ALREADY_EXISTS; }
      m_mapAxisProfiles[profileName].m_mockProfileShared.reset(new IAxisProfileC());
      auto it = m_mapAxisProfiles.find(profileName);
      MockAxisProfileManager::configureProfileCInterface(it->second);
      return AxisProfileResult::STS_OK;
    }

    AxisProfileResult deleteAxisProfile(const std::string& profileName) override
    {
      auto iter = m_mapAxisProfiles.find(profileName);
      if (iter == m_mapAxisProfiles.end()) { return AxisProfileResult::ERR_PROFILE_NOT_FOUND; }
      else { iter->second.m_mockProfileShared.reset(); m_mapAxisProfiles.erase(iter); return AxisProfileResult::STS_OK; }
    }

    AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile) override
    {
      auto iter = m_mapAxisProfiles.find(profileName);
      if (iter == m_mapAxisProfiles.end()) { return AxisProfileResult::ERR_PROFILE_NOT_FOUND; }
      else if (iter->second.m_mockProfileShared.use_count() != 1) { return AxisProfileResult::ERR_PROFILE_IN_USE; }
      else { profile = iter->second.m_mockProfileShared; }
      return AxisProfileResult::STS_OK;
    }

    bool profileExists(const std::string& profileName) const override
    {
      return (m_mapAxisProfiles.find(profileName) == m_mapAxisProfiles.end()) ? false : true;
    }

    std::uint32_t getConfiguredProfileNumber() const override { return static_cast<std::uint32_t>(m_mapAxisProfiles.size()); };

    AxisProfileResult registerCheckCallback(const CheckCallBack& /*checkFct*/, const std::string& /*bundleName*/) override
    {
      return AxisProfileResult::STS_OK;
    }

    AxisProfileResult unregisterAllCheckCallbacks(const std::string& /*bundleName*/) override
    {
      return AxisProfileResult::STS_OK;
    }

    /// @brief This method configures the C-Interface of a new axisprofile mockup.
    /// @param[in]   profileInfo   Profile information struct element that will be configured
    static void configureProfileCInterface(MockProfileInfo& profileInfo)
    {
      profileInfo.m_mockProfileShared->handle = &profileInfo.m_axisProfileHandle;
      profileInfo.m_mockProfileShared->getCommandInterface = AXISPROFILE_getCommandInterfaceCMock;
      profileInfo.m_mockProfileShared->getStatusControlInterface = AXISPROFILE_getStatusControlInterfaceCMock;
      profileInfo.m_mockProfileShared->getDiagMsgInterface = AXISPROFILE_getDiagMsgInterfaceCMock;
      profileInfo.m_mockProfileShared->getCyclicDataInterface = AXISPROFILE_getCyclicDataInterfaceCMock;
      profileInfo.m_mockProfileShared->getBootingConfigInterface = AXISPROFILE_getBootingConfigInterfaceCMock;
      profileInfo.m_mockProfileShared->getDeviceParamInterface = AXISPROFILE_getDeviceParamInterfaceCMock;
      profileInfo.m_mockProfileShared->getProbeInterface = AXISPROFILE_getProbeInterfaceCMock;
      profileInfo.m_mockProfileShared->getCmdInterface = AXISPROFILE_getCmdInterfaceCMock;
      profileInfo.m_mockProfileShared->getPropertyInterface = AXISPROFILE_getPropertyInterfaceCMock;
    }

  public: // data

    std::map<std::string, MockProfileInfo> m_mapAxisProfiles; ///< Map containing all axisprofiles (key = device Address)
  };
}
