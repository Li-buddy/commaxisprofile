#pragma once

/// @file
///
/// This source file is part of the axisprofile library and implement mock axisprofile manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "manager/i_axisprofile_manager_internal.h"
#include "fieldbuses/i_fieldbus_adapter.h"
#include "mockup/mock_axisprofile.h"
#include "manager/deviceinfo_manager.h"
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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

  //! @class MockAxisProfileManager
  //! @brief This class implements a mockup for of the axis profile manager for internal usage.
  class MockAxisProfileManager : public comm::axisprofile::IAxisProfileManagerInternal
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileManager() = default;
    //! @brief Destructor
    virtual ~MockAxisProfileManager()
    {
      for (auto& it : m_mapAxisProfiles)
      {
        it.second.reset();
        delete it.second.get();
      }

      m_mapAxisProfiles.clear();
    }

    // ----------------------------- Internal Interface ---------------------------------

    comm::axisprofile::AxisProfileResult processDataAt() override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult processDataMdt() override { return m_returnValue; }
    void setDataLayer(comm::datalayer::IDataLayerFactory* /*datalayer*/, IBackgroundThread* /*bkThread*/) override {}
    comm::axisprofile::AxisProfileResult initManager(const bool& /*abortInitialization*/, double /*cycleTime*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult validateMappingCfg() { return m_returnValue; };
    comm::axisprofile::AxisProfileResult updateProfileAddress() { return m_returnValue; };
    comm::axisprofile::AxisProfileResult validateProtocolInfo() {return m_returnValue;}
    void getFieldbusInstances(ContainerFieldbusInst& setInstances)override
    {
      setInstances = m_deviceInfoMgr.getFieldbusMasters();
    };
    comm::axisprofile::AxisProfileResult validateFieldbusMasterInfo() { return m_returnValue; }
    comm::axisprofile::AxisProfileResult setProfileInputBuffer(const char* /*profileName*/, const char* /*inputBufferURI*/) override { return m_returnValue; }
    const char* getProfileInputBuffer(const char* /*profileName*/) override { return nullptr; }
    comm::axisprofile::AxisProfileResult setProfileOutputBuffer(const char* /*profileName*/, const char* /*outputBufferURI*/) override { return m_returnValue; }
    const char* getProfileOutputBuffer(const char* /*profileName*/) override { return nullptr; }
    const comm::axisprofile::StateMachineState* &getStateMachineStatusIF() override {return m_stateMachineState;};
    comm::axisprofile::AxisProfileResult processAuxJobs() override { return m_returnValue;}
    std::uint32_t getConfiguredProfileNumber() const override { return static_cast<std::uint32_t>(m_mapAxisProfiles.size()); }
    void runAllJobs() override {};

    comm::axisprofile::IAxisProfile* getProfileByName(const char* profileName) override
    {
      auto elem = m_mapAxisProfiles.find(profileName);
      if (elem == m_mapAxisProfiles.end()) { return nullptr; }
      else { return elem->second.get(); }
    }

    void getProfileNames(std::vector<std::string> &profileNames) override
    {
      profileNames.clear();

      for ( auto const &profileInfo : m_mapAxisProfiles )
      {
        profileNames.push_back(profileInfo.first);
      }
      return;
    }

    comm::axisprofile::AxisProfileResult initDeviceInfoMgr() { return m_returnValue; }

    comm::axisprofile::AxisProfileResult resetAllFPUFlag() override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult execExternalChecking() override { return m_returnValue; };
    void getActivatedProfiles(std::vector<comm::axisprofile::IAxisProfile*>& profiles) override
    {
      profiles.clear();
      for ( auto const &profileInfo : m_mapAxisProfiles )
      {
        auto profile = static_cast<comm::axisprofile::IAxisProfileInternal*>(profileInfo.second.get());
        if (profile->checkProfileActivated())
        {
          profiles.push_back(profileInfo.second.get());
        }
      }
    };

    comm::axisprofile::AxisProfileResult collectAllDeviceInfo(const bool& /*abortInitialization*/)override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult checkConsistency() override { return m_returnValue; }

    // ----------------------------- External Interface ---------------------------------

    comm::axisprofile::AxisProfileResult createAxisProfile(const comm::axisprofile::AxisProfileType type, const std::string& profileName) override
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        auto iter = m_mapAxisProfiles.find(profileName);
        if (iter != m_mapAxisProfiles.end()) { return comm::axisprofile::AxisProfileResult::ERR_PROFILE_ALREADY_EXISTS; }
        m_mapAxisProfiles.insert(std::make_pair(profileName, new test::MockAxisProfile(profileName, type)));
      }

      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult deleteAxisProfile(const std::string& profileName) override
    {
      if (comm::axisprofile::aprStatusSucceeded(m_returnValue))
      {
        auto iter = m_mapAxisProfiles.find(profileName);
        if (iter == m_mapAxisProfiles.end()) { return comm::axisprofile::AxisProfileResult::ERR_INVALID_ARGUMENT; }
        else { iter->second.reset(); delete iter->second.get(); m_mapAxisProfiles.erase(iter); return comm::axisprofile::AxisProfileResult::STS_OK; }
      }

      return m_returnValue;
    }

    comm::axisprofile::AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& /*profile*/) override
    {
      auto iter = m_mapAxisProfiles.find(profileName);
      if (iter == m_mapAxisProfiles.end()) { return comm::axisprofile::AxisProfileResult::ERR_PROFILE_NOT_FOUND; }
      return m_returnValue;
    }

    bool profileExists(const std::string& profileName) const override
    {
      return (m_mapAxisProfiles.find(profileName) == m_mapAxisProfiles.end()) ? false : true;
    }

    AxisProfileResult registerCheckCallback(const CheckCallBack& /*checkFct*/, const std::string& /*bundleName*/) override
    {
      return m_returnValue;
    }

    AxisProfileResult unregisterAllCheckCallbacks(const std::string& /*bundleName*/) override
    {
      return m_returnValue;
    }

  public: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   ///< Result of member functions
    std::map<std::string, std::shared_ptr<comm::axisprofile::IAxisProfile>> m_mapAxisProfiles;           ///< Map containing all axisprofiles (key = device Address)
    const comm::axisprofile::StateMachineState* m_stateMachineState{};                                   ///< pointer to statemachine state
    comm::axisprofile::DeviceInfoManager        m_deviceInfoMgr;                                         ///< Device information manager object
  };
}
