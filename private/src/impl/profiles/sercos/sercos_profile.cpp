/// @file
///
/// @brief This file is part of the axisprofile library and implements the
/// main class for the SERCOS profile.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile_internal_defs.h"
#include "cmd_jobs/sercos_job_device_param.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "common/axisprofile_fpu_helper.h"
#include "helper/datalayer_helper.h"
#include "profiles/axisprofile.h"
#include "profiles/axisprofile_diag_msg.h"
#include "profiles/category/drive/drive_category_adapter_soe.h"
#include "profiles/profile_defs.h"
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/sercos/sercos_defs.h"
#include "profiles/sercos/sercos_boot_param_snapshot.h"
#include "sercos_cmd.h"
#include "sercos_param_access.h"
#include "sercos_profile.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

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

SercosProfile::SercosProfile(const std::string& profileName, const NetworkTechnology technology) :
  AxisProfile(profileName, AxisProfileType::SERVODRIVE_OVER_ETHERCAT, DriveProfileType::SERCOS, technology)
{
  // create parameter access class
  m_paramAccess = new SercosParamAccess(m_profileBasicInfo);

  // Create SercosCmd
  m_cmdIf = new SercosCmd(m_profileBasicInfo);

  // Create boot parameter snapshot interface
  m_bootParamSnapshotInterface = new SercosBootParamSnapshot(m_profileBasicInfo);

  // create profile category object
  m_profileCategory = new DriveCategoryAdapterSoE(m_profileBasicInfo, &m_telegramAT, &m_telegramMDT);

  // Create sharing handler and register interfaces
  m_profileCategory->registerSharingITFs(m_sharingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY,          m_profileCategory);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PARAMACCESS,       m_paramAccess);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CMD,               m_cmdIf);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_BOOTPARAMSNAPSHOT, m_bootParamSnapshotInterface);

  // add init methods
  loadInitMethod();
}

AxisProfileResult SercosProfile::init()
{
  // Initialize the status word with drive enabled
  m_telegramAT.m_statusWord.m_word = (static_cast<std::uint16_t>(ReadyToOperate::DRIVE_ENABLED) << SERCOS_STSWORD_RDY_TO_OPERATE_OFFSET);
  m_telegramAT.m_diagMsg.m_word = static_cast<std::uint32_t>(0U);

  return AxisProfile::init();
}

AxisProfileResult SercosProfile::processDataAt(std::uint8_t* dataByteArray)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  IAxisProfileMappingHandler* const handler = getMappingHandler();

  const auto& realTimeInfos = handler->getInputRealtimeInfo();
  std::uint16_t dummy{0};
  for ( auto const &realTimeInfo : realTimeInfos )
  {
    std::uint8_t* data{reinterpret_cast<std::uint8_t*>(&dummy)};
    switch ( realTimeInfo.m_id )
    {
    case InputMappingValueID::STATUSWORD:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_statusWord.m_word));
      break;
    case InputMappingValueID::POSITION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualPosition));
      break;
    case InputMappingValueID::VELOCITY:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualVelocity));
      break;
    case InputMappingValueID::DIAGMSG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_diagMsg.m_word));
      break;
    case InputMappingValueID::SWITCH_REF:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitRefSwitch.m_byte));
      break;
    case InputMappingValueID::SWITCH_POS_LIM:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitPosLimSwitch.m_byte));
      break;
    case InputMappingValueID::SWITCH_NEG_LIM:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_digitalInput.m_bitNegLimSwitch.m_byte));
      break;
    case InputMappingValueID::PROBE1_POSEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.posEdg));
      break;
    case InputMappingValueID::PROBE1_NEGEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.negEdg));
      break;
    case InputMappingValueID::PROBE1_ACTIVE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.probeActive));
      break;
    case InputMappingValueID::PROBE1_POSLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.posLatched));
      break;
    case InputMappingValueID::PROBE1_NEGLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.negLatched));
      break;
    case InputMappingValueID::PROBE1_POSDIF:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.positionDif));
      break;
    case InputMappingValueID::PROBE1_MAKFAL:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe1.markFailure));
      break;
    case InputMappingValueID::PROBE2_POSEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.posEdg));
      break;
    case InputMappingValueID::PROBE2_NEGEDG:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.negEdg));
      break;
    case InputMappingValueID::PROBE2_ACTIVE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.probeActive));
      break;
    case InputMappingValueID::PROBE2_POSLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.posLatched));
      break;
    case InputMappingValueID::PROBE2_NEGLAT:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.negLatched));
      break;
    case InputMappingValueID::PROBE2_POSDIF:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.positionDif));
      break;
    case InputMappingValueID::PROBE2_MAKFAL:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_probe.m_probe2.markFailure));
      break;
    case InputMappingValueID::TORQUE_FORCE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualTorqueInfo.m_actualTorqueForce));
      break;
    case InputMappingValueID::TORQUE_LIMIT_STATUS:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_atData.m_actualTorqueInfo.m_torqueLimitStatus.m_word));
      break;
    case InputMappingValueID::SMO_SAFETY_SELECTION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_smoData.m_safetySelection));
      break;
    case InputMappingValueID::SMO_SAFETY_STATUS:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_smoData.m_safetyStatus));
      break;
    case InputMappingValueID::USER_PDO_INPUT_1:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[0]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_2:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[1]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_3:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[2]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_4:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[3]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_5:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[4]));
      break;
    case InputMappingValueID::USER_PDO_INPUT_6:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramAT.m_userPDOInputs[5]));
      break;
    case InputMappingValueID::SIGNAL_STATUS_WORD:
      data = reinterpret_cast<uint8_t*>(&(m_telegramAT.m_signalStatusword));
      break;
    default:
      result = AxisProfileResult::ERR_WRONGDATATYPE;
      break;
    }
    DatalayerHelper::copyBits(data, 0, dataByteArray, realTimeInfo.m_bitOffset, realTimeInfo.m_bitSize);
  }

  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_BEFORE_AXISPROFILE,
                        m_profileBasicInfo.m_profileName.c_str(), "INPUT data analysis");

  // Interpret the new AT data
  m_profileCategory->getStatusControlInternal()->analyzeStatusWord();

  const ProfileAssignedInfo profileAssignedInfo  = m_mappingHandler->getProfileAssignedInfo();
  if (profileAssignedInfo.m_input.m_diagcode)
  {
    static_cast<AxisProfileDiagMsg*>(m_diagMsgInterface)->analyzeDiagMsg();
  }

  APR_CHECK_FPU_ERR_PROFILE(MAIN_DIAG_SYSTEM_ERR, DETAILED_DIAG_FPU_ERR_DURING_AXISPROFILE,
                        m_profileBasicInfo.m_profileName.c_str(), "INPUT data analysis");
  return result;
}

AxisProfileResult SercosProfile::processDataMdt(std::uint8_t* dataByteArray)
{
  AxisProfileResult result = AxisProfileResult::STS_OK;

  IAxisProfileMappingHandler* const handler = getMappingHandler();
  const auto& realTimeInfos = handler->getOutputRealtimeInfo();
  std::uint16_t dummy{0};
  for ( auto const &realTimeInfo : realTimeInfos )
  {
    std::uint8_t* data{reinterpret_cast<std::uint8_t*>(&dummy)};
    switch ( realTimeInfo.m_id )
    {
    case OutputMappingValueID::CONTROLWORD:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_controlWord));
      break;
    case OutputMappingValueID::POSITION:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdPosition));
      break;
    case OutputMappingValueID::VELOCITY:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdVelocity));
      break;
    case OutputMappingValueID::TORQUE_FORCE:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_cmdTorqueForce));
      break;
    case OutputMappingValueID::TORQUE_VEL_LIMIT_HIGH:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_velLimitHigh));
      break;
    case OutputMappingValueID::TORQUE_VEL_LIMIT_LOW:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_mdtData.m_cmdTorqueForceInfo.m_velLimitLow));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_1:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[0]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_2:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[1]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_3:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[2]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_4:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[3]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_5:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[4]));
      break;
    case OutputMappingValueID::USER_PDO_OUTPUT_6:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_userPDOOutputs[5]));
      break;
    case OutputMappingValueID::SIGNAL_CONTROL_WORD:
      data = reinterpret_cast<std::uint8_t*>(&(m_telegramMDT.m_signalControlword));
      break;
    default:
      result = AxisProfileResult::ERR_WRONGDATATYPE;
      break;
    }
    DatalayerHelper::copyBits(dataByteArray, realTimeInfo.m_bitOffset, data, 0, realTimeInfo.m_bitSize);
  }

  return result;
}

AxisProfileResult SercosProfile::getDeviceState()
{
  return AxisProfileResult::ERR_NOTIMPLEMENTED;
}

AxisProfileScalingStrategyCfg SercosProfile::getProfileScalingStrategyCfg() const
{
  ScalingStrategy* scaling = static_cast<ScalingStrategy*>(m_profileCategory->getScalingInternal());
  return AxisProfileScalingStrategyCfg{scaling->getScalingStrategy(), getDeviceScalingType(AxisProfileDeviceValueType::POSITION),
    scaling->getScalingNumerator(), scaling->getScalingDenominator(), scaling->getEncoderResolution(), scaling->getVelocityFactorCfg()};
}

std::vector<std::unique_ptr<IJobObj>> SercosProfile::onGetJobList()
{
  std::vector<std::unique_ptr<IJobObj>> jobs;
  jobs.reserve(6);

  jobs.emplace_back(std::make_unique<SercosJobReset>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<SercosJobHoming>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<SercosJobSetAbsPos>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<SercosJobFixedStop>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<SercosJobSetOpMode>(m_profileBasicInfo, m_sharingHandler));
  jobs.emplace_back(std::make_unique<SercosJobDeviceParam>(m_profileBasicInfo, m_sharingHandler));
  return jobs;
}
