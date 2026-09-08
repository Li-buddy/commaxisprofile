/// @file
///
/// @brief This source file is part of the axisprofile library and implement the CategoryAdapterDrive class.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "category_adapter_drive.h"
#include "diagnosis/dia.h"
#include "common/string_helper.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------
CategoryAdapterDrive::CategoryAdapterDrive(const ProfileBasicInfo& profileBasicInfo,
                                           const AcknowledgeTelegram* at,
                                           MasterDataTelegram* mdt)
  :CategoryAdapter(profileBasicInfo, at, mdt, AxisProfileCategory::CATEGORY_DRIVE)
{
}
