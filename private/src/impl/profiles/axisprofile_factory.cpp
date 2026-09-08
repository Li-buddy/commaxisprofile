/// @file
///
/// This source file is part of the axisprofile library and implements
/// factory to create every supported axisprofile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_factory.h"
#include "axisprofile_internal_defs.h"
#include "canopen/canopen_defs.h"
#include "canopen/canopen_profile.h"
#include "category/encoder/category_encoder_defs.h"
#include "common/debug_trace.h"
#include "diagnosis/dia.h"
#include "i_axisprofile_internal.h"
#include "sercos/sercos_defs.h"
#include "sercos/sercos_profile.h"
#include <string>

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
AxisProfileResult AxisProfileFactory::createProfile(const std::string& profileName, const AxisProfileType axisProfileType, IAxisProfileInternal** newProfile)
{
  //get profile type information from axisprofile type
  DriveProfileType driveProfileType{};
  NetworkTechnology networkTechnology{};
  AxisProfileResult result = convertAxisProfileType(axisProfileType, driveProfileType, networkTechnology);
  if ( aprStatusFailed(result) )
  {
    return result;
  }
  //verify request information
  result = checkRequestedProfile(axisProfileType, driveProfileType, networkTechnology);
  if ( aprStatusFailed(result) )
  {
    return result;
  }

  //create detail type profile 
  switch ( driveProfileType )
  {
    case DriveProfileType::SERCOS:
      *newProfile = static_cast<IAxisProfileInternal*>( new sercos::SercosProfile(profileName, networkTechnology) );
      break;
    case DriveProfileType::CANOPEN:
      *newProfile = static_cast<IAxisProfileInternal*>( new canopen::CanopenProfile(profileName, networkTechnology) );
      break;
    case DriveProfileType::UNDEFINED:
    case DriveProfileType::MAX_VAL:
      // internal error
      return AxisProfileResult::ERR_NOK;

      // default case not necessary because invalid enum values will be detected in method checkRequestedProfile()
  }
  return result;
}

AxisProfileResult AxisProfileFactory::getInputMappingValueIDList(std::map<InputMappingValueID, MappingCfgInfo>& InputMappingValueID, const AxisProfileCategory profileCategory, const AxisProfileType axisProfileType)
{
  switch (profileCategory)
  {
  case AxisProfileCategory::CATEGORY_DRIVE:
    switch (axisProfileType)
    {
    case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
      InputMappingValueID = sercos::INPUT_ALL_MAPPING;
      break;
    case AxisProfileType::CANOPEN_OVER_ETHERCAT:
      InputMappingValueID = canopen::INPUT_ALL_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile type not right,It should be SERVODRIVE_OVER_ETHERCAT or CANOPEN_OVER_ETHERCAT!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    break;
  case AxisProfileCategory::CATEGORY_ENCODER:
    InputMappingValueID = category_encoder::INPUT_ALL_MAPPING;
    break;
  default:
    APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileFactory::getOutputMappingValueIDList(std::map<OutputMappingValueID, MappingCfgInfo>& OutputMappingValueID, const AxisProfileCategory profileCategory, const AxisProfileType axisProfileType)
{
  switch (profileCategory)
  {
  case AxisProfileCategory::CATEGORY_DRIVE:
    switch (axisProfileType)
    {
    case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
      OutputMappingValueID = sercos::OUTPUT_ALL_MAPPING;
      break;
    case AxisProfileType::CANOPEN_OVER_ETHERCAT:
      OutputMappingValueID = canopen::OUTPUT_ALL_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile type not right,It should be SERVODRIVE_OVER_ETHERCAT or CANOPEN_OVER_ETHERCAT!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
    break;
  case AxisProfileCategory::CATEGORY_ENCODER:
    OutputMappingValueID = category_encoder::OUTPUT_ALL_MAPPING;
    break;
  default:
    APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  return AxisProfileResult::STS_OK;
}

// -------------------------------------------------------------------------------
// ------------------------------ Private Methods --------------------------------
// -------------------------------------------------------------------------------

AxisProfileResult AxisProfileFactory::checkRequestedProfile(const AxisProfileType axisProfileType, const DriveProfileType driveProfileType, const NetworkTechnology networkTechnology)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch ( driveProfileType )
  {
    case DriveProfileType::SERCOS:
      switch ( networkTechnology )
      {
        case NetworkTechnology::ETHERCAT:   // SoE (Servodrive over Ethercat)
          if ( axisProfileType != AxisProfileType::SERVODRIVE_OVER_ETHERCAT ) { result = AxisProfileResult::ERR_NOTIMPLEMENTED; }
          break;
        default:
          result = AxisProfileResult::ERR_NOTIMPLEMENTED;
          break;
      }
      break;
    case DriveProfileType::CANOPEN:
      switch ( networkTechnology )
      {
        case NetworkTechnology::ETHERCAT:   // CoE (CANopen application over Ethercat)
          if ( axisProfileType != AxisProfileType::CANOPEN_OVER_ETHERCAT ) { result = AxisProfileResult::ERR_NOTIMPLEMENTED; }
          break;
        default:
          result = AxisProfileResult::ERR_NOTIMPLEMENTED;
          break;
      }
      break;
    default:
      result = AxisProfileResult::ERR_NOTIMPLEMENTED;
      break;
  }

  return result;
}

AxisProfileResult AxisProfileFactory::convertAxisProfileType(const AxisProfileType type, DriveProfileType& driveProfileType, NetworkTechnology& networkTechnology)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  switch ( type )
  {
    case AxisProfileType::SERVODRIVE_OVER_ETHERCAT:
      driveProfileType = DriveProfileType::SERCOS;
      networkTechnology = NetworkTechnology::ETHERCAT;
      break;
    case AxisProfileType::CANOPEN_OVER_ETHERCAT:
      driveProfileType = DriveProfileType::CANOPEN;
      networkTechnology = NetworkTechnology::ETHERCAT;
      break;

    default:
      Dia::sendDiagnosis(MAIN_DIAG_GENERAL_WRN, DETAILED_DIAG_FUNCTION_NOT_IMPLEMENTED, __FILE__, __FUNCTION__, __LINE__, "Profile type not implemented.");
      result = AxisProfileResult::ERR_NOTIMPLEMENTED;
      break;
  }

  return result;
}
