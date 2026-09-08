#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements mockup class of category.
///
/// SPDX-FileCopyrightText: Copyright (c) 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/category/i_category_adapter.h"
#include "profiles/data_sharing/i_sharing_handler.h"

#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_probe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_axisprofile_property.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
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
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------
  /// @brief This is mockup class of axisprofile category object
  class MockCategory : public ::comm::axisprofile::ICategoryAdapter
  {
  public://method
    /// @brief Constructor
    MockCategory()
    {
      m_cmdMock = new test::MockAxisProfileCommand();
      m_commandInterface = m_cmdMock;
      m_statusControlMock = new test::MockAxisProfileStatusControl();
      m_statusControlInterface = m_statusControlMock;
      m_deviceParamMock = new test::MockAxisProfileDeviceParam();
      m_deviceParamInterface = m_deviceParamMock;
      m_probeMock = new test::MockAxisProfileProbe();
      m_probeInterface = m_probeMock;
      m_cyclicdataMock = new test::MockAxisProfileCyclicData();
      m_cyclicDataInterface = m_cyclicdataMock;
      m_scalingMock = new test::MockScaling();
      m_scaling = m_scalingMock;
    }

    virtual ~MockCategory() noexcept override
    {
      delete m_cmdMock; m_cmdMock = nullptr;
      delete m_statusControlMock; m_statusControlMock = nullptr;
      delete m_deviceParamMock; m_deviceParamMock = nullptr;
      delete m_probeMock; m_probeMock = nullptr;
      delete m_cyclicdataMock; m_cyclicdataMock = nullptr;
      delete m_scalingMock; m_scalingMock = nullptr;
    }

    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* handler) final
    {
      m_sharingHandler = handler;
    }

    comm::axisprofile::IAxisProfileCommandInternal* getCommandInternal() final
    {
      return m_commandInterface;
    }

    comm::axisprofile::IAxisProfileStatusControlInternal* getStatusControlInternal()final
    {
      return m_statusControlInterface;
    }

    comm::axisprofile::IAxisProfileCyclicDataInternal* getCyclicDataInternal() final
    {
      return m_cyclicDataInterface;
    }

    comm::axisprofile::IAxisProfileDeviceParamInternal* getDeviceParamInternal() final
    {
      return m_deviceParamInterface;
    }

    comm::axisprofile::IAxisProfileProbeInternal* getProbeInternal() final
    {
      return m_probeInterface;
    }

    comm::axisprofile::IScaling* getScalingInternal() final
    {
      return m_scaling;
    }

    comm::axisprofile::AxisProfileResult updateScalingStrategy(const comm::axisprofile::AxisProfileScalingStrategy& /*strategy*/)
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult setStatemachine(const comm::axisprofile::StateMachineState* const statemachine)
    {
      m_cyclicDataInterface->setStateMachineStatusIF(statemachine);
      return m_expReturn;
    }

    const comm::axisprofile::StateMachineState*& getStatemachine()
    {
      return m_cyclicDataInterface->getStatemachine();
    }

    void loadInitMethod(std::vector<std::function<AxisProfileResult()>>& methods) final
    {
      methods.clear();
      methods.reserve(5);
      methods.emplace_back(std::bind(&IScaling::booting, m_scaling));
      methods.emplace_back(std::bind(&IAxisProfileCommandInternal::booting, m_commandInterface));
      methods.emplace_back(std::bind(&IAxisProfileDeviceParamInternal::booting, m_deviceParamInterface));
      methods.emplace_back(std::bind(&IAxisProfileStatusControlInternal::booting, m_statusControlInterface));
      methods.emplace_back(std::bind(&IAxisProfileProbeInternal::booting, m_probeInterface));
    }

    void registerSharingITFs(comm::axisprofile::IAxisProfileSharingHandler* handler) final
    {
      m_sharingHandler = handler;
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_SCALING, m_scaling);
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_CYCLICDATA, m_cyclicDataInterface);
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_DEVICEPARAM, m_deviceParamInterface);
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_COMMAND, m_commandInterface);
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_STATUSCONTROL, m_statusControlInterface);
      m_sharingHandler->registSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_PROBE, m_probeInterface);
    }

    void removeSharingITFs() final
    {
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_COMMAND);
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_STATUSCONTROL);
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_CYCLICDATA);
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_DEVICEPARAM);
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_PROBE);
      m_sharingHandler->removeSharingITF(comm::axisprofile::SharingInterfaceType::ITF_TYPE_SCALING);
    }

    comm::axisprofile::AxisProfileCategory getProfileCategory() const {return m_profileCategory;}   

  public: // data
    comm::axisprofile::IScaling* m_scaling {};                                           ///< Scaling class used for this profile
    comm::axisprofile::IAxisProfileCommandInternal* m_commandInterface {};               ///< Pointer to the internal command interface
    comm::axisprofile::IAxisProfileStatusControlInternal* m_statusControlInterface {};   ///< Pointer to the internal status- and controlword interface
    comm::axisprofile::IAxisProfileCyclicDataInternal* m_cyclicDataInterface {};         ///< Pointer to the internal cyclic data interface
    comm::axisprofile::IAxisProfileDeviceParamInternal* m_deviceParamInterface {};       ///< Pointer to the internal device parameter interface
    comm::axisprofile::IAxisProfileProbeInternal* m_probeInterface {};                   ///< Pointer to the internal probe interface
    comm::axisprofile::AxisProfileCategory m_profileCategory {comm::axisprofile::AxisProfileCategory::UNDEFINED}; ///< profile category
    comm::axisprofile::IAxisProfileSharingHandler* m_sharingHandler {};                                           ///< Pointer to the sharing handler
    comm::axisprofile::AxisProfileResult m_expReturn {comm::axisprofile::AxisProfileResult::STS_OK};              ///< the expected return result
    bool m_expBoolRes {true};                                                                                     ///< the expected bool return result
    
    test::MockAxisProfileCommand* m_cmdMock {};                   //!< Mockup of the command interface used in test
    test::MockAxisProfileStatusControl* m_statusControlMock {};   //!< Mockup of the status control interface used in test
    test::MockAxisProfileDeviceParam* m_deviceParamMock {};      //!< Mockup of the booting config interface used in test 
    test::MockAxisProfileProbe* m_probeMock {};                   //!< Mockup of the probe interface used in test
    test::MockAxisProfileCyclicData* m_cyclicdataMock {};
    test::MockScaling* m_scalingMock {};
  };
}
