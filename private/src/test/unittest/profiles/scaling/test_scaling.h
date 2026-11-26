#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/scaling/scaling.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestScaling
//! Test class for the sercos scaling class.
class TestScaling: public xTST_Test2
{
private: // Types

 /// @brief Structure the test data
 struct TestData
  {
    comm::axisprofile::PosScalingType m_positionType{};     //!< position scaling type
    comm::axisprofile::VelScalingType m_velocityType{};     //!< velocity scaling type
    comm::axisprofile::AccScalingType m_accelerationType{}; //!< acceleration scaling type
    bool m_posMetrication{};                                //!< position metric system
    bool m_velMetrication{};                                //!< velocity metric system
    bool m_accMetrication{};                                //!< acceleration metric system
    comm::axisprofile::AxisProfileResult m_expResult{};     //!< Expected return code of the axis-profile
    const wchar_t *m_testName{};                            //!< name of the test
  };

 /// @brief Structure the device type of test data
 struct TestDataDeviceType
 {
   comm::axisprofile::PosScalingType m_positionType{};                      //!< internal position type of device
   comm::axisprofile::VelScalingType m_velocityType{};                      //!< internal velocity type of device
   comm::axisprofile::TorqueScalingType m_torqueType{};                     //!< internal torque type of device
   comm::axisprofile::AccScalingType m_accelerationType{};                  //!< internal acceleration type of device
   comm::axisprofile::AxisProfileDeviceScalingType m_expDeviceTypePos{};    //!< expected device type of position
   comm::axisprofile::AxisProfileDeviceScalingType m_expDeviceTypeVel{};    //!< expected device type of velocity
   comm::axisprofile::AxisProfileDeviceScalingType m_expDeviceTypeTorque{}; //!< expected device type of torque
   comm::axisprofile::AxisProfileDeviceScalingType m_expDeviceTypeAcc{};    //!< expected device type of acceleration
   const wchar_t* m_testName{};                                             //!< name of the test
 };

public: // Methods

  //! Constructor
  TestScaling();
  //! Destructor
  virtual ~TestScaling() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Scaling.Scaling");
  TEST_DECLARE_DESCRIPTION("Unittest of the Scaling class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);

  HRESULT doInit() final;
  HRESULT doTest() final;
  HRESULT doEnd() final;

private: // Methods

  //! @brief    Test of method to check scaling uniform.
  //! @return   S_OK, if test was successful 
  HRESULT testcheckScalingUniform();

  //! @brief    Test of method to check scaling uniform.
  //! @param[in] testingScaling       pointer to the scaling object
  //! @param[in] testCase             test data
  //! @return   S_OK, if test was successful 
  HRESULT executeCheckScalingUniform(comm::axisprofile::Scaling* testingScaling, const TestData& testCase);

  //! @brief    Test of method to get the device type.
  //! @return   S_OK, if test was successful 
  HRESULT testGetDeviceType();

  //! @brief    Test of method to get the device type.
  //! @param[in] testingScaling       pointer to the scaling object
  //! @param[in] testCase             test data
  //! @return   S_OK, if test was successful 
  HRESULT executeGetDeviceType(comm::axisprofile::Scaling* testingScaling, const TestDataDeviceType& testCase);
  
  /// @brief This method test modulo related methods.
  /// @return S_OK, when tests were successful
  HRESULT testModuloMethods() const;

  /// @brief Test the out-of-range handling of scaled-to-raw conversions.
  /// @return S_OK, when tests were successful.
  HRESULT testOutOfRangeHandling();

private: // data

  const std::vector<TestData> m_testDataSet;
  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
};
