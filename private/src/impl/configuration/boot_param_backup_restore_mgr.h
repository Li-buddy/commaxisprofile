#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the implementation for boot parameter backup/restore.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_boot_param_backup_restore_mgr.h"
#include "comm/axisprofile/datatypes/deviceParamSnapshot_generated.h"
#include "comm/datalayer/datalayer.h"
#include "profiles/i_device_boot_param_snapshot.h"
#include <string>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
struct PersistenceHelperItf;
class TestBootParamBackupRestoreManager;

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

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileManagerInternal;
  class IDeviceBootParamSnapshot;

  /// @brief Concrete boot parameter backup/restore manager.
  class BootParamBackupRestoreMgr : public IBootParamBackupRestoreMgr
  {
  public:
    /// @brief Constructor.
    /// @param[in] profileMgr Axis profile manager.
    /// @param[in] persistenceHelper Persistence helper.
    /// @param[in] bundlePath Root path of the running bundle.
    BootParamBackupRestoreMgr(IAxisProfileManagerInternal *const profileMgr, const PersistenceHelperItf *persistenceHelper,
                              const std::string& bundlePath);

    /// @brief Extract boot parameter snapshot from datalayer flatbuffer variant.
    /// @param[in] data Variant holding flatbuffer data.
    /// @param[out] bootParam Persisted boot parameter snapshot.
    /// @return Operation result.
    static AxisProfileResult extractFlatbufferData(const comm::datalayer::Variant* data, BootParamSnapshot& bootParam);

    /// @brief Extract boot parameter snapshot from flatbuffer view.
    /// @param[in] data Flatbuffer view.
    /// @param[out] bootParam Persisted boot parameter snapshot.
    /// @return Operation result.
    static AxisProfileResult extractFlatbufferData(const fbtypes::bootParamSnapshot* data, BootParamSnapshot& bootParam);

    /// @brief Generate boot parameter snapshot flatbuffer.
    /// @param[in] fbb Flatbuffer builder.
    /// @param[in] bootParam Persisted boot parameter snapshot.
    /// @return Flatbuffer offset.
    static flatbuffers::Offset<fbtypes::bootParamSnapshot> generateFlatbufferData(flatbuffers::FlatBufferBuilder& fbb, const BootParamSnapshot& bootParam);

    AxisProfileResult loadBootParams(const std::string& profileName) const final;
    AxisProfileResult saveBootParam(const std::string& profileName) const final;

  private:  // methods
    /// @brief Extract boot parameter list from flatbuffer view.
    /// @param[in] data Flatbuffer parameter list view.
    /// @param[out] parameterMap Internal parameter map.
    /// @return Operation result.
    static AxisProfileResult extractParamList(const flatbuffers::Vector<flatbuffers::Offset<fbtypes::bootParam>>* data, BootParamMap& parameterMap);

    /// @brief Convert internal parameter map to flatbuffer list.
    /// @param[in] fbb Flatbuffer builder.
    /// @param[in] parameterMap Internal parameter map.
    /// @return Flatbuffer offset.
    static flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<fbtypes::bootParam>>> generateParamList(flatbuffers::FlatBufferBuilder& fbb, const BootParamMap& parameterMap);

    /// @brief Convert flatbuffer boot protocol to internal protocol.
    /// @param[in] protocol Flatbuffer protocol.
    /// @return Internal protocol.
    static BootSnapshotProtocol toInternalBootSnapshotProtocol(fbtypes::bootSnapshotProtocol protocol);

    /// @brief Convert internal boot protocol to flatbuffer protocol.
    /// @param[in] protocol Internal protocol.
    /// @return Flatbuffer protocol.
    static fbtypes::bootSnapshotProtocol toFlatbufferBootSnapshotProtocol(BootSnapshotProtocol protocol);

    /// @brief Convert flatbuffer value type to internal value type.
    /// @param[in] type Flatbuffer value type.
    /// @return Internal value type.
    static BootParamValueType toInternalParamValueType(fbtypes::paramValueType type);

    /// @brief Convert internal value type to flatbuffer value type.
    /// @param[in] type Internal value type.
    /// @return Flatbuffer value type.
    static fbtypes::paramValueType toFlatbufferParamValueType(BootParamValueType type);

    /// @brief Validate parameters and resolve boot parameter snapshot interface.
    /// @param[in] profileName Profile name.
    /// @param[in] filePath Path to source/target file.
    /// @param[in] abortMsg Abort message for trace output.
    /// @param[out] bootParamIF Resolved snapshot interface.
    /// @return Operation result.
    AxisProfileResult getBootParamSnapshotInterface(const std::string& profileName, const std::string& filePath,
                                                    const char *abortMsg, IDeviceBootParamSnapshot *&bootParamIF) const;

    /// @brief Resolve path to boot parameter snapshot bfbs file.
    /// @return Relative bfbs path.
    std::string getFbsFilePath() const;
    
    /// @brief Make the boot params data json file path using profile name.
    /// @param[in]   profileName  The profile name
    /// @return The determined boot params data json file path
    std::string makeBootParamsFilePath(const std::string& profileName) const;
    
  private:  // data
    friend class ::TestBootParamBackupRestoreManager;      ///< test class.

    IAxisProfileManagerInternal *const m_profileManager{}; ///< Axis profile manager.
    const PersistenceHelperItf *m_persistenceHelper{};     ///< Persistence helper.
    std::string m_bundlePath{};                            ///< Bundle root path for bfbs lookup.
  };
}
