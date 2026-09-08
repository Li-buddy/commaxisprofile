#pragma once

/// @file          i_aux_job_handler.h
///
/// This header file is part of the axisprofile library and implements the
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design. 


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "profiles/profile_defs.h"
#include "profiles/i_axisprofile_sharing.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    /// @brief IAuxJobHandler
    /// This is the internal interface of the auxiliary job.
    class IAuxJobHandler :public IAxisProfileSharing
    {
    public: // methods

      virtual ~IAuxJobHandler() noexcept override = default;

      /// @brief  This method add a function pointer to auxiliary job list.
      ///         The function pointer will be added when program start-up phase
      /// @return Return STS_OK when the job is added successfully.
      virtual AxisProfileResult addAuxJob(auxJobs job) = 0;

      /// @brief  This method is called in RT-Thread.
      /// @return Return STS_OK when success.
      virtual AxisProfileResult runAllAuxJobs() = 0;

      /// @brief this method will clear all jobs form container.
      virtual void clearAllJobs() = 0;
    };
  }
}
