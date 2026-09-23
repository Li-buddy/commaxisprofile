// ---------------------------------------------------------------------------
//
//! @file       mapping_handler.cpp
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This source file is part of the axisprofile library and implements the
//! mapping of internal values to dl-rt values
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "mapping_handler.h"
#include "common/debug_trace.h"
#include "diagnosis/dia.h"
#include "profiles/category/category_mapping_helper.h"

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

AxisProfileMappingHandler::AxisProfileMappingHandler(ProfileBasicInfo& profileBasicInfo)
  : m_profileTrace(&profileBasicInfo.m_profileTrace), m_axisProfileType(profileBasicInfo.m_axisProfileType)
{}

AxisProfileMappingHandler::~AxisProfileMappingHandler()
{
  m_inputMapping.clear();
  m_outputMapping.clear();
  m_inputMappingCfgInfo.clear();
  m_outputMappingCfgInfo.clear();
}

AxisProfileResult AxisProfileMappingHandler::configureInputValue(InputMappingValueID valueID, const char* valueURI)
{
  if (!valueURI)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "valueURI"));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::string temp(valueURI);
  APR_TRACE_MESSAGE("%s; InputConfig: ID=%s -> URI=%s", m_profileTrace->createTraceMsg(), inputMappingValueIdToString(valueID).c_str(), valueURI);
  return mappingConfig(valueID, temp, m_inputMapping);
}

AxisProfileResult AxisProfileMappingHandler::configureInputValues(const std::vector<InputMappingEntry>& inputConfig)
{
  for (const auto& elem : inputConfig)
  {
    if (aprStatusFailed(configureInputValue(elem.m_id, elem.m_uri.c_str())) || aprStatusFailed(configureInputCfgInfo(elem.m_id, {elem.m_group, elem.m_bRequired})))
    {
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }

  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileMappingHandler::configureInputCfgInfo(InputMappingValueID valueID, MappingCfgInfo mappingInfo)
{
  if (mappingInfo.m_mappingGroup == MappingGroup::UNKNOWN)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_INVALID_ARGUMENT, "Mapping Type is 'Unknown'"));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return mappingConfig(valueID, mappingInfo, m_inputMappingCfgInfo);
}

const std::vector< InputMappingEntry> AxisProfileMappingHandler::getConfiguredInputs() const
{
  std::vector<InputMappingEntry> inputMapping {};
  inputMapping.reserve(m_inputMapping.size());

  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();

  for (auto const& mappingItem : m_inputMapping)
  {
    // this part may can be simplified
    inputMapping.emplace_back(mappingItem.first,
                              mappingItem.second,
                              CategoryMappingHelper::getInputMappingGroup(category, m_axisProfileType, mappingItem.first),
                              CategoryMappingHelper::checkInputMappingItemRequired(category, m_axisProfileType, mappingItem.first));
  }
  return inputMapping;
}

const std::vector<InputMappingEntry> AxisProfileMappingHandler::getAvailableInputs() const
{
  std::vector<InputMappingEntry> inputMapping {};
  inputMapping.reserve(m_inputMapping.size());
  std::vector<InputMappingValueID> vecAvailableInputs;
  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();
  CategoryMappingHelper::getAvailableInputMappingItems(category, vecAvailableInputs);

  for (auto const& mappingItem : vecAvailableInputs)
  {
    // this part may can be simplified
    inputMapping.emplace_back(mappingItem,
                              "",
                              CategoryMappingHelper::getInputMappingGroup(category, m_axisProfileType, mappingItem),
                              CategoryMappingHelper::checkInputMappingItemRequired(category, m_axisProfileType, mappingItem));
  }
  return inputMapping;
}

void AxisProfileMappingHandler::clearInputConfiguration()
{
  m_inputMapping.clear();
  m_inputMappingCfgInfo.clear();
}

const char* AxisProfileMappingHandler::getConfiguredInputURI(InputMappingValueID valueID) const
{
  auto it = m_inputMapping.find(valueID);
  if (it == m_inputMapping.end())
  {
    return nullptr;
  }
  else
  {
    return it->second.c_str();
  }
}

AxisProfileResult AxisProfileMappingHandler::configureOutputValue(OutputMappingValueID valueID, const char* valueURI)
{
  if (!valueURI)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_EXEC_NULL, "valueURI"));
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  std::string temp(valueURI);
  APR_TRACE_MESSAGE("%s; OutputConfig: ID=%s -> URI=%s", m_profileTrace->createTraceMsg(), outputMappingValueIdToString(valueID).c_str(), valueURI);
  return mappingConfig(valueID, temp, m_outputMapping);
}

AxisProfileResult AxisProfileMappingHandler::configureOutputValues(const std::vector<OutputMappingEntry>& outputConfig)
{
  for (const auto& elem : outputConfig)
  {
    if (aprStatusFailed(configureOutputValue(elem.m_id, elem.m_uri.c_str())) || aprStatusFailed(configureOutputCfgInfo(elem.m_id, {elem.m_group, elem.m_bRequired})))
    {
      return AxisProfileResult::ERR_INVALID_ARGUMENT;
    }
  }

  return AxisProfileResult::STS_OK;
}

const std::vector< OutputMappingEntry> AxisProfileMappingHandler::getConfiguredOutputs() const
{
  std::vector<OutputMappingEntry> outputMapping {};
  outputMapping.reserve(m_outputMapping.size());
  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();
  for (auto const& mappingItem : m_outputMapping)
  {
    outputMapping.emplace_back(mappingItem.first,
                               mappingItem.second,
                               CategoryMappingHelper::getOutputMappingGroup(category, m_axisProfileType, mappingItem.first),
                               CategoryMappingHelper::checkOutputMappingItemRequired(category, m_axisProfileType, mappingItem.first));
  }
  return outputMapping;
}

const std::vector<OutputMappingEntry> AxisProfileMappingHandler::getAvailableOutputs() const
{
  std::vector<OutputMappingEntry> outputMapping {};
  outputMapping.reserve(m_outputMapping.size());

  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();
  std::vector<OutputMappingValueID> vecAvailableOutputs;
  CategoryMappingHelper::getAvailableOutputMappingItems(category, vecAvailableOutputs);

  for (auto const& mappingItem : vecAvailableOutputs)
  {
    outputMapping.emplace_back(mappingItem,
                               "",
                               CategoryMappingHelper::getOutputMappingGroup(category, m_axisProfileType, mappingItem),
                               CategoryMappingHelper::checkOutputMappingItemRequired(category, m_axisProfileType, mappingItem));
  }
  return outputMapping;
}

void AxisProfileMappingHandler::clearOutputConfiguration()
{
  m_outputMapping.clear();
  m_outputMappingCfgInfo.clear();
}

AxisProfileResult AxisProfileMappingHandler::configureOutputCfgInfo(OutputMappingValueID valueID, MappingCfgInfo mappingInfo)
{
  if (mappingInfo.m_mappingGroup == MappingGroup::UNKNOWN)
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_INVALID_ARGUMENT, "Mapping Type is 'Unknown'"));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }
  return mappingConfig(valueID, mappingInfo, m_outputMappingCfgInfo);
}

const char* AxisProfileMappingHandler::getConfiguredOutputURI(OutputMappingValueID valueID) const
{
  auto it = m_outputMapping.find(valueID);
  if (it == m_outputMapping.end())
  {
    return nullptr;
  }
  else
  {
    return it->second.c_str();
  }
}

AxisProfileResult AxisProfileMappingHandler::checkInputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping)
{
  const auto& inputItems = getConfiguredInputs();

  // get mandatory items from concrete category(encoder/drive profile)
  std::vector<InputMappingValueID> mandatoryInputs;
  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();
  CategoryMappingHelper::getMandatoryInputMappingItems(category, mandatoryInputs);
  for (const auto& inputItem : inputItems)
  {
    // ignore unmapped items
    if (inputItem.m_uri.empty())
    {
      continue;
    }

    // check name consistence only for non empty device name
    auto it = std::find(mandatoryInputs.cbegin(), mandatoryInputs.cend(), inputItem.m_id);
    if (it != mandatoryInputs.cend())
    {
      size_t pos = inputItem.m_uri.find("/");
      if (pos != std::string::npos && strlen(pProfile->getPropertyInterface()->getDeviceName()) != 0)
      {
        // check name consistency between mapping items and device name among current profile 
        std::string tmpName = inputItem.m_uri.substr(0, pos);
        if (tmpName != pProfile->getPropertyInterface()->getDeviceName())
        {
          Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG, __FILE__, __FUNCTION__, __LINE__,
                             "Input variable mapping inconsistence occurred!please check profile : %s, device : %s, mapping variable : %s",
                             pProfile->getProfileName(),
                             pProfile->getPropertyInterface()->getDeviceName(),
                             inputMappingValueIdToString(inputItem.m_id).c_str());
          return AxisProfileResult::ERR_PROFILE_CONFIG;
        }
      }
    }
    nCount++;
    // check name duplication mapping items among different profiles
    setMapping.insert(inputItem.m_uri);
    if (nCount > setMapping.size())
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG, __FILE__, __FUNCTION__, __LINE__,
                         "Input variable mapping inconsistence occurred!please check profile : %s, mapping variable : %s",
                         pProfile->getProfileName(),
                         inputMappingValueIdToString(inputItem.m_id).c_str());
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileMappingHandler::checkOutputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping)
{
  const auto& outputItems = getConfiguredOutputs();

  // get mandatory items from concrete category
  std::vector<OutputMappingValueID> mandatoryOutputs;
  auto categoryItf = m_sharingHandler->getCategoryIF();
  auto category = categoryItf->getProfileCategory();
  CategoryMappingHelper::getMandatoryOutputMappingItems(category, mandatoryOutputs);
  for (const auto& outputItem : outputItems)
  {
    // ignore unmapped items
    if (outputItem.m_uri.empty())
    {
      continue;
    }

    // check name consistence only for non empty device name
    auto it = std::find(mandatoryOutputs.cbegin(), mandatoryOutputs.cend(), outputItem.m_id);
    if (it != mandatoryOutputs.cend())
    {
      size_t pos = outputItem.m_uri.find("/");
      if (pos != std::string::npos && strlen(pProfile->getPropertyInterface()->getDeviceName()) != 0)
      {
        // check name consistency between mapping items and device name among current profile 
        std::string tmpName = outputItem.m_uri.substr(0, pos);
        if (tmpName != pProfile->getPropertyInterface()->getDeviceName())
        {
          Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG, __FILE__, __FUNCTION__, __LINE__,
                             "Output variable mapping inconsistence occurred!please check profile : %s, device : %s, mapping variable : %s",
                             pProfile->getProfileName(),
                             pProfile->getPropertyInterface()->getDeviceName(),
                             outputMappingValueIdToString(outputItem.m_id).c_str());
          return AxisProfileResult::ERR_PROFILE_CONFIG;
        }
      }
    }
    nCount++;
    // check name duplication mapping items among different profiles
    setMapping.insert(outputItem.m_uri);
    if (nCount > setMapping.size())
    {
      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_BOOT_VALIDATE_MAPPING_CFG, __FILE__, __FUNCTION__, __LINE__,
                         "Output variable mapping inconsistence occurred!please check profile : %s, mapping variable : %s",
                         pProfile->getProfileName(),
                         outputMappingValueIdToString(outputItem.m_id).c_str());
      return AxisProfileResult::ERR_PROFILE_CONFIG;
    }
  }
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileMappingHandler::initRealTimeMappingInput(const comm::datalayer::MemoryMap* memoryMap)
{
  if (!memoryMap)
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_INIT_RT_INPUT_MAPPING, __FILE__, __FUNCTION__, __LINE__,
                       "Nullptr to memoryMap! please check profile [%s]", m_profileTrace->createTraceMsg());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_inputMappingRT.clear();

  auto variables = memoryMap->variables();
  for (auto const& mappingItem : m_inputMapping)
  {
    auto memVariable = variables->LookupByKey(mappingItem.second.c_str());
    if (memVariable)
    {
      m_inputMappingRT.emplace_back(InputMappingInfoRT
                                    {
                                      memVariable->bitoffset(), memVariable->bitsize(), mappingItem.first
                                    }
      );

      APR_TRACE_MESSAGE("%s; InputInfo: ID=%s, URI=%s, BitOffset=%u, BitSize=%u", m_profileTrace->createTraceMsg(), inputMappingValueIdToString(mappingItem.first).c_str(),
                        mappingItem.second.c_str(), memVariable->bitoffset(), memVariable->bitsize());
    }
    else
    {
      // skip optional item
      auto categoryItf = m_sharingHandler->getCategoryIF();
      auto category = categoryItf->getProfileCategory();
      if (!CategoryMappingHelper::checkInputMappingItemRequired(category, m_axisProfileType, mappingItem.first))
      {
        continue;
      }

      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_INIT_RT_INPUT_MAPPING,__FILE__, __FUNCTION__, __LINE__,
                         "Requested variable '%s' is not found in the RT memory map! please check profile [%s]", mappingItem.second.c_str(), m_profileTrace->createTraceMsg());

      return AxisProfileResult::ERR_DLRT_MEMMAP_VAR_NOT_FOUND;
    }
  }

  determineInputAssigned();
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfileMappingHandler::initRealTimeMappingOutput(const comm::datalayer::MemoryMap* memoryMap)
{
  if (!memoryMap)
  {
    Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_INIT_RT_OUTPUT_MAPPING, __FILE__, __FUNCTION__, __LINE__,
                       "Nullptr to memoryMap! please check profile [%s]", m_profileTrace->createTraceMsg());
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  m_outputMappingRT.clear();
  auto variables = memoryMap->variables();

  for (auto const& mappingItem : m_outputMapping)
  {
    auto memVariable = variables->LookupByKey(mappingItem.second.c_str());
    if (memVariable)
    {
      m_outputMappingRT.emplace_back(OutputMappingInfoRT
                                     {
                                       memVariable->bitoffset(), memVariable->bitsize(), mappingItem.first
                                     }
      );
      APR_TRACE_MESSAGE("%s; InputInfo: ID=%s, URI=%s, BitOffset=%u, BitSize=%u", m_profileTrace->createTraceMsg(), outputMappingValueIdToString(mappingItem.first).c_str(),
                        mappingItem.second.c_str(), memVariable->bitoffset(), memVariable->bitsize());
    }
    else
    {
      // skip optional item
      auto categoryItf = m_sharingHandler->getCategoryIF();
      auto category = categoryItf->getProfileCategory();
      if (!CategoryMappingHelper::checkOutputMappingItemRequired(category, m_axisProfileType, mappingItem.first))
      {
        continue;
      }

      Dia::sendDiagnosis(MAIN_DIAG_CONFIG_ERR, DETAILED_DIAG_CONFIG_INIT_RT_OUTPUT_MAPPING, __FILE__, __FUNCTION__, __LINE__,
                         "Requested variable '%s' is not found in the RT memory map! please check profile [%s]", mappingItem.second.c_str(), m_profileTrace->createTraceMsg());

      return AxisProfileResult::ERR_DLRT_MEMMAP_VAR_NOT_FOUND;
    }
  }

  determineOutputAssigned();
  return AxisProfileResult::STS_OK;
}

const std::vector< InputMappingInfoRT>& AxisProfileMappingHandler::getInputRealtimeInfo() const
{
  return m_inputMappingRT;
}

const std::vector< OutputMappingInfoRT>& AxisProfileMappingHandler::getOutputRealtimeInfo() const
{
  return m_outputMappingRT;
}

AxisProfileResult AxisProfileMappingHandler::determineInputAssigned()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  m_profileAssignedInfo.m_input = ProfileInputAssignedInfo {false};

  for (auto const& realTimeInfo : m_inputMappingRT)
  {
    switch (realTimeInfo.m_id)
    {
      case InputMappingValueID::STATUSWORD:
        m_profileAssignedInfo.m_input.m_statusWord = true;
        break;
      case InputMappingValueID::POSITION:
        m_profileAssignedInfo.m_input.m_position = true;
        break;
      case InputMappingValueID::VELOCITY:
        m_profileAssignedInfo.m_input.m_velocity = true;
        break;
      case InputMappingValueID::DIAGMSG:
        m_profileAssignedInfo.m_input.m_diagcode = true;
        break;
      case InputMappingValueID::SWITCH_REF:
        m_profileAssignedInfo.m_input.m_switchRef = true;
        break;
      case InputMappingValueID::SWITCH_POS_LIM:
        m_profileAssignedInfo.m_input.m_switchPosLim = true;
        break;
      case InputMappingValueID::SWITCH_NEG_LIM:
        m_profileAssignedInfo.m_input.m_switchNegLim = true;
        break;
      case InputMappingValueID::PROBE1_POSEDG:
        m_profileAssignedInfo.m_input.m_probe1PosEdg = true;
        break;
      case InputMappingValueID::PROBE1_NEGEDG:
        m_profileAssignedInfo.m_input.m_probe1NegEdg = true;
        break;
      case InputMappingValueID::PROBE1_ACTIVE:
        m_profileAssignedInfo.m_input.m_probe1Active = true;
        break;
      case InputMappingValueID::PROBE1_POSLAT:
        m_profileAssignedInfo.m_input.m_probe1PosLat = true;
        break;
      case InputMappingValueID::PROBE1_NEGLAT:
        m_profileAssignedInfo.m_input.m_probe1NegLat = true;
        break;
      case InputMappingValueID::PROBE1_POSDIF:
        m_profileAssignedInfo.m_input.m_probe1PosDif = true;
        break;
      case InputMappingValueID::PROBE1_MAKFAL:
        m_profileAssignedInfo.m_input.m_probe1MakFal = true;
        break;
      case InputMappingValueID::PROBE2_POSEDG:
        m_profileAssignedInfo.m_input.m_probe2PosEdg = true;
        break;
      case InputMappingValueID::PROBE2_NEGEDG:
        m_profileAssignedInfo.m_input.m_probe2NegEdg = true;
        break;
      case InputMappingValueID::PROBE2_ACTIVE:
        m_profileAssignedInfo.m_input.m_probe2Active = true;
        break;
      case InputMappingValueID::PROBE2_POSLAT:
        m_profileAssignedInfo.m_input.m_probe2PosLat = true;
        break;
      case InputMappingValueID::PROBE2_NEGLAT:
        m_profileAssignedInfo.m_input.m_probe2NegLat = true;
        break;
      case InputMappingValueID::PROBE2_POSDIF:
        m_profileAssignedInfo.m_input.m_probe2PosDif = true;
        break;
      case InputMappingValueID::PROBE2_MAKFAL:
        m_profileAssignedInfo.m_input.m_probe2MakFal = true;
        break;
      case InputMappingValueID::TORQUE_FORCE:
        m_profileAssignedInfo.m_input.m_torqueForce = true;
        break;
      case InputMappingValueID::TORQUE_LIMIT_STATUS:
        m_profileAssignedInfo.m_input.m_torqueForceLimSts = true;
        break;
      case InputMappingValueID::OP_MODE_DISPLAY:
        m_profileAssignedInfo.m_input.m_opModeDisplay = true;
        break;
      case InputMappingValueID::SMO_SAFETY_SELECTION:
        m_profileAssignedInfo.m_input.m_safetySelection = true;
        break;
      case InputMappingValueID::SMO_SAFETY_STATUS:
        m_profileAssignedInfo.m_input.m_safetyStatus = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_1:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[0] = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_2:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[1] = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_3:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[2] = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_4:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[3] = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_5:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[4] = true;
        break;
      case InputMappingValueID::USER_PDO_INPUT_6:
        m_profileAssignedInfo.m_input.m_userPDOs.m_userPDO[5] = true;
        break;
      case InputMappingValueID::SIGNAL_STATUS_WORD:
        m_profileAssignedInfo.m_input.m_signalStatusWord = true;
        break;
      case InputMappingValueID::IO_IN_REFERENCE:
        m_profileAssignedInfo.m_input.m_isInReference = true;
        break;
      case InputMappingValueID::HOMED_STATUS:
        m_profileAssignedInfo.m_input.m_homed = true;
        break;
      case InputMappingValueID::UNKNOWN:
      default:
        result = AxisProfileResult::ERR_WRONGDATATYPE;
        break;
    }
  }
  return result;
}

AxisProfileResult AxisProfileMappingHandler::determineOutputAssigned()
{
  AxisProfileResult result = AxisProfileResult::STS_OK;
  m_profileAssignedInfo.m_output = ProfileOutputAssignedInfo {false};

  for (auto const& realTimeInfo : m_outputMappingRT)
  {
    switch (realTimeInfo.m_id)
    {
      case OutputMappingValueID::CONTROLWORD:
        m_profileAssignedInfo.m_output.m_controlWord = true;
        break;
      case OutputMappingValueID::POSITION:
        m_profileAssignedInfo.m_output.m_position = true;
        break;
      case OutputMappingValueID::VELOCITY:
        m_profileAssignedInfo.m_output.m_velocity = true;
        break;
      case OutputMappingValueID::TORQUE_FORCE:
        m_profileAssignedInfo.m_output.m_torqueForce = true;
        break;
      case OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH:
        m_profileAssignedInfo.m_output.m_torqueVelLimHigh = true;
        break;
      case OutputMappingValueID::TORQUE_VEL_LIMIT_LOW:
        m_profileAssignedInfo.m_output.m_torqueVelLimLow = true;
        break;
      case OutputMappingValueID::OP_MODE:
        m_profileAssignedInfo.m_output.m_opMode = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_1:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[0] = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_2:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[1] = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_3:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[2] = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_4:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[3] = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_5:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[4] = true;
        break;
      case OutputMappingValueID::USER_PDO_OUTPUT_6:
        m_profileAssignedInfo.m_output.m_userPDOs.m_userPDO[5] = true;
        break;
      case OutputMappingValueID::IO_SET_REFERENCE:
        m_profileAssignedInfo.m_output.m_setReference = true;
        break;
      case OutputMappingValueID::IO_REFERENCE_OFFSET:
        m_profileAssignedInfo.m_output.m_referenceOffset = true;
        break;
      case OutputMappingValueID::UNKNOWN:
      default:
        result = AxisProfileResult::ERR_WRONGDATATYPE;
        break;
    }
  }
  return result;
}

template <typename T_ValueID, typename T_MappingValue, typename T_Map>
AxisProfileResult AxisProfileMappingHandler::mappingConfig(T_ValueID valueID, T_MappingValue mappingValue, T_Map& mappingMap)
{
  if ((valueID == T_ValueID::UNKNOWN))
  {
    APR_TRACE_ERROR(m_profileTrace->createTraceMsg(AxisProfileResult::ERR_INVALID_ARGUMENT, "ValueID is 'Unknown'"));
    return AxisProfileResult::ERR_INVALID_ARGUMENT;
  }

  auto mappingItem = mappingMap.find(valueID);
  if (mappingItem != mappingMap.end())
  {
    mappingItem->second = mappingValue;
  }
  else
  {
    mappingMap[valueID] = mappingValue;
  }

  return AxisProfileResult::STS_OK;
}

const ProfileAssignedInfo& AxisProfileMappingHandler::getProfileAssignedInfo() const
{
  return m_profileAssignedInfo;
}

void AxisProfileMappingHandler::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}
