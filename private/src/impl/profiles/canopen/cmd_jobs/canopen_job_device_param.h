#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the job of device parameter read/write.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_job_base.h"
#include "profiles/canopen/canopen_param_access.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  constexpr std::uint8_t BUF_SIZE = 8; ///< Size of read/write buffer, extend to 64 have an use case

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  /// @brief structure stores job data of ref@ CanopenParamStruct
  struct JobDeviceParamCoE : public CanopenParamStruct
  {
    /// @brief Constructor
    JobDeviceParamCoE() : CanopenParamStruct(0, 0, nullptr, 0) {};

    /// @brief This method compares IDN and read/write type
    /// @param[in]  id          Index of the parameter
    /// @param[in]  type        Read/write type
    /// @return True if both are equal
    bool compareIdAndType(const ObjectDirEntry& id, ReadWriteType type) const
    {
      return (id == m_objDirEntry) && (m_readWriteType == type);
    }

    /// @brief Getter of generic type value
    /// @tparam     T           Type of value
    /// @param[out] value       The value to be get
    template<typename T>
    void getData(T& value) const
    {
      value = *static_cast<T*>(m_ptrData);
    }

    /// @brief Setter of generic type value
    /// @tparam T               Type of value
    /// @param[in]  idn         Index of the parameter
    /// @param[in]  type        Read/write type
    /// @param[in]  value       The value to be set
    template<typename T>
    void setData(ObjectDirEntry idn, ReadWriteType type, const T& value, std::size_t size)
    {
      m_objDirEntry = idn;
      m_readWriteType = type;
      if (type == ReadWriteType::WRITE)
      {
        dataSize = std::size_t(value);
        *static_cast<T*>(m_ptrData) = value;
      }
      else
      {
        dataSize = size;
      }
    }
  };

  ///< Named type for job data of ref@ CanopenParamStruct
  DECLARE_NAMED_TYPE(DatJobDeviceParamCoE, JobDeviceParamCoE);

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is class of job object of device parameter read/write.
  class CanopenJobDeviceParam : public AxisprofileJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]  profileInfo   Reference to the Profile Basic Information
    /// @param[in]  shareHandle   Pointer to the sharing handler
    explicit CanopenJobDeviceParam(ProfileBasicInfo& profileInfo,
                                   IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~CanopenJobDeviceParam() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() {return "CanopenJobDeviceParam";}

    JobIDType getID() final {return jobName();}
    const IContainer* getJobData() final;
    IContainer* getJobDataRef() final;

  protected: // methods

    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;

  private: // data

    RequestStatemachine m_smStatus {};            ///< Status request state machine
    CanopenParamAccess* m_paramAccessIf {};       ///< CANOpen parameter access
    DatJobDeviceParamCoE m_data {};               ///< Data of the object
    std::array<std::uint8_t, BUF_SIZE> m_buf {};  ///< Read buffer
  };
}
