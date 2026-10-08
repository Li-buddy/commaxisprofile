/// @file
///
/// This source file is part of the axisprofile library and implements
/// base class of all axis profiles.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "axisprofile.h"
#include "axisprofile_property.h"
#include "bundle/axisprofile_c.h"
#include "common/axisprofile_fpu_helper.h"
#include "fieldbuses/fieldbus_adapter_factory.h"
#include "job_mgr/job_mgr.h"
#include "profiles/aux_job/aux_job_handler.h"
#include "profiles/axisprofile_diag_msg.h"
#include "profiles/category/category_adapter_factory.h"
#include "profiles/data_sharing/sharing_handler.h"
#include "value_mapping/mapping_handler.h"
#include "profiles/i_device_boot_param_snapshot.h"

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

AxisProfile::AxisProfile(const std::string& profileName, const AxisProfileType axisProfileType, const DriveProfileType driveProfileType, const NetworkTechnology networkTechnology)
  :m_driveProfileType(driveProfileType), m_networkTechnology(networkTechnology)
{
  // initial basic information
  m_profileBasicInfo.m_axisProfileType = axisProfileType,
  m_profileBasicInfo.m_profileName = profileName;
  m_profileBasicInfo.m_profileTrace.setConstantProfileData(m_profileBasicInfo.m_profileName, m_profileBasicInfo.m_axisProfileType);

  // create fieldbus adapter
  FieldbusAdapterFactory::createFieldbusAdapter(m_profileBasicInfo, &m_profileBasicInfo.m_fieldbusAdapter);

  // create share handler
  m_sharingHandler = new AxisProfileSharingHandler(&m_profileBasicInfo.m_profileTrace);

  // create job manager
  m_jobMgr = new JobMgr(m_profileBasicInfo);

  // create mapping handler object
  m_mappingHandler = new AxisProfileMappingHandler(m_profileBasicInfo);

  // Create diagnostic message class
  m_diagMsgInterface = new AxisProfileDiagMsg(&m_profileBasicInfo.m_profileTrace, &m_telegramAT);

  // Create auxiliary job handler class
  m_auxJobHandler = new AuxJobHandler();

  //Create property interface
  m_propertyInterface = new AxisProfileProperty(m_profileBasicInfo);

  // register basic interfaces, some interfaces will be registered in the constructor of the derived classes
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_JOB_MGR, m_jobMgr);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, m_mappingHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_DIAGMSG, m_diagMsgInterface);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, m_auxJobHandler);
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_PROPERTY, m_propertyInterface);
}

AxisProfile::~AxisProfile() noexcept
{
  delete m_diagMsgInterface; m_diagMsgInterface = nullptr;
  delete m_mappingHandler; m_mappingHandler = nullptr;
  delete m_profileBasicInfo.m_fieldbusAdapter; m_profileBasicInfo.m_fieldbusAdapter = nullptr;
  delete m_sharingHandler; m_sharingHandler = nullptr;
  delete m_auxJobHandler; m_auxJobHandler = nullptr;
  delete m_paramAccess; m_paramAccess = nullptr;
  delete m_profileCategory; m_profileCategory = nullptr;
  delete m_cmdIf; m_cmdIf = nullptr;
  delete m_propertyInterface; m_propertyInterface = nullptr;
  delete m_bootParamSnapshotInterface; m_bootParamSnapshotInterface = nullptr;
  delete m_jobMgr; m_jobMgr = nullptr;
}

void AxisProfile::loadInitMethod()
{
  // the category init method
  m_profileCategory->loadInitMethod(m_initMethods);
  m_initMethods.insert(m_initMethods.begin(), [&](){return registerAllJobs();});
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------- External Interface ------------------------------------------- //

const IAxisProfileCommandC* AxisProfile::getCommandInterfaceC() const
{
  return m_profileCategory->getCommandInternal()->getCInterface();
}

const IAxisProfileStatusControlC* AxisProfile::getStatusControlInterfaceC() const
{
  return m_profileCategory->getStatusControlInternal()->getCInterface();
}

const IAxisProfileDiagMsgC* AxisProfile::getDiagMsgInterfaceC() const
{
  return m_diagMsgInterface->getCInterface();
}

const IAxisProfileCyclicDataC* AxisProfile::getCyclicDataInterfaceC() const
{
  return m_profileCategory->getCyclicDataInternal()->getCInterface();
}

const IAxisProfileBootingConfigC* AxisProfile::getBootingConfigInterfaceC() const
{
  return m_profileCategory->getDeviceParamInternal()->getDeprecatedCInterface();
}

const IAxisProfileDeviceParamC* AxisProfile::getDeviceParamInterfaceC() const
{
  return m_profileCategory->getDeviceParamInternal()->getCInterface();
}

const IAxisProfileProbeC* AxisProfile::getProbeInterfaceC() const
{
  return m_profileCategory->getProbeInternal()->getCInterface();
}

const IAxisProfileCmdC* AxisProfile::getCmdInterfaceC() const
{
  return m_cmdIf->getCInterface();
}

const IAxisProfilePropertyC* AxisProfile::getPropertyInterfaceC() const
{
  return m_propertyInterface->getCInterface();
}

AxisProfileResult AxisProfile::init()
{
  return AxisProfileResult::STS_OK;
}

AxisProfileResult AxisProfile::setDeviceAddress(std::uint32_t deviceAddress)
{
  m_profileBasicInfo.m_deviceAddress = deviceAddress;
  m_profileBasicInfo.m_profileTrace.updateDeviceAddress(m_profileBasicInfo.m_deviceAddress);
  return AxisProfileResult::STS_OK;
}

std::uint32_t AxisProfile::getDeviceAddress() const
{
  return m_profileBasicInfo.m_deviceAddress;
}

AxisProfileType AxisProfile::getProfileType() const
{
  return m_profileBasicInfo.m_axisProfileType;
}

AxisProfileDeviceScalingType AxisProfile::getDeviceScalingType(AxisProfileDeviceValueType valueType) const
{
  if (!m_profileCategory->getScalingInternal())
  {
    return AxisProfileDeviceScalingType::UNDEFINED;
  }
  return m_profileCategory->getScalingInternal()->getDeviceScalingType(valueType);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------- Internal Interface ------------------------------------------- //

const IAxisProfileC* AxisProfile::createCInterface()
{
  // *** Create and initialize C-Interface *** //
  IAxisProfileC* newItfInstance = new IAxisProfileC();
  newItfInstance->handle = this;
  newItfInstance->getCommandInterface = AXISPROFILE_getCommandInterfaceC;
  newItfInstance->getStatusControlInterface = AXISPROFILE_getStatusControlInterfaceC;
  newItfInstance->getDiagMsgInterface = AXISPROFILE_getDiagMsgInterfaceC;
  newItfInstance->getCyclicDataInterface = AXISPROFILE_getCyclicDataInterfaceC;
  newItfInstance->getBootingConfigInterface = AXISPROFILE_getBootingConfigInterfaceC;
  newItfInstance->getProbeInterface = AXISPROFILE_getProbeInterfaceC;
  newItfInstance->getCmdInterface = AXISPROFILE_getCmdInterfaceC;
  newItfInstance->getDeviceParamInterface = AXISPROFILE_getDeviceParamInterfaceC;
  newItfInstance->getPropertyInterface = AXISPROFILE_getPropertyInterfaceC;
  return newItfInstance;
}


IAxisProfileCommandInternal* AxisProfile::getCommandInterface()
{
  return m_profileCategory->getCommandInternal();
}

IAxisProfileStatusControlInternal* AxisProfile::getStatusControlInterface()
{
  return m_profileCategory->getStatusControlInternal();
}

IAxisProfileDiagMsgInternal* AxisProfile::getDiagMsgInterface()
{
  return m_diagMsgInterface;
}

IAxisProfileCyclicDataInternal* AxisProfile::getCyclicDataInterface()
{
  return m_profileCategory->getCyclicDataInternal();
}

IAxisProfileDeviceParamInternal* AxisProfile::getDeviceParamInterface()
{
  return m_profileCategory->getDeviceParamInternal();
}

IAxisProfileProbeInternal* AxisProfile::getProbeInterface()
{
  return m_profileCategory->getProbeInternal();
}

IAxisProfileCmdInternal* AxisProfile::getCmdInterface()
{
  return m_cmdIf;
}

IAxisProfilePropertyInternal* AxisProfile::getPropertyInterface()
{
  return m_propertyInterface;
}

IDeviceBootParamSnapshot* AxisProfile::getDeviceBootParmSnapshotInterface()
{
  return m_bootParamSnapshotInterface;
}

IAxisProfileMappingHandler* AxisProfile::getMappingHandler() const
{
  return m_mappingHandler;
}

IFieldbusAdapter* AxisProfile::getFieldbusAdapter() const
{
  return m_profileBasicInfo.m_fieldbusAdapter;
}

IScaling* AxisProfile::getScaling() const
{
  return m_profileCategory->getScalingInternal();
}

const char* AxisProfile::getProfileName()
{
  return m_profileBasicInfo.m_profileName.c_str();
}

DriveProfileType AxisProfile::getDriveProfileType() const
{
  return m_driveProfileType;
}

NetworkTechnology AxisProfile::getNetworkTechnology() const
{
  return m_networkTechnology;
}

// TODO: remove in next PR
AxisProfileResult AxisProfile::runAllAuxJobs()
{
  return m_sharingHandler->getAuxJobHandlerIF()->runAllAuxJobs();
}

AxisProfileResult AxisProfile::setDeviceName(const char* cDeviceName)
{
  m_strDeviceName = cDeviceName;
  return AxisProfileResult::STS_OK;
}

const char* AxisProfile::getDeviceName() const
{
  return m_strDeviceName.c_str();
}

AxisProfileResult AxisProfile::setProfileCategory(AxisProfileCategory category)
{
  if (m_profileCategory && category == m_profileCategory->getProfileCategory())
  {
    return AxisProfileResult::STS_OK;
  }
  // note: category can't cross different protocol
  ICategoryAdapter* originalCategory = m_profileCategory;
  m_profileCategory = CategoryAdapterFactory::createCategoryAdapter(m_profileBasicInfo, &m_telegramAT, &m_telegramMDT, category);
  // release original usage object memory
  if (originalCategory)
  {
    m_profileCategory->setStatemachine(originalCategory->getStatemachine());

    originalCategory->removeSharingITFs();
    m_sharingHandler->removeSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY);
    delete originalCategory;
    originalCategory = nullptr;
  }

  init();
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_profileCategory);
  m_profileCategory->registerSharingITFs(m_sharingHandler);
  loadInitMethod();

  return AxisProfileResult::STS_OK;
}

AxisProfileCategory AxisProfile::getProfileCategory()
{
  if (!m_profileCategory)
  {
    return AxisProfileCategory::UNDEFINED;
  }

  return m_profileCategory->getProfileCategory();
}

void AxisProfile::resetRtInputData()
{
  m_telegramAT.reset();
  auto diagMsgIf = getDiagMsgInterface();
  if (diagMsgIf)
  {
    diagMsgIf->resetErrCode();
  }
}

AxisProfileResult AxisProfile::bootingReset()
{
  return m_profileCategory->getCommandInternal()->resetAllAxisprofileCommands();
}

void AxisProfile::runJobsInBkThread()
{
  if (m_jobMgr)
  {
    m_jobMgr->runAllJobs();
  }
}

void AxisProfile::setCycleTimeToScaling(double cycleTime)
{
  m_profileCategory->getScalingInternal()->setCycleTime(cycleTime);
}

void AxisProfile::resetFPUFlag()
{
  m_profileCategory->getCyclicDataInternal()->resetFPUFlag();
}

IJobMgr* AxisProfile::getJobMgr()
{
  return m_jobMgr;
}

AxisProfileResult AxisProfile::registerAllJobs()
{
  // clear all exist commands
  m_jobMgr->unregisterAllJobs();

  // register jobs
  auto jobs = onGetJobList();

  AxisProfileResult result {AxisProfileResult::STS_OK};
  for (auto& job : jobs)
  {
    auto res = m_jobMgr->registerJob(job);
    if (res != AxisProfileResult::STS_OK)
    {
      APR_TRACE_ERROR("%s: Job %s register failed, ErrCode=0x%X",
                      m_profileBasicInfo.m_profileName.c_str(), job->getID().c_str(), res);
      result = res;
    }
  }
  return result;
}

void AxisProfile::setProfileActivated(bool activated)
{
  m_profileActivated = activated;
}

bool AxisProfile::checkProfileActivated() const
{
  return m_profileActivated;
}
