/// @file
///
/// @brief This file is part of the axisprofile library and implements factory to create fieldbus adapters.
///
/// SPDX-FileCopyrightText: Copyright (c) 2020-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "fieldbus_adapter_coe.h"
#include "fieldbus_adapter_factory.h"
#include "fieldbus_adapter_soe.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Declaration of constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward function references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
AxisProfileResult FieldbusAdapterFactory::createFieldbusAdapter(ProfileBasicInfo& profileBasicInfo, IFieldbusAdapter** newFieldbusAdapter)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch (profileBasicInfo.m_axisProfileType)
  {
    case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
      *newFieldbusAdapter = new FieldbusAdapterSoE(&profileBasicInfo.m_profileTrace);
      break;
    case AxisProfileType::CANOPEN_OVER_ETHERCAT:
      *newFieldbusAdapter = new FieldbusAdapterCoE(&profileBasicInfo.m_profileTrace);
      break;
    default:
      result = AxisProfileResult::ERR_NOTIMPLEMENTED;
      break;
  }

  return result;
}
