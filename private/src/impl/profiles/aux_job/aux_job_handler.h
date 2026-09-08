#pragma once

/// @file      aux_job_handler.h
///
/// This header file is part of the axisprofile library and implements the
/// aux cyclic job
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_aux_job_handler.h"
#include <list>

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
    // Declaration of macros
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    /// @brief This is the class for the handling of the data sharing handler between every profile sub-module
    class AuxJobHandler : public IAuxJobHandler
    {
    public: // methods

      /// @brief Constructor
      AuxJobHandler() = default;
      /// @brief Destructor
      virtual ~AuxJobHandler() noexcept override;

      AxisProfileResult addAuxJob(auxJobs job) final;
      AxisProfileResult runAllAuxJobs() final;
      void clearAllJobs() final;

    protected: // method

      void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    private: // data

      std::list<auxJobs> m_auxJobs;                       ///< job list which will be called cyclically.
      IAxisProfileSharingHandler* m_sharingHandler {};    ///< mediator for sharing data 
    };
  }
}
