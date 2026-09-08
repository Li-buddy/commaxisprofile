#pragma once

// ---------------------------------------------------------------------------
//
//! @file       backup_restore_mgr.h
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
//! This header file is part of the axisprofile library and implements the
//! backup restore manager. This class manages the remanent saving/loading
//! of axis profile configuration data to/from a JSON file.
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_backup_restore_mgr.h"
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
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
    const std::string MSG_ABORT_LOAD_CFG = "Abort load configuration!";  //!< Detailed diag info for error occurs during load config process
    const std::string MSG_ABORT_SAVE_CFG = "Abort save configuration!";  //!< Detailed diag info for error occurs during save config process
    const std::string MSG_ABORT_LOAD_SYSTEM_CFG = "Abort load system configuration!";  //!< Detailed diag info for error occurs during load system config process
    const std::string MSG_ABORT_SAVE_SYSTEM_CFG = "Abort save system configuration!";  //!< Detailed diag info for error occurs during save system config process
    const std::string MSG_ABORT_MAKE_PERSISTENCE = "Abort make persistence!";          //!< Detailed diag info for error occurs during make persistence process

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

    //! @class BackupRestoreMgr
    //! This is the backup/restore manager of the axisprofile.
    class BackupRestoreMgr : public IBackupRestoreMgr
    {
    public: // Methods
      //! @brief constructor
      //! @param[in]   profileMgr          Pointer to the axis profile manager
      //! @param[in]   dlClient            Pointer to the datalayer client
      //! @param[in]   persistenceHelper   Pointer to the persistence helper
      BackupRestoreMgr(IAxisProfileManagerInternal *const profileMgr, comm::datalayer::IClient *const dlClient, const PersistenceHelperItf *persistenceHelper);

      //! @brief destructor
      virtual ~BackupRestoreMgr() noexcept override;

      AxisProfileResult load(const std::string &profileName, const std::string &dlCfgUri, const std::string &filePath) final;
      AxisProfileResult save(const std::string &profileName, const std::string &dlCfgUri, const std::string &filePath) final;
      AxisProfileResult loadSystemCfg(const std::string &folderPath) final;
      AxisProfileResult saveSystemCfg(const std::string &folderPath) final;

      AxisProfileResult loadSystemCfg(const comm::datalayer::Variant &persistenceParam) final;
      AxisProfileResult saveSystemCfg(const comm::datalayer::Variant &persistenceParam) final;
      AxisProfileResult makePersistent(const comm::datalayer::Variant& persistenceParam, const std::string& axisprofileName) final;

    private: // Types
      //! @brief Struct that stores all relevant information of a Json configuration file.
      struct FileInformation
      {
        std::string m_profileName; //!< Name of the profile (derived from the file name)
        std::string m_dlURI;       //!< Datalayer configuration node URI for this profile
        std::string m_path;        //!< File path (including file name)
      };

    private: // Methods
      //! @brief This method extracts/generates all necessary information from the given file path.
      //! @param[in]    filePath   The path of the file (from the persistence helper)
      //! @param[out]   fileInfo   The file information variable that will be initialized by the method
      //! @return Return value indicating success or error.
      AxisProfileResult getFileInfoFromPath(std::string const &filePath, FileInformation &fileInfo) const;

      //! @brief extract the name of a file from the given path.
      //! @param[in]   filePath   the file path
      //! @param[out]  name       the extracted file name (without file extension)
      //! @return Return value indicating success or error.
      AxisProfileResult getFileNameFromPath(const std::string &filePath, std::string &name) const;

      //! @brief This method determines the folder/file path from the given input data. If input data is null or empty, the default will be used.
      //! @param[in]    data          Input string data
      //! @param[in]    profileName   Name of the axisprofile. If present, "<profileName>.json" will be added to the path.
      //! @param[out]   path          The determined path
      void determineWorkPath(const std::string &data, const std::string &profileName, std::string &path) const;

      //! @brief Load the given configuration.
      //! @param[in]   folderPath   Path to the folder with axisprofile configuration.
      //! @param[in]   loadId       Identification for the load process.
      //! @return Return value indicating success or error.
      AxisProfileResult executeCfgLoad(const std::string &folderPath, const std::string &loadId);

      //! @brief Check if the loading of the given configuration is possible.
      //! @param[in]   folderPath   Path to the folder with axisprofile configuration.
      //! @param[in]   loadId       Identification for the load process.
      //! @return Return value indicating success or error.
      AxisProfileResult queryCfgLoad(const std::string &folderPath, const std::string &loadId);

      //! @brief Save the actual configuration of the axisprofiel system to the given folder.
      //! @param[in]   folderPath   Path to the folder for the axisprofile configuration.
      //! @param[in]   loadId       Identification for the load process.
      //! @return Return value indicating success or error.
      AxisProfileResult executeCfgSave(const std::string &folderPath, const std::string &loadId);

      //! @brief get all configuration files in the given folder.
      //! @param[in]   folderPath       Path to the folder with motion configuration.
      //! @param[in]   loadId           Identification for the load process.
      //! @param[out]  availableFiles   Vector with the list of the configuration files.
      //! @return Return value indicating success or error.
      AxisProfileResult getAvailableFiles(const std::string &folderPath, const std::string &loadId, std::vector<std::string> &availableFiles);

      //! @brief compare the configuration of the given files with the actual configuration.
      //! @param[in]  files   Vector with the list of the configuration files.
      //! @return Returns true if the objects of the configurations differ.
      bool newOrDeletedObjectsAvailable(std::vector<FileInformation> const &filesInformation);

      //! @brief compare the configuration of the given files with the actual configuration and check if the changes are valid in state "Running"
      //! @param[in]  loadId  Identification for the load process.
      //! @param[in]  files   Vector with list of the configuration files.
      //! @return Returns true data changed that is invalid in state "Running".
      bool checkChangedData(const std::string &loadId, std::vector<FileInformation> &filesInformation);

      //! @brief compare the given configuration with the actual configuration and check if the changes are valid in state "Running".
      //! @param[in]  file      File information for a profile.
      //! @param[in]  cfgData   Vector with the new configuration of the profile.
      //! @return Returns true if the changes are valid in to load in state "Running".
      bool checkConfigData(FileInformation fileInformation, comm::datalayer::Variant *cfgData);

      //! @brief extract the given persistence param flatbuffer to the different values.
      //! @param[in]   paramFb       Fatbuffer with the persistence parameters
      //! @param[out]  configPath    Relative configuration path from the solution
      //! @param[out]  id            Identification for the load process.
      //! @param[out]  phase         Loading phase for the load process.
      //! @return Return value indicating success or error.
      AxisProfileResult extractPersistenceData(const comm::datalayer::PersistenceParam *paramFb, std::string &configPath, std::string &id, std::string &phase);

      /// @brief Save the given single axisprofile configuration.
      /// @param[in]   axisprofileName  Name of the single axisprofile object that should be saved.
      /// @param[in]   folderPath       Path to the folder for the axisprofile configuration.
      /// @return Return value indicating success or error.
      AxisProfileResult exeMakePersistentCfgSave(const std::string& axisprofileName, const std::string& folderPath);

    private:                                                 // Data
      IAxisProfileManagerInternal *const m_profileManager{}; //!< Pointer to the axisprofile manager
      comm::datalayer::IClient *const m_dlClient{};          //!< Pointer to a datalayer client
      const PersistenceHelperItf *m_persistenceHelper{};     //!< Pointer to datalayer persistence helper

      const std::string m_bundleSubfolder = "axisprofile/"; //!< The configuration folder name of this bundle in $SNAP_COMMON/activeConfiguration
      const std::string m_typeAddress;                      //!< The datalayer node of the registered profiles config type
    };
  } // namespace axisprofile
} // namespace comm
