#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines CanOpen get operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  /// @brief This is class of job object of CANOpen get operation mode.
  class CanopenJobGetOpMode : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit CanopenJobGetOpMode(ProfileBasicInfo& profileInfo,
                                 IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~CanopenJobGetOpMode() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "CanopenJobGetOpMode"; }

    JobIDType getID() final { return jobName(); }
    const IContainer* getModeData();

  protected: // methods

    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;

  private: // data

    JobDataOperationMode  m_opModeData {};  ///< Job output data for operation mode
    std::uint8_t m_rawMode {};              ///< Raw operation mode from device
    bool m_isCyclicData {false};
  };
}
