#pragma once

// ---------------------------------------------------------------------------
//
//! @file       axisprofile_manager.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and implements the
//! manager class for all axisprofiles.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_manager.h"
#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_internal.h"
#include "i_axisprofile_manager_internal.h"
#include "deviceinfo_manager.h"
#include "common/mutex_helper.h"

#include <vector>
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestAxisProfileManager;
class TestAxisProfileManagerExternal;

namespace comm
{
  namespace datalayer
  {
    class IDataLayerFactory;
    class IClientInternal;
  }

  namespace axisprofile
  {
    class IBootParamBackupRestoreMgr;
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
    // Declaration of constants
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

    //! @class AxisProfileManager
    //! This is the central administration class of this component.
    class AxisProfileManager : public IAxisProfileManagerInternal
    {
    private: // types

      /// @brief struct to hold the information for a specific memory in the datalayer rt
      struct MemoryInfo : public RtMemInfoBase
      {
        std::vector<IAxisProfileInternal*> m_profiles;  ///< vector of profiles, that need this buffer for cyclic access
        MemoryInfo(const std::string& uri, IAxisProfileInternal* profile)
        {
          m_bufferURI = uri;
          m_profiles.push_back(profile);
        }
      };

    public: // methods

      //! @brief constructor
      AxisProfileManager() = default;
      //! @brief destructor
      virtual ~AxisProfileManager() noexcept override;

      // ------------------------------------ Internal Interface -------------------------------------

      void setDataLayer(comm::datalayer::IDataLayerFactory* datalayer, IBackgroundThread* bkThread) override;
      void getFieldbusInstances(ContainerFieldbusInst& mapInstances) final;      
      AxisProfileResult initManager(const bool& abortInitialization, double cycleTime = STD_CYCLE_TIME) final;      
      AxisProfileResult processDataAt() final;
      AxisProfileResult processDataMdt() final;
      IAxisProfile* getProfileByName(const char* profileName) final;
      void getProfileNames(std::vector<std::string> &profileNames) final;
      AxisProfileResult setProfileInputBuffer(const char* profileName, const char* inputBufferURI) final;
      const char* getProfileInputBuffer(const char* profileName) final;
      AxisProfileResult setProfileOutputBuffer(const char* profileName, const char* outputBufferURI) final;
      const char* getProfileOutputBuffer(const char* profileName) final;
      const StateMachineState* &getStateMachineStatusIF() final {return m_stateMachineState;};
      AxisProfileResult resetAllFPUFlag() final;
      AxisProfileResult execExternalChecking() final;
      void getActivatedProfiles(std::vector<IAxisProfile*> &profiles) final;
      AxisProfileResult collectAllDeviceInfo(const bool& abortInitialization) final;
      AxisProfileResult checkConsistency() final;
      void setBootParamBackupRestoreMgr(const IBootParamBackupRestoreMgr* bootParamBackupRestoreMgr) final;

      // ------------------------------------- External Interface -------------------------------------

      AxisProfileResult createAxisProfile(const AxisProfileType type, const std::string& profileName) final;
      AxisProfileResult deleteAxisProfile(const std::string& profileName) final;
      AxisProfileResult connectAxisProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& profile) final;
      bool profileExists(const std::string& profileName) const final;
      AxisProfileResult processAuxJobs() final;
      std::uint32_t getConfiguredProfileNumber() const final;
      AxisProfileResult registerCheckCallback(const CheckCallBack& checkFct, const std::string& bundleName) final;
      AxisProfileResult unregisterAllCheckCallbacks(const std::string& bundleName) final;
      void runAllJobs() final;

    private: // methods

      //! @brief This method initializes the realtime memory´s that are registered for the profiles
      //! @return  Return value indicating success or error.
      AxisProfileResult initMemory();

      //! @brief This method reads the memory map´s of the realtime memory´s and initializes the corresponding profile mappings
      //! @param[in]   abortInitialization   Flag indicating if the initialization should be aborted (e.g. used to exit the infinite while loop)
      //! @return  Return value indicating success or error.
      AxisProfileResult initMapping(const bool& abortInitialization);

      /// @brief This method initializes the device information manager.
      /// @return  Return value indicating success (>=0) or error (<0).
      AxisProfileResult initDeviceInfoMgr();

      //! @brief This method closes all realtime memory´s
      //! @return  Return value indicating success or error.
      AxisProfileResult closeAllMemories();

      //! @brief This method gets the requested profile information structure.
      //! @param[in]   profileName   Name of the profile.
      //! @return Pointer to the profile information or nullptr if it could not be found.
      ProfileInfo* getProfileInfoByName(const char* profileName);

      /// @brief This method reset all realtime data
      void resetRtInputData();


      /// @brief This method will validate all mapping item in each profile.
      /// @return Return value indicating success (>=0) or error (<0).
      AxisProfileResult validatePdoCfg();

      /// @brief This method will update device address according to corresponding device name.
      AxisProfileResult updateProfileAddress();

      /// @brief This function will validate protocol information between device and profile
      /// @return Return value indicating success (>=0) or error (<0).
      AxisProfileResult validateProtocolInfo();

      /// @brief This method browses the fieldbus master instances node via datalayer to get the correct address string.
      ///        Note: If more than one instance is available, the one that matches the configured output buffer URI will be used.
      /// @return  Return value indicating success or error.
      AxisProfileResult validateFieldbusMasterInfo();

      /// @brief Get all activated profile infomation.
      /// @param[out] profiles  Holds all activated profiles information.
      void getActivatedProfilesInfo(std::vector<ProfileInfo>& profilesInfo);

      /// @brief Save boot parameters if needed.
      /// @param[in] profileInternal Pointer to the profile internal interface.
      /// @param[in] profileName Name of the profile.
      void saveBootParamsIfNeed(IAxisProfileInternal* profileInternal, const std::string& profileName);

      /// @brief Check whether the profile's boot parameters should be auto-saved after booting.
      /// @param[in] profile Pointer to the profile.
      /// @return true when auto-save should be triggered.
      bool shouldTriggerAutoBootParamsSave(IAxisProfileInternal* profile);
      
    private: // data

      /// @brief The central data container which contains all existing axisprofiles.
      /// This map is used by this class to manage all axis profiles. The map key is the name of the profile.
      std::map<std::string, ProfileInfo> m_mapAxisProfiles;              ///< map of profile information
      DeviceInfoManager                  m_deviceInfoMgr;                ///< device information manager
      const StateMachineState*           m_stateMachineState{};          ///< statemachine

      std::vector<MemoryInfo>            m_vecCyclicMemoryAccessAT{};    ///< vector of the memory information used for at
      std::vector<MemoryInfo>            m_vecCyclicMemoryAccessMDT{};   ///< vector of the memory information used for mdt

      // Datalayer
      comm::datalayer::IDataLayerFactory* m_datalayer = nullptr;         ///< ServiceDependency - Pointer to datalayer
      comm::datalayer::IClientInternal* m_datalayerClient = nullptr;     ///< Internal wrapped datalayer client
      const IBootParamBackupRestoreMgr* m_bootParamBackupRestoreMgr{};   ///< Boot parameter backup helper for auto-save

      // Unittest
      friend class ::TestAxisProfileManager;           ///< Unittest of the internal interface methods
      friend class ::TestAxisProfileManagerExternal;   ///< Unittest of the external interface methods

      // Workarounds...
      bool m_traceErrorAlreadySentAT = false;    ///< Workaround: Flag to prevent sending same trace error message cyclical in processDataAt()
      bool m_traceErrorAlreadySentMDT = false;   ///< Workaround: Flag to prevent sending same trace error message cyclical in processDataMdt()
      bool m_initialFieldbusConnection = true;   ///< This variable indicates if it is the first try to read from fieldbus driver ( workaround for bootup)

      std::multimap<std::string, CheckCallBack> m_extChkFuncs{};   ///< external check functions of axis profiles, key is bundle name
      MutexHelper m_mutex{};                                       ///< used for register from multiply external bundle.
    };
  }
}
