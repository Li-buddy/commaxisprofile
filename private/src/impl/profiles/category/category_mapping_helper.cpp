/// @file
///
/// @brief This source file is part of the axisprofile library and implement the CategoryMappingHelper.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "drive/category_drive_defs.h"
#include "encoder/category_encoder_defs.h"
#include "category_mapping_helper.h"
#include "common/debug_trace.h"
#include "profiles/axisprofile_factory.h"

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
AxisProfileResult CategoryMappingHelper::getAvailableInputMappingItems(const AxisProfileCategory category, std::vector<InputMappingValueID>& vecInputItems)
{
  switch (category)
  {
    case AxisProfileCategory::CATEGORY_DRIVE:
      vecInputItems =  category_drive::AVAILABLE_INPUT_MAPPING;
      break;
    case AxisProfileCategory::CATEGORY_ENCODER:
      vecInputItems =  category_encoder::AVAILABLE_INPUT_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CategoryMappingHelper::getAvailableOutputMappingItems(const AxisProfileCategory category, std::vector<OutputMappingValueID>& vecOutputItems)
{
  switch (category)
  {
    case AxisProfileCategory::CATEGORY_DRIVE:
      vecOutputItems =  category_drive::AVAILABLE_OUTPUT_MAPPING;
      break;
    case AxisProfileCategory::CATEGORY_ENCODER:
      vecOutputItems =  category_encoder::AVAILABLE_OUTPUT_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

bool CategoryMappingHelper::checkInputMappingItemRequired(const AxisProfileCategory category, const AxisProfileType profileType, InputMappingValueID inputItems)
{
  std::map<InputMappingValueID, MappingCfgInfo> inputMappingCfgInfo;
  AxisProfileFactory::getInputMappingValueIDList(inputMappingCfgInfo, category, profileType);
  return inputMappingCfgInfo[inputItems].m_bIsMandatory;
}

bool CategoryMappingHelper::checkOutputMappingItemRequired(const AxisProfileCategory category, const AxisProfileType profileType, OutputMappingValueID outputItems)
{
  std::map<OutputMappingValueID, MappingCfgInfo> outputMappingCfgInfo;
  AxisProfileFactory::getOutputMappingValueIDList(outputMappingCfgInfo, category, profileType);
  return outputMappingCfgInfo[outputItems].m_bIsMandatory;
}

MappingGroup CategoryMappingHelper::getInputMappingGroup(const AxisProfileCategory category, const AxisProfileType profileType, InputMappingValueID inputItems)
{
  std::map<InputMappingValueID, MappingCfgInfo> inputMappingCfgInfo;
  AxisProfileFactory::getInputMappingValueIDList(inputMappingCfgInfo, category, profileType);
  return inputMappingCfgInfo[inputItems].m_mappingGroup;
}

MappingGroup CategoryMappingHelper::getOutputMappingGroup(const AxisProfileCategory category, const AxisProfileType profileType, OutputMappingValueID outputItems)
{
  std::map<OutputMappingValueID, MappingCfgInfo> outputMappingCfgInfo;
  AxisProfileFactory::getOutputMappingValueIDList(outputMappingCfgInfo, category, profileType);
  return outputMappingCfgInfo[outputItems].m_mappingGroup;
}

AxisProfileResult CategoryMappingHelper::getMandatoryInputMappingItems(const AxisProfileCategory category, std::vector<InputMappingValueID>& vecInputItems)
{
  switch (category)
  {
    case AxisProfileCategory::CATEGORY_DRIVE:
      vecInputItems = category_drive::MANDATORY_INPUT_MAPPING;
      break;
    case AxisProfileCategory::CATEGORY_ENCODER:
      vecInputItems = category_encoder::MANDATORY_INPUT_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult CategoryMappingHelper::getMandatoryOutputMappingItems(const AxisProfileCategory category, std::vector<OutputMappingValueID>& vecOutputItems)
{
  switch (category)
  {
    case AxisProfileCategory::CATEGORY_DRIVE:
      vecOutputItems = category_drive::MANDATORY_OUTPUT_MAPPING;
      break;
    case AxisProfileCategory::CATEGORY_ENCODER:
      vecOutputItems = category_encoder::MANDATORY_OUTPUT_MAPPING;
      break;
    default:
      APR_TRACE_ERROR("Profile category not right,It should be CATEGORY_ENCODER or CATEGORY_DRIVE!");
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return AxisProfileResult::STS_OK;
}
