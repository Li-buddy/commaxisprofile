#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include <string>

// Mockups
#include "mockup/external_bundles/mock_diagnosis.h"

// External Includes
#include "comm/datalayer/variant.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

//! @brief This structure represents the data of the datalayer diagnosis flatbuffer
struct diagnosisFbData
{
  std::uint32_t m_mainDiagCode{};            //!< Main diagnosis code
  std::uint32_t m_detailedDiagCode{};        //!< Detailed diagnosis code
  std::string m_dynamicDescription{};   //!< Dynamic Description
};

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    class Dia;
  }
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestDia
//! Test class the diagnosis object
class TestDia : public xTST_Test2
{
public:
  //! Constructor
  TestDia();
  //! Destructor
  virtual ~TestDia() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Diagnosis");
  TEST_DECLARE_DESCRIPTION("Unittest for the diagnosis class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd();

private: // Methods

  //! @brief Check the behavior when the diagnosis bundle is not available.
  //! @return  S_OK, when the test was successful.
  HRESULT testDiagnosisNotAvailable();

  //! @brief Validate sending of a diagnosis
  //! @return  S_OK, when the test was successful.
  HRESULT testSendDiag();

  //! @brief Validate sending of a diagnosis with variant data
  //! @return  S_OK, when the test was successful.
  HRESULT testSendDiagVariant();

  //! @brief Check deinitialization of the diagnosis class
  //! @return  S_OK, when the test was successful.
  HRESULT testDeInit();

  //! @brief Method to generate a datalayer diagnosis flatbuffer
  //! @param[in]    diagData          Input data used to build the flatbuffer.
  //! @param[out]   outData           Variant where the flatbuffer should be stored.
  //! @param[in]    leaveFieldEmpty   Optional. Flag that indicates if a required field should not be added to simulate error cases.
  //! @return       Return value indicating success or error.
  bool generateDiagnosisFlatbuffer(const diagnosisFbData& diagData, comm::datalayer::Variant& outData, bool leaveFieldEmpty = false);

private: // Data

  comm::axisprofile::Dia* m_testClass{};                           //!< Test Class
  test::MockDiagnosisRegistration m_mockDiagRegistration{};        //!< Mockup of the diagnosis registration
  std::shared_ptr<common::log::diagnosis::ILog4> m_mockDiagLog{};  //!< Mockup of the diagnosis shared pointer

  const std::string m_profileName = "testProfile";   //!< Profile name
  const std::string m_dynamicDesc = "axisprofile";   //!< Dynamic diagnosis description
  const std::uint32_t m_exampleMainDiagCode = 1234;       //!< Example main diagnosis code
  const std::uint32_t m_exampleDetailedDiagCode = 5678;   //!< Example detailed diagnosis code
};
