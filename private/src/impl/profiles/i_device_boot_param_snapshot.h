#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines the
/// interface for reading protocol-specific boot parameter snapshots.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_axisprofile_sharing.h"
#include "profiles/profile_defs.h"

#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  /// @brief Default buffer size for SoE list parameter access.
  constexpr std::size_t LIST_BUFF_DEFAULT_SIZE = 64;


  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief protocol types of the boot parameter snapshots.
  enum class BootSnapshotProtocol : std::uint8_t
  {
    UNKNOWN = 0,  ///< Unknown or not defined protocol.
    SERCOS = 1,   ///< Sercos protocol
    CANOPEN = 2,  ///< CANopen protocol
  };

  /// @brief Generic persisted boot parameter value type.
  enum class BootParamValueType : std::uint16_t
  {
    NONE_TYPE   = 0,       ///< No value.
    UNSIGNED16 = 1,       ///< Unsigned 16-bit value.
    SIGNED16   = 2,       ///< Signed 16-bit value.
    UNSIGNED32 = 3,       ///< Unsigned 32-bit value.
    SIGNED32   = 4,       ///< Signed 32-bit value.
    BYTEARRAY  = 5,       ///< Byte-array value.
    ERROR_TYPE  = 0xFFFF,  ///< Invalid value type.
  };

  /// @brief Generic persisted boot parameter value payload.
  struct BootParamValue
  {
    std::uint16_t valueUint16 {};                 ///< Unsigned 16-bit value.
    std::int16_t valueInt16 {};                   ///< signed 16-bit value.
    std::int32_t valueInt32 {};                   ///< Signed 32-bit value.
    std::uint32_t valueUint32 {};                 ///< Unsigned 32-bit value.
    std::vector<std::uint8_t> valueByteArray {};  ///< Byte-array value.
  };

  /// @brief One persisted boot parameter item.
  struct BootParamItem
  {
    BootParamValueType type {BootParamValueType::NONE_TYPE};   ///< Value type.
    BootParamValue value {};                                  ///< Value payload.
  };

  using BootParamMap = std::map<std::string, BootParamItem>;

  /// @brief Persisted boot parameter snapshot.
  struct BootParamSnapshot
  {
    std::uint8_t schemaVersion {1};                                 ///< Schema version of the snapshot, used for compatibility check when loading from file.
    std::string profileName;                                        ///< Axis profile name.
    BootSnapshotProtocol protocol {BootSnapshotProtocol::UNKNOWN};  ///< Protocol of the persisted snapshot.
    std::uint32_t checksumValue {};                                 ///< Checksum value of the persisted snapshot.
    BootParamMap paramMap {};                                       ///< Persisted parameter list map.
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This interface reads drive boot parameter snapshots from the device.
  class IDeviceBootParamSnapshot : public IAxisProfileSharing
  {
  public: // methods

    /// @brief Destructor
    virtual ~IDeviceBootParamSnapshot() noexcept = default;

    /// @brief Store the persisted boot parameter snapshot.
    /// @param[in] snapshot Persisted boot parameter snapshot.
    virtual void setBootParamSnapshot(const BootParamSnapshot& snapshot) = 0;

    /// @brief Get the persisted boot parameter snapshot.
    /// @return Persisted boot parameter snapshot.
    virtual const BootParamSnapshot& getBootParamSnapshot() const = 0;
  };
}
