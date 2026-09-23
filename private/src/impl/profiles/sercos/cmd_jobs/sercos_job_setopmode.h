#pragma once

///
/// @brief This file is part of the axisprofile library and defines the Sercos
///        operation-mode command job.
///

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/axisprofile_job_base.h"
#include "profiles/profile_defs.h"
#include "profiles/i_axisprofile_status_control_internal.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::sercos
{
  /// @brief Job for setting and verifying the Sercos operation mode.
  class SercosJobSetOpMode : public AxisprofileJobBase
  {
  public: // methods

    /// @brief Constructor
    explicit SercosJobSetOpMode(ProfileBasicInfo& profileInfo,
                                IAxisProfileSharingHandler* shareHandle = nullptr);

    /// @brief Destructor
    ~SercosJobSetOpMode() noexcept override = default;

    /// @brief This static method returns the job name.
    static const char* jobName() { return "SercosJobSetOpMode"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;

  private: // data

    IAxisProfileStatusControlInternal* m_statusControlIf {};
    AxisProfileOperationMode m_requestedOperationMode {AxisProfileOperationMode::UNDEFINED};
  };
}
