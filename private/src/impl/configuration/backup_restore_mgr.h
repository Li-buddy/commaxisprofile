#pragma once

// ---------------------------------------------------------------------------
//
/// @file       backup_restore_mgr.h
///
/// @author     DC-AE/ESW2
///
/// @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
///
/// @version    1.0.0
///
/// @date       2019
//
// ---------------------------------------------------------------------------
//
/// This header file is part of the axisprofile library and implements the
/// backup restore manager. This class manages the remanent saving/loading
/// of axis profile configuration data to/from a JSON file.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_backup_restore_mgr.h"
#include <memory>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestBackupRestoreManager;
struct PersistenceHelperItf;

namespace comm
{
  namespace axisprofile
  {
    class IAxisProfileManagerInternal;
  }

  namespace datalayer
  {
    class IClient;
    struct DlResult;
    class Variant;
    struct PersistenceParam;
  } // namespace datalayer
} // namespace comm

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
    const std::string MSG_ABORT_LOAD_CFG = "Abort load configuration!";  ///< Detailed diag info for error occurs during load config process
    const std::string MSG_ABORT_SAVE_CFG = "Abort save configuration!";  ///< Detailed diag info for error occurs during save config process
    const std::string MSG_ABORT_LOAD_SYSTEM_CFG = "Abort load system configuration!";  ///< Detailed diag info for error occurs during load system config process
    const std::string MSG_ABORT_SAVE_SYSTEM_CFG = "Abort save system configuration!";  ///< Detailed diag info for error occurs during save system config process
    const std::string MSG_ABORT_MAKE_PERSISTENCE = "Abort make persistence!";          ///< Detailed diag info for error occurs during make persistence process

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

    /// @class BackupRestoreMgr
    /// This is the backup/restore manager of the axisprofile.
    class BackupRestoreMgr : public IBackupRestoreMgr
    {
    public: // Methods
      /// @brief constructor
      /// @param[in]   profileMgr          Pointer to the axis profile manager
      /// @param[in]   dlClient            Pointer to the datalayer client
      /// @param[in]   persistenceHelper   Pointer to the persistence helper
      BackupRestoreMgr(IAxisProfileManagerInternal *const profileMgr, comm::datalayer::IClient *const dlClient, const PersistenceHelperItf *persistenceHelper);
      BackupRestoreMgr(const BackupRestoreMgr&) = delete;
      BackupRestoreMgr& operator=(const BackupRestoreMgr&) = delete;

      /// @brief destructor
      virtual ~BackupRestoreMgr() noexcept override;

      AxisProfileResult load(const std::string &profileName, const std::string &filePath) final;
      AxisProfileResult save(const std::string& profileName, const std::string& filePath) final;

      AxisProfileResult loadSystemCfg(const std::string &folderPath) final;
      AxisProfileResult saveSystemCfg(const std::string &folderPath) final;

      AxisProfileResult loadSystemCfg(const comm::datalayer::Variant &persistenceParam) final;
      AxisProfileResult saveSystemCfg(const comm::datalayer::Variant &persistenceParam) final;
      AxisProfileResult makePersistent(const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName) final;

    private: // Methods

      /// @brief Validate form parameter will used in context.
      /// @param[in]   profileName   Name of the requested axis profile.
      /// @return Return value indicating success or error.
      AxisProfileResult validateParam(const std::string& profileName) const;

      /// @brief Load a single axis profile configuration file.
      /// @param[in]   profileName   Name of the axis profile that should be loaded.
      /// @param[in]   filePath      File path of the axis profile config JSON file.
      /// @return Return value indicating success or error.
      AxisProfileResult loadCfg(const std::string &profileName, const std::string &filePath);

      /// @brief Store a single axis profile configuration data into a JSON file via explicit datalayer uri.
      /// @param[in]   profileName   Name of the requested axis profile that should be saved.
      /// @param[in]   filePath      Target path where the axis profile config JSON file should be stored.
      /// @return Return value indicating success or error.
      AxisProfileResult saveCfg(const std::string &profileName, const std::string &filePath);

      /// @brief find the position of the last path separator in the given file path.
      /// @param filePath   the file path
      /// @return Return value the position of the last path separator in the given file path, or std::string::npos if no path separator is found.
      static size_t findPathSeparator(const std::string& filePath);

      /// @brief extract the name of a file (without file extension) from the given path.
      /// @param[in]   filePath   the file path
      /// @return Return value the name of a file (without file extension)
      static std::string getFileNameFromPath(const std::string &filePath);

      /// @brief Make the given path to a folder path. If the given path is empty, the default folder path will be used.
      /// @param[in]   path   The given path
      /// @return The determined folder path
      static std::string makeFolderPath(const std::string& path);

      /// @brief Load the given configuration.
      /// @param[in]   folderPath   Path to the folder with axisprofile configuration.
      /// @param[in]   loadId       Identification for the load process.
      /// @return Return value indicating success or error.
      AxisProfileResult executeCfgLoad(const std::string &folderPath, const std::string &loadId);

      /// @brief Check if the loading of the given configuration is possible.
      /// @param[in]   folderPath   Path to the folder with axisprofile configuration.
      /// @param[in]   loadId       Identification for the load process.
      /// @return Return value indicating success or error.
      AxisProfileResult queryCfgLoad(const std::string &folderPath, const std::string &loadId);

      /// @brief Save the actual configuration of the axisprofiel system to the given folder.
      /// @param[in]   folderPath   Path to the folder for the axisprofile configuration.
      /// @param[in]   loadId       Identification for the load process.
      /// @return Return value indicating success or error.
      AxisProfileResult executeCfgSave(const std::string &folderPath, const std::string &loadId);

      /// @brief get all configuration files in the given folder.
      /// @param[in]   folderPath       Path to the folder with motion configuration.
      /// @param[in]   loadId           Identification for the load process.
      /// @param[out]  availableFiles   Vector with the list of the configuration files.
      /// @return Return value indicating success or error.
      AxisProfileResult getAvailableFiles(const std::string &folderPath, const std::string &loadId, std::vector<std::string> &availableFiles);

      /// @brief compare profile object sets between loaded files and current runtime.
      /// @param[in]  newProfiles   Vector with profile names from configuration files.
      /// @return Returns true if the objects of the configurations differ.
      bool newOrDeletedObjectsAvailable(const std::vector<std::string>& newProfiles);

      /// @brief compare the configuration files with runtime data and check if changes are valid in state "Running"
      /// @param[in]  loadId  Identification for the load process.
      /// @param[in]  filePaths   Vector with list of the configuration files.
      /// @return Returns true data changed that is invalid in state "Running".
      bool checkChangedData(const std::string &loadId, const std::vector<std::string>& filePaths);

      /// @brief compare the given configuration with the actual configuration and check if the changes are valid in state "Running".
      /// @param[in]  dlUri     Datalayer uri for a profile configuration node.
      /// @param[in]  cfgData   Vector with the new configuration of the profile.
      /// @return Returns true if the changes are valid in to load in state "Running".
      bool checkConfigData(const std::string& dlUri, comm::datalayer::Variant *cfgData);

      /// @brief Load profile configuration data from a file using configuration metadata.
      /// @param[in]   filePath      File path of the configuration JSON file.
      /// @param[out]  data          Loaded file data.
      /// @return Return value indicating success or error.
      AxisProfileResult readCfgDataFromFile(const std::string& filePath, comm::datalayer::Variant& data);

      /// @brief extract the given persistence param flatbuffer to the different values.
      /// @param[in]   paramFb       Fatbuffer with the persistence parameters
      /// @param[out]  configPath    Relative configuration path from the solution
      /// @param[out]  id            Identification for the load process.
      /// @param[out]  phase         Loading phase for the load process.
      /// @return Return value indicating success or error.
      static AxisProfileResult extractPersistenceData(const comm::datalayer::PersistenceParam *paramFb, std::string &configPath, std::string &id, std::string &phase);

      /// @brief Save the given single axisprofile configuration.
      /// @param[in]   axisprofileName  Name of the single axisprofile object that should be saved.
      /// @param[in]   folderPath       Path to the folder for the axisprofile configuration.
      /// @return Return value indicating success or error.
      AxisProfileResult exeMakePersistentCfgSave(const std::string& axisprofileName, const std::string& folderPath);

      /// @brief replace wildcard with profileName in the string
      /// @param[in] str  the string with wildcard
      /// @param[in] profileName  the profileName to replace with
      /// return Return value replaced with profile name.
      static std::string replaceWildCardWithProfileName(std::string const& str, std::string const& profileName);

    private: // Data
      friend class ::TestBackupRestoreManager;
      IAxisProfileManagerInternal *const m_profileManager{};                   ///< Pointer to the axisprofile manager
      comm::datalayer::IClient *const m_dlClient{};                            ///< Pointer to a datalayer client
      const PersistenceHelperItf *m_persistenceHelper{};                       ///< Pointer to datalayer persistence helper

      const std::string m_bundleSubfolder = "axisprofile/";                    ///< The configuration folder name of this bundle in $SNAP_COMMON/activeConfiguration
      const std::string m_typeAddress;                                         ///< The datalayer node of the registered profiles config type

    };
  } // namespace axisprofile
} // namespace comm
