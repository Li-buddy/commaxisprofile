/// @file
///
/// @brief This source file is part of the axisprofile library and implement the CategoryAdapter.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "category_adapter.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace comm::axisprofile;

CategoryAdapter::CategoryAdapter(const ProfileBasicInfo& profileBasicInfo, const AcknowledgeTelegram* at,
                                 MasterDataTelegram* mdt, AxisProfileCategory category)
  :m_profileBasicInfo(profileBasicInfo), m_telegramAT(at), m_telegramMDT(mdt), m_profileCategory(category)
{
}

CategoryAdapter::~CategoryAdapter()
{
  delete m_commandInterface; m_commandInterface = nullptr;
  delete m_statusControlInterface; m_statusControlInterface = nullptr;
  delete m_cyclicDataInterface; m_cyclicDataInterface = nullptr;
  delete m_deviceParamInterface; m_deviceParamInterface = nullptr;
  delete m_scaling; m_scaling = nullptr;
  delete m_probeInterface; m_probeInterface = nullptr;
}

AxisProfileCategory CategoryAdapter::getProfileCategory() const
{
  return m_profileCategory;
}

void CategoryAdapter::setSharingHandler(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
}

IAxisProfileCommandInternal* CategoryAdapter::getCommandInternal()
{
  return m_commandInterface;
}

IAxisProfileStatusControlInternal* CategoryAdapter::getStatusControlInternal()
{
  return m_statusControlInterface;
}

IAxisProfileCyclicDataInternal* CategoryAdapter::getCyclicDataInternal()
{
  return m_cyclicDataInterface;
}

IAxisProfileDeviceParamInternal* CategoryAdapter::getDeviceParamInternal()
{
  return m_deviceParamInterface;
}

IAxisProfileProbeInternal* CategoryAdapter::getProbeInternal()
{
  return m_probeInterface;
}

IScaling* CategoryAdapter::getScalingInternal()
{
  return m_scaling;
} 

void CategoryAdapter::registerSharingITFs(IAxisProfileSharingHandler* handler)
{
  m_sharingHandler = handler;
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_SCALING, m_scaling);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA, m_cyclicDataInterface);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, m_deviceParamInterface);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, m_commandInterface);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL, m_statusControlInterface);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROBE, m_probeInterface);
}

void CategoryAdapter::removeSharingITFs()
{
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND);
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_STATUSCONTROL);
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_CYCLICDATA);
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM);
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_PROBE);
  m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_SCALING);
}

AxisProfileResult CategoryAdapter::setStatemachine(const StateMachineState* const statemachine)
{
  return m_cyclicDataInterface->setStateMachineStatusIF(statemachine);
}

const StateMachineState*& CategoryAdapter::getStatemachine()
{
  return m_cyclicDataInterface->getStatemachine();
}

void CategoryAdapter::loadInitMethod(std::vector<std::function<AxisProfileResult()>>& methods)
{
  methods.clear();
  methods.reserve(6);
  methods.emplace_back([this]{return m_deviceParamInterface->booting();});
  methods.emplace_back([this]{return m_scaling->booting();});
  methods.emplace_back([this]{return m_commandInterface->booting();});
  methods.emplace_back([this]{return m_statusControlInterface->booting();});
  methods.emplace_back([this]{return m_probeInterface->booting();});
  methods.emplace_back([this]{return m_cyclicDataInterface->booting();});
}
