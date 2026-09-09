#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines CanOpen set operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_base.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  /// @brief This is class of job object of CANOpen set operation mode.
  class CanopenJobSetOpMode : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit CanopenJobSetOpMode(ProfileBasicInfo& profileInfo,
                                 IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    virtual ~CanopenJobSetOpMode() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "CanopenJobSetOpMode"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;

  private: // data

    AxisProfileOperationMode m_requestedOpMode {AxisProfileOperationMode::UNDEFINED};  ///< Requested operation mode
  };
}