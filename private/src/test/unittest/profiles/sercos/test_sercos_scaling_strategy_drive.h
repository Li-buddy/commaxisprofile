#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "mockup/external_bundles/ethercat_master/mock_ethercat_master_nodes.h"
#include "profiles/sercos/drive/sercos_scaling_strategy_drive.h"
#include "stdafx.h"

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

/// @brief Test class for the sercos scaling class.
class TestScalingStrategySoEDrive: public xTST_Test2
{
private: // Types

  struct TestPosData
  {
    std::uint16_t m_posScalingType{};                           ///< S-0-0076 (Position data scaling type)
    std::uint16_t m_posScalingFactor{};                         ///< S-0-0077 (Linear position data scaling factor)
    std::int16_t m_posScalingExponent{};                        ///< S-0-0078 (Linear position data scaling exponent)
    std::uint32_t m_posRotationalResolution{};                  ///< S-0-0079 (Rotational position resolution)
    double m_expLSBWeight{};                                    ///< Expected least significant bit weight
    comm::axisprofile::AxisProfileResult m_expResult{};         ///< Expected return code of the axis-profile
    const wchar_t *m_testName{};                                ///< name of the test
  };

  struct TestVelData
  {
    std::uint16_t m_velScalingType{};                           ///< S-0-0044 (Position data scaling type)
    std::uint16_t m_velScalingFactor{};                         ///< S-0-0045 (Linear position data scaling factor)
    std::int16_t m_velScalingExponent{};                        ///< S-0-0046 (Linear position data scaling exponent)
    double m_expLSBWeight{};                                    ///< Expected least significant bit weight
    comm::axisprofile::TimeUnit m_expTimeUnit{};                ///< Expected time unit
    comm::axisprofile::VelScalingType m_expVelType{};           ///< Expected velocity scaling type
    comm::axisprofile::AxisProfileResult m_expResult{};         ///< Expected return code of the axis-profile
    const wchar_t *m_testName{};                                ///< name of the test
  };

  struct TestAccData
  {
    std::uint16_t m_accScalingType{};                           ///< S-0-0160 (acceleration data scaling type)
    std::uint16_t m_accScalingFactor{};                         ///< S-0-0161 (Linear acceleration data scaling factor)
    std::int16_t m_accScalingExponent{};                        ///< S-0-0162 (Linear acceleration data scaling exponent)
    double m_expLSBWeight{};                                    ///< Expected least significant bit weight
    comm::axisprofile::AxisProfileResult m_expResult{};         ///< Expected return code of the axis-profile
    const wchar_t *m_testName{};                                ///< name of the test
  };  

  struct TestTorqueData
  {
    std::uint16_t m_translPosScalingFactor{};                   ///< S-0-0077 (scaling factor for translation position data: related to feed constant conversion)
    std::int16_t m_translPosScalingExponent{};                  ///< S-0-0078 (scaling exponent for translation position data: related to feed constant conversion)
    std::uint16_t m_torqueScalingType{};                        ///< S-0-0086 (Torque data scaling type)
    std::uint16_t m_torqueScalingFactor{};                      ///< S-0-0093 (Torque data scaling factor)
    std::int16_t  m_torqueScalingExponent{};                    ///< S-0-0094 (Torque data scaling exponent)
    std::uint32_t m_gearInputRevolution{};                      ///< S-0-0121 (Gear input revolution)
    std::uint32_t m_gearOutputRevolution{};                     ///< S-0-0122 (Gear output revolution)
    std::uint32_t m_feedConstant{};                             ///< S-0-0123 (Feed constant)
    std::uint32_t m_nominalTorque{};                            ///< S-0-0533 (Nominal torque/force of motor)
    double m_expLSBWeight{};                                    ///< Expected least significant bit weight
    bool m_expMetrication{};                                    ///< Expected metrication
    comm::axisprofile::TorqueScalingType m_expTorqueType{};     ///< Expected torque scaling type
    comm::axisprofile::AxisProfileResult m_expResult{};         ///< Expected return code of the axis-profile
    const wchar_t *m_testName{};                                ///< name of the test
  };


public: // Methods

  /// Constructor
  explicit TestScalingStrategySoEDrive() = default;
  /// Destructor
  virtual ~TestScalingStrategySoEDrive() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.ScalingStrategySoEDrive");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos scaling strategy class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief End function of the test.
  /// @return S_OK, when deinitialization successful.
  HRESULT doEnd();

private: // Methods

  /// @brief    Test of method to calculate the LSB weight for position scaling data.
  /// @return   S_OK, if test was successful 
  HRESULT testCalcPosScalingLSBWeight();

  /// @brief    Test of method to calculate the LSB weight for velocity scaling data.
  /// @return   S_OK, if test was successful 
  HRESULT testCalcVelScalingLSBWeight();

  /// @brief    Test of method to calculate the LSB weight for acceleration scaling data.
  /// @return   S_OK, if test was successful 
  HRESULT testCalcAccScalingLSBWeight();  

  /// @brief    Test of method to calculate the LSB weight for torque scaling data.
  /// @return   S_OK, if test was successful 
  HRESULT testCalcTorqueScalingLSBWeight();

  /// @brief    Test of method to access scaling parameters.
  /// @return   S_OK, if test was successful 
  HRESULT testGetterAndSetter();

  /// @brief    Test of method modulo related functions
  /// @return   S_OK, if test was successful 
  HRESULT testModulo() const;

  /// @brief Executes one test-case of the position scaling.
  /// @param[in]   paramRequestNode  Parameter request node for scaling parameter input.
  /// @param[in]   scaling           Sercos scaling class for executing the scaling.
  /// @param[in]   testCaseData      Data of one test-case.
  /// @return      S_OK, if test was successful 
  HRESULT runTestCasePosScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, comm::axisprofile::sercos::ScalingStrategySoEDrive& scaling, const TestPosData& testCaseData);

  /// @brief Executes one test-case of the velocity scaling.
  /// @param[in]   paramRequestNode  Parameter request node for scaling parameter input.
  /// @param[in]   scaling           Sercos scaling class for executing the scaling.
  /// @param[in]   testCaseData      Data of one test-case.
  /// @return      S_OK, if test was successful 
  HRESULT runTestCaseVelScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, comm::axisprofile::sercos::ScalingStrategySoEDrive& scaling, const TestVelData& testCaseData);

  /// @brief Executes one test-case of the acceleration scaling.
  /// @param[in]   paramRequestNode  Parameter request node for scaling parameter input.
  /// @param[in]   scaling           Sercos scaling class for executing the scaling.
  /// @param[in]   testCaseData      Data of one test-case.
  /// @return      S_OK, if test was successful 
  HRESULT runTestCaseAccScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, comm::axisprofile::sercos::ScalingStrategySoEDrive& scaling, const TestAccData& testCaseData);

  /// @brief Executes one test-case of the torque scaling.
  /// @param[in]   paramRequestNode  Parameter request node for scaling parameter input.
  /// @param[in]   scaling           Sercos scaling class for executing the scaling.
  /// @param[in]   testCaseData      Data of one test-case.
  /// @return      S_OK, if test was successful 
  HRESULT runTestCaseTorqueScaling(test::MockEthercatMasterParamRequestNodeSoE& paramRequestNode, comm::axisprofile::sercos::ScalingStrategySoEDrive& scaling, const TestTorqueData& testCaseData);

  /// @brief Simple helper method that prints all scaling parameters.
  /// @param[in]   testCaseData   Data of one test-case.
  inline void printScalingParams(const TestPosData& testCaseData);

  /// @brief Simple helper method that prints all scaling parameters.
  /// @param[in]   testCaseData   Data of one test-case.
  inline void printScalingParams(const TestVelData& testCaseData);

  /// @brief Simple helper method that prints all scaling parameters.
  /// @param[in]   testCaseData   Data of one test-case.
  inline void printScalingParams(const TestAccData& testCaseData);  

  /// @brief Simple helper method that prints all scaling parameters.
  /// @param[in]   testCaseData   Data of one test-case.
  inline void printScalingParams(const TestTorqueData& testCaseData);

private: // Data
  const std::string m_profileName = "testProfile";   ///< Name of the test axis profile
};
