#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_diagnosis.h
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
//! of the diagnosis registration interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "common/log/diagnosis/i_registration4.h"
#include "common/log/diagnosis/i_log4.h"
#include "common/log/trace/i_registration_real_time3.h"
#include "common/log/trace/i_log_real_time3.h"
#include "common/log/diagnosis/log_parameters.h"

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

  //! @class MockDiagnosisLog
  //! Helper class to simulate the diagnosis log in test
  class MockDiagnosisLog : public common::log::diagnosis::ILog4
  {
  public:
    MockDiagnosisLog() = default;
    virtual ~MockDiagnosisLog() noexcept override = default;
    common::log::diagnosis::DiagnosisResult log(const common::log::diagnosis::LogType&, std::uint32_t, const common::log::diagnosis::LogParameters*, ...) override
    {
      return m_returnResult;
    }


    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(common::log::diagnosis::DiagnosisResult result) { m_returnResult = result; }

  private:
    common::log::diagnosis::DiagnosisResult m_returnResult = common::log::diagnosis::DiagnosisResult::OKAY;   //!< Result of member functions
    static const common::log::diagnosis::LogParameters m_logParameters;  //!< log parameters               
  };


  //! @class MockDiagnosisRegistration
  //! Helper class to simulate the diagnosis registration in test
  class MockDiagnosisRegistration : public common::log::diagnosis::IRegistration4
  {
  public:
    MockDiagnosisRegistration() = default;
    virtual ~MockDiagnosisRegistration() noexcept override = default;

    //! @brief This deprecated method registers main diagnoses to the diagnosis system.
    //! @return       Status of execution indicating success or error referred 
    common::log::diagnosis::DiagnosisResult registerDefaultDiagnoses(const std::string& /*pathToJsonFile*/, std::shared_ptr<common::log::diagnosis::ILog4>& logReference) override
    {
      logReference = m_mockLog;
      return m_returnResult;
    }

    //! @brief This method unregisters main diagnoses from the diagnosis system.
    //! @return       Status of execution indicating success or error referred 
    common::log::diagnosis::DiagnosisResult unregisterDefaultDiagnoses(const std::string& /*pathToJsonFile*/) override { return m_returnResult; }
    
    //! @brief This method registers detailed diagnoses to the diagnosis system.
    //! @return       Status of execution indicating success or error referred 
    common::log::diagnosis::DiagnosisResult getDefaultMainDiagnosisText(std::uint32_t /*mainDiagnosisNumber*/, std::string& /*mainDiagnosisText*/) override { return m_returnResult; }
        
    //! @brief This method gets the detailed diagnosis text from the corresponding registered detailed diagnosis element by passing the detailed diagnosis code.
    //! @return       Status of execution indicating success or error referred
    common::log::diagnosis::DiagnosisResult getDefaultDetailedDiagnosisText(std::uint32_t /*detailedDiagnosisNumber*/, std::uint32_t /*relatedMainDiagnosisNumber*/, std::string& /*detailedDiagnosisText*/) override { return m_returnResult; }

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(common::log::diagnosis::DiagnosisResult result) { m_returnResult = result; }


    //! @brief This method can be used by tests to set a log object for testing
    //!
    //! @param[in]    unit   New log object
    void setTestLog(std::shared_ptr<common::log::diagnosis::ILog4> unit) { m_mockLog = unit; }

  private:
    common::log::diagnosis::DiagnosisResult m_returnResult = common::log::diagnosis::DiagnosisResult::OKAY;   //!< Result of member functions
    std::shared_ptr<common::log::diagnosis::ILog4> m_mockLog{};                                               //!< used log object
  };
}


