/// @file
///
/// @brief This file is part of the axisprofile library and implements the Sercos
/// boot parameter snapshot reader.
///
/// SPDX-FileCopyrightText: Copyright (c) 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "sercos_boot_param_snapshot.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "comm/ethercat/master/fbs/parameter_generated.h"
#include "common/debug_trace.h"
#include "diagnosis/dia.h"
#include "profiles/category/drive/axisprofile_device_param_drive.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/sercos/sercos_boot_params.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;
using namespace comm::axisprofile::sercos;

// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

SercosBootParamSnapshot::SercosBootParamSnapshot(ProfileBasicInfo& profileBasicInfo)
  : BootParamSnapshotBase(profileBasicInfo, BootSnapshotProtocol::SERCOS)
{
}

AxisProfileResult SercosBootParamSnapshot::getParameter(std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen, bool forceReadFromeDrive,
                                                       comm::ethercat::master::fbs::ElementFlags elementFlag)
{
  m_elementFlag = elementFlag;
  return getParameterImpl(BootParamId(idn), bufPtr, bufLen, forceReadFromeDrive);
}

AxisProfileResult SercosBootParamSnapshot::getParameterValueFromSnapshot(const comm::axisprofile::IAxisProfileSharingHandler* sharingHandler,
                                                                         const char* traceMsg, std::uint16_t idn, std::uint8_t* bufPtr, std::size_t bufLen,
                                                                         comm::ethercat::master::fbs::ElementFlags elementFlag)
{
  if (sharingHandler == nullptr)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  auto* snapshot = static_cast<SercosBootParamSnapshot*>(sharingHandler->getBootParamSnapshotIF());
  auto* devParamIF = static_cast<AxisProfileDeviceParamDrive*>(sharingHandler->getDeviceParamIF());
  if (snapshot == nullptr || devParamIF == nullptr)
  {
    return AxisProfileResult::ERR_EXEC_NULL;
  }

  APR_TRACE_MESSAGE("%s, begin read SoE parameter: [%s] from paramSnapshot.",
                    (traceMsg != nullptr) ? traceMsg : "",
                    idnToString(idn).c_str());
  return snapshot->getParameter(idn, bufPtr, bufLen, devParamIF->isNeedReadBootParams(), elementFlag);
}

std::unique_ptr<ParamReqDataBase> SercosBootParamSnapshot::generateReqData(const BootParamId& paramId) const
{
  const auto* idn = paramId.getIdn();
  if (idn == nullptr)
  {
    return nullptr;
  }

  return std::make_unique<ParamReqDataSoE>(m_profileBasicInfo.m_deviceAddress, *idn, m_elementFlag, m_profileBasicInfo.m_driveNumber);
}

const BootParamSnapshotBase::BootParamDescriptorBase* SercosBootParamSnapshot::getBootParamDescriptor(const BootParamId& paramId) const
{
  const auto* idn = paramId.getIdn();
  if (idn == nullptr)
  {
    return nullptr;
  }

  const BootParamDescriptor* descriptor = getSercosBootParamDescriptor(*idn);
  if (descriptor == nullptr)
  {
    return nullptr;
  }

  m_descriptorCache.m_type = descriptor->type;
  m_descriptorCache.m_valueLen = getBootParamDataLength(descriptor->type);
  return &m_descriptorCache;
}

std::string SercosBootParamSnapshot::parameterToString(const BootParamId& paramId) const
{
  const auto* idn = paramId.getIdn();
  return idn ? idnToString(*idn) : std::string("invalid-parameter-id");
}
