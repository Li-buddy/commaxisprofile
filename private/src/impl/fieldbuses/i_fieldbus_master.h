#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines interface of fieldbus master.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <set>
#include "fieldbus_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm::datalayer
{
  class IDataLayerFactory;
}

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
  // Forward references
  // ---------------------------------------------------------------------------
  class IAxisProfileInternal;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is interface class of fieldbus master object.
  class IFieldbusMaster
  {
  public: // methods

    /// @brief destructor
    virtual ~IFieldbusMaster() noexcept = default;

    /// @brief This method get name of fieldbus master.
    /// @return Master name.
    virtual const char* getMasterName() = 0;

    /// @brief This method get all linked profiles.
    /// @return Set of linked profiles.
    virtual const std::set<IAxisProfileInternal*>& getLinkedProfiles() = 0;

    /// @brief This method add a profile pointer into internal set container.
    /// @param[in]  profile   Pointer to a profile which lined to this master
    virtual void addLinkedProfile(IAxisProfileInternal* profile) = 0;

    /// @brief This method init datalayer-rt of fieldbus master.
    /// @param[in]  abort     Aborting initialization when true
    /// @return Return value indicating success or error.
    virtual AxisProfileResult initRtData(bool abort) = 0;

    /// @brief This method process datalayer-rt of fieldbus master.
    /// @return Return value indicating success or error.
    virtual AxisProfileResult ProcessRtData() = 0;

    /// @brief This method gets actual distribution clock time, DC base time when fieldbus is EtherCAT.
    ///        Unit is second.
    ///
    /// @note ActDcBaseTime = EcatTaskStartTime + SendOffset + OutputShiftTime
    /// 
    /// @return last DC base time.
    virtual double getActDcBaseTime() = 0;

    /// @brief This method get fieldbus cycle time in seconds
    /// @return fieldbus cycle time in second
    virtual double getFieldbusCycleTime() = 0;
  };
}
