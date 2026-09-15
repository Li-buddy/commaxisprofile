#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile.h
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
//! This header file is part of the axisprofile library and declares a mockup
//! of the axisprofile class for internal usage.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/i_axisprofile_internal.h"
#include "profiles/axisprofile.h"
#include <map>
#include "mockup/mock_fieldbus_adapter_soe.h"
#include "mockup/mock_scaling.h"
#include "mockup/mock_profile_trace.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_cyclic_data.h"
#include "mockup/mock_axisprofile_status_control.h"
#include "mockup/mock_axisprofile_diag_msg.h"
#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_probe.h"
#include "mockup/mock_axisprofile_cmd.h"
#include "mockup/mock_job_mgr.h"
#include "mockup/mock_axisprofile_property.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @class MockAxisProfile
  /// Mockup of the axisprofile for internal usage.
  class MockAxisProfile : public comm::axisprofile::IAxisProfileInternal
  {
  public:
    /// @brief Constructor
    /// @param[in]   name   name of the profile
    /// @param[in]   type   type of the profile
    explicit MockAxisProfile(const std::string& name, comm::axisprofile::AxisProfileType type) 
      : m_profileName(name)
    {
      m_mappingHandler = new MockAxisProfileMappingHandler();
      m_fieldbusAdapterMock = new MockFieldbusAdapterSoE();
      m_scalingMock = new MockScaling();
      m_dprPropertyMock.setProfileType(type);
    }
    //! @brief Destructor
    virtual ~MockAxisProfile()
    {
      if (m_mappingHandler) delete m_mappingHandler;
      if (m_fieldbusAdapterMock) delete m_fieldbusAdapterMock;
      if (m_scalingMock) delete m_scalingMock;
    }

    // ----------------------------- Internal Interface ---------------------------------

    const IAxisProfileC* createCInterface() override { return nullptr; }
    comm::axisprofile::AxisProfileResult init() override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult processDataAt(std::uint8_t* /*dataByteArray*/) override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult processDataMdt(std::uint8_t* /*dataByteArray*/) override { return m_returnValue; }
    comm::axisprofile::IAxisProfileMappingHandler* getMappingHandler() const override { return m_mappingHandler; }
    comm::axisprofile::IFieldbusAdapter* getFieldbusAdapter() const override { return m_fieldbusAdapterMock; }
    comm::axisprofile::IScaling* getScaling() const override { return m_scalingMock; }
    const char* getProfileName() override { return m_profileName.c_str(); }
    comm::axisprofile::DriveProfileType getDriveProfileType() const override { return comm::axisprofile::DriveProfileType::UNDEFINED; }
    comm::axisprofile::NetworkTechnology getNetworkTechnology() const override { return comm::axisprofile::NetworkTechnology::UNDEFINED; }
    const std::vector<std::function<comm::axisprofile::AxisProfileResult()>> &getInitMethods() final{return m_initMethods;};
    comm::axisprofile::AxisProfileResult runAllAuxJobs() final{ return m_returnValue;}
    comm::axisprofile::AxisProfileResult setDriveNumber(const std::uint8_t driveNumber) {m_driveNumber = driveNumber; return comm::axisprofile::AxisProfileResult::STS_OK; }
    std::uint8_t getDriveNumber() const { return m_driveNumber; }
    comm::axisprofile::AxisProfileResult setProfileCategory(comm::axisprofile::AxisProfileCategory typeMode) override
    {
      m_profileCategory = typeMode;
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }
    comm::axisprofile::AxisProfileCategory getProfileCategory() override
    {
      return m_profileCategory;
    }
    comm::axisprofile::AxisProfileResult bootingReset() override {return m_returnValue;}
    void resetFPUFlag() override {}
    comm::axisprofile::IJobMgr* getJobMgr() override { return &m_jobMgr; }
    void setProfileActivated(bool activated) override { m_profileActivated = activated; };
    bool checkProfileActivated() const override { return m_profileActivated; };

    // ----------------------------- External Interface ---------------------------------

    comm::axisprofile::AxisProfileResult getDeviceState() override { return m_returnValue; }
    comm::axisprofile::AxisProfileType getProfileType() const { return m_dprPropertyMock.getProfileType()/*m_profileType*/; }
    comm::axisprofile::IAxisProfileCommandInternal* getCommandInterface() override { return &m_dprCommandMock; }
    comm::axisprofile::IAxisProfileStatusControlInternal* getStatusControlInterface() override { return &m_dprStatusControlMock; }
    comm::axisprofile::IAxisProfileDiagMsgInternal* getDiagMsgInterface() override { return &m_dprDiagMsgMock; }
    comm::axisprofile::IAxisProfileCyclicDataInternal* getCyclicDataInterface() override { return &m_dprCyclicDataMock; }
    comm::axisprofile::IAxisProfileDeviceParamInternal* getDeviceParamInterface() override { return &m_dprDeviceParamMock; }
    comm::axisprofile::IAxisProfileProbeInternal* getProbeInterface() override { return &m_dprProbeMock; }
    comm::axisprofile::IAxisProfileCmdInternal* getCmdInterface() override { return &m_dprCmdMock;}
    comm::axisprofile::IAxisProfilePropertyInternal* getPropertyInterface() override { return &m_dprPropertyMock; }
    comm::axisprofile::AxisProfileResult setDeviceAddress(std::uint32_t deviceAddress) override { m_deviceAddress = deviceAddress; return m_returnValue; }
    std::uint32_t getDeviceAddress() const override { return m_deviceAddress; }
    const char* getDeviceName() const override { return m_strDeviceName.c_str(); }
    comm::axisprofile::AxisProfileResult setDeviceName(const char* cDeviceName) override { m_strDeviceName = cDeviceName; return comm::axisprofile::AxisProfileResult::STS_OK; }
    const IAxisProfileCommandC* getCommandInterfaceC() const override { return nullptr; }
    const IAxisProfileStatusControlC* getStatusControlInterfaceC() const override { return nullptr; }
    const IAxisProfileDiagMsgC* getDiagMsgInterfaceC() const override { return nullptr; }
    const IAxisProfileCyclicDataC* getCyclicDataInterfaceC() const override { return nullptr; }
    const IAxisProfileBootingConfigC* getBootingConfigInterfaceC() const override { return nullptr; }
    const IAxisProfileDeviceParamC* getDeviceParamInterfaceC() const override { return nullptr; }
    const IAxisProfileProbeC* getProbeInterfaceC() const override { return nullptr; }
    const IAxisProfileCmdC* getCmdInterfaceC() const override { return nullptr; }
    const IAxisProfilePropertyC* getPropertyInterfaceC() const override { return nullptr; }
    comm::axisprofile::AxisProfileDeviceScalingType getDeviceScalingType(comm::axisprofile::AxisProfileDeviceValueType /*valueType*/) const override { return comm::axisprofile::AxisProfileDeviceScalingType::UNDEFINED; };
    comm::axisprofile::AxisProfileResult setProfileScalingStrategyCfg(const comm::axisprofile::AxisProfileScalingStrategyCfg& ) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileScalingStrategyCfg getProfileScalingStrategyCfg() const final { return comm::axisprofile::AxisProfileScalingStrategyCfg{}; }
    void resetRtInputData() override {};
    void runJobsInBkThread() override {};
    void setCycleTimeToScaling(double /*cycleTime*/) override {};
    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    virtual void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

    //! @brief This method deletes the existing mapping handler, creates a new one, or registers a mapping handler that is passed to the function.
    //! @param[in]   deleteOrCreate   If false, the existing mapping handler will be deleted. If true, a new mapping handler mockup will be created OR
    //!                               if a mapping handler was passed to the function this one will be set.
    //! @param[in]   mappingHandler   Optional. Pointer to the mapping handler instance that should be used.
    void setMappingHandler(bool deleteOrCreate, comm::axisprofile::IAxisProfileMappingHandler* mappingHandler = nullptr)
    {
      if (m_mappingHandler) { delete m_mappingHandler; m_mappingHandler = nullptr; }
      if (deleteOrCreate) { m_mappingHandler = mappingHandler ? mappingHandler : new test::MockAxisProfileMappingHandler(); }
    }

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK;   //!< Result of member functions
    std::string m_profileName;                                           ///< Name of this profile
    std::uint32_t m_deviceAddress{};                                     ///< Address of the connected device
    std::string m_strDeviceName;                                         ///< Name of the connected device
    comm::axisprofile::IAxisProfileMappingHandler* m_mappingHandler{};   ///< Mockup of the mapping handler
    comm::axisprofile::IFieldbusAdapter* m_fieldbusAdapterMock{};        ///< Mockup of the fieldbus adapter class
    comm::axisprofile::IScaling* m_scalingMock{};                        ///< Mockup of the scaling class
    std::uint8_t m_driveNumber {};                                       ///< Drive number of the device
    comm::axisprofile::AxisProfileCategory m_profileCategory {comm::axisprofile::AxisProfileCategory::CATEGORY_DRIVE};	

    std::vector<std::function<comm::axisprofile::AxisProfileResult()>> m_initMethods  //!< Mockup of init methods
    {
      {[this]()->comm::axisprofile::AxisProfileResult { return this->getScaling()->booting(); } },
      {[this]()->comm::axisprofile::AxisProfileResult { return this->getCommandInterface()->booting(); } },
      {[this]()->comm::axisprofile::AxisProfileResult { return this->getDeviceParamInterface()->booting(); } },
      {[this]()->comm::axisprofile::AxisProfileResult { return this->getStatusControlInterface()->booting(); } },
    };
    bool m_profileActivated{true};         ///< This property holds whether the profile is activated.

  private: // data

    test::MockAxisProfileTrace m_profileTraceMock;              //!< Mockup of the axisprofile trace helper class
    test::MockAxisProfileCommand m_dprCommandMock;              //!< Mockup of the command interface
    test::MockAxisProfileCyclicData m_dprCyclicDataMock;        //!< Mockup of the cyclic data interface
    test::MockAxisProfileStatusControl m_dprStatusControlMock;  //!< Mockup of the status control interface
    test::MockAxisProfileDiagMsg m_dprDiagMsgMock;              //!< Mockup of the diagnostic message interface
    test::MockAxisProfileDeviceParam m_dprDeviceParamMock;      //!< Mockup of the device parameter interface
    test::MockAxisProfileProbe m_dprProbeMock;                  //!< Mockup of the probe interface
    test::MockAxisProfileCmd m_dprCmdMock;                      //!< Mockup of the command interface
    test::MockJobMgr m_jobMgr;                                  ///< Mockup of the job manager
    test::MockAxisProfileProperty m_dprPropertyMock;            //!< Mockup of the property interface
  };
}
