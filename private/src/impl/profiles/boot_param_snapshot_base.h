#pragma once

/// @file
///
/// @brief Common abstract base class for protocol-specific boot parameter snapshots.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_device_boot_param_snapshot.h"

#include <memory>
#include <string>
#include <variant>

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
  /// @brief Lightweight parameter identity wrapper for protocol-specific boot parameters.
  class BootParamId
  {
  public:
    /// @brief Construct from a Sercos IDN.
    /// @param[in] idn Sercos parameter IDN.
    explicit BootParamId(std::uint16_t idn) : m_value(idn) {}

    /// @brief Construct from a CANopen object entry.
    /// @param[in] objEntry CANopen object index/sub-index.
    explicit BootParamId(const ObjectDirEntry& objEntry) : m_value(objEntry) {}

    /// @brief Check whether this ID stores a Sercos IDN.
    /// @return true when the internal value is std::uint16_t.
    bool isIdn() const { return std::holds_alternative<std::uint16_t>(m_value); }

    /// @brief Check whether this ID stores a CANopen object entry.
    /// @return true when the internal value is ObjectDirEntry.
    bool isObjectDirEntry() const { return std::holds_alternative<ObjectDirEntry>(m_value); }

    /// @brief Get Sercos IDN pointer when available.
    /// @return Pointer to IDN, otherwise nullptr.
    const std::uint16_t* getIdn() const { return std::get_if<std::uint16_t>(&m_value); }

    /// @brief Get CANopen object entry pointer when available.
    /// @return Pointer to ObjectDirEntry, otherwise nullptr.
    const ObjectDirEntry* getObjectDirEntry() const { return std::get_if<ObjectDirEntry>(&m_value); }

  private:
    /// @brief Variant-backed storage for the parameter identity.
    std::variant<std::uint16_t, ObjectDirEntry> m_value {};
  };

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief Common base for protocol-specific boot parameter snapshot readers.
  class BootParamSnapshotBase : public IDeviceBootParamSnapshot
  {
  public: // methods

    /// @brief Virtual destructor.
    ~BootParamSnapshotBase() noexcept override = default;

    /// @brief Set the externally loaded boot parameter snapshot.
    /// @param[in] snapshot Snapshot payload to apply.
    void setBootParamSnapshot(const BootParamSnapshot& snapshot) final;

    /// @brief Get the current boot parameter snapshot.
    /// @return Current snapshot payload.
    const BootParamSnapshot& getBootParamSnapshot() const final;

  protected: // data types

    /// @brief Unified descriptor abstraction for supported boot parameter metadata.
    struct BootParamDescriptorBase
    {
      /// @brief Value type of the boot parameter.
      BootParamValueType m_type {BootParamValueType::NONE_TYPE};

      /// @brief Expected raw byte length for fixed-size parameter types.
      std::size_t m_valueLen {};
    };

  protected: // methods

    void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    /// @brief Construct the common snapshot base.
    /// @param[in] profileBasicInfo Shared profile runtime context.
    /// @param[in] protocol Protocol tag for snapshot compatibility checks.
    BootParamSnapshotBase(ProfileBasicInfo& profileBasicInfo, BootSnapshotProtocol protocol);

    /// @brief Read one boot parameter with cache-first behavior.
    /// @param[in] paramId Protocol-specific parameter identity.
    /// @param[out] bufPtr Output buffer for raw parameter bytes.
    /// @param[in] bufLen Output buffer size in bytes.
    /// @param[in] forceReadFromDrive When true, bypass cache and read from drive.
    /// @return Return value indicating success or error.
    AxisProfileResult getParameterImpl(const BootParamId& paramId, std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromDrive);

    /// @brief Check whether the configured snapshot matches current protocol and state.
    /// @return true when snapshot exists and protocol matches.
    bool hasValidSnapshot() const;

    /// @brief Read one boot parameter from drive and update snapshot cache.
    /// @param[in] paramId Protocol-specific parameter identity.
    /// @param[out] bufPtr Output buffer for raw parameter bytes.
    /// @param[in] bufLen Output buffer size in bytes.
    /// @return Return value indicating success or error.
    AxisProfileResult readBootParameterFromDrive(const BootParamId& paramId, std::uint8_t* bufPtr, std::size_t bufLen);

  protected: // virtual methods

    /// @brief Convert protocol-specific parameter identity into snapshot map key.
    /// @param[in] paramId Protocol-specific parameter identity.
    /// @return Stable key string used by snapshot map.
    virtual std::string parameterToString(const BootParamId& paramId) const = 0;

    /// @brief Resolve metadata descriptor for one boot parameter.
    /// @param[in] paramId Protocol-specific parameter identity.
    /// @return Pointer to descriptor when known, otherwise nullptr.
    virtual const BootParamDescriptorBase* getBootParamDescriptor(const BootParamId& paramId) const = 0;

    /// @brief Build protocol-specific fieldbus request data for one parameter.
    /// @param[in] paramId Protocol-specific parameter identity.
    /// @return Request data object, or nullptr when identity is invalid.
    virtual std::unique_ptr<ParamReqDataBase> generateReqData(const BootParamId& paramId) const = 0;

  private: // methods

    /// @brief Read one parameter value from persisted snapshot map.
    /// @param[in] cache Cached parameter item.
    /// @param[out] bufPtr Output buffer for raw parameter bytes.
    /// @param[in] bufLen Output buffer size in bytes.
    /// @return Return value indicating success or error.
    static AxisProfileResult readBootParameterFromCache(const BootParamItem& cache, std::uint8_t* bufPtr, std::size_t bufLen);

  protected: // data

    /// @brief Profile context used for protocol-specific parameter access.
    ProfileBasicInfo& m_profileBasicInfo;

    /// @brief Pointer to the sharing handler.
    IAxisProfileSharingHandler* m_sharingHandler {};

    /// @brief In-memory boot parameter snapshot payload.
    BootParamSnapshot m_snapshot {};

    /// @brief Flag that indicates whether snapshot has been configured.
    bool m_snapshotConfigured {false};

    /// @brief Reusable descriptor cache for derived class descriptor adaptation.
    mutable BootParamDescriptorBase m_descriptorCache {};

  };
}
