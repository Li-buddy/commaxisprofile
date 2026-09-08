#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and define the base class of
/// diagnosis for axisprofile object.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2026 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "comm/datalayer/diagnosis_generated.h"
#include "comm/datalayer/variant.h"
#include "common/debug_trace.h"
#include "common/log/diagnosis/i_log4.h"
#include "common/log/diagnosis/i_registration4.h"
#include "diagnosis/axisprofile_detailed_diag.h"
#include "diagnosis/axisprofile_main_diag.h"
#include <memory>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace common::log::diagnosis
{
  class IRegistration4;
}
namespace comm::datalayer
{
  class Variant;
}

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
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

  /// @brief This is the base class for all motion objects.
  class Dia
  {
  public: // methods

    /// @brief constructor
    Dia() = default;

    /// @brief Destructor.
    virtual ~Dia() noexcept;

    /// @brief This method returns the class name.
    ///
    /// @return   Return class name.
    static const char* getClassName(void) { return (CLASS_NAME(Dia)); }

    /// @brief Static function to set an unique diagnosis for a single axis profile.
    /// @param[in]   mainDiagnosisCode      32Bit main diagnosis code defined in the diagnosis database
    /// @param[in]   detailedDiagnosisCode  32Bit detailed diagnosis code defined in the diagnosis database
    /// @param[in]   profileName            Name of the profile
    /// @param[in]   fileName               Filename (use __FILE__)
    /// @param[in]   functionName           Function name (use __FUNCTION__)
    /// @param[in]   lineNumber             Line number (use __LINE__)
    /// @param[in]   dynamicDescription     Dynamic description of diagnosis
    /// @param[in]   Args                   Arguments for dynamic description of diagnosis
    ///
    /// @return  Return value indicating success or error.
    template<typename... ArgsType>
    static AxisProfileResult sendDiagnosis(const std::uint32_t mainDiagnosisCode, const std::uint32_t detailedDiagnosisCode, const char* profileName, const char* fileName, const char* functionName, const std::uint32_t lineNumber, const char* dynamicDescription, const ArgsType&... Args)
    {
      common::log::diagnosis::DiagnosisResult diagnosisResult = common::log::diagnosis::DiagnosisResult::OKAY;
      if (getDiagnosisLog())
      {
        // bugfix fixedstop_auto_test
        COMMON_LOG_DIAGNOSIS_LOG4_EXTENDED_WITH_RESULT(diagnosisResult, getDiagnosisLog(), ::common::log::diagnosis::LogType::SET_AND_RESET, mainDiagnosisCode, detailedDiagnosisCode, profileName, "rexroth-automation", "comm.axisprofile",
          common::log::trace::getFileName(fileName), functionName, lineNumber, dynamicDescription, Args...);
        if (common::log::diagnosis::statusFailed(diagnosisResult))
        {
          APR_TRACE_ERROR("Failed to send diagnosis! logExtended() returned 0x%08X", diagnosisResult);
          return AxisProfileResult::ERR_SEND_DIAGNOSIS;
        }
      }
      else
      {
        APR_TRACE_ERROR("Diagnosis bundle is not loaded");
      }
      return AxisProfileResult::STS_OK;
    }

    /// @brief Static function to set an unique diagnosis and for the comm.axisprofile.
    /// @param[in]   mainDiagnosisCode      32Bit main diagnosis code defined in the diagnosis database
    /// @param[in]   detailedDiagnosisCode  32Bit detailed diagnosis code defined in the diagnosis database
    /// @param[in]   fileName               Filename (use __FILE__)
    /// @param[in]   functionName           Function name (use __FUNCTION__)
    /// @param[in]   lineNumber             Line number (use __LINE__)
    /// @param[in]   dynamicDescription     Dynamic description of diagnosis
    /// @param[in]   Args                   Args of dynamic description of diagnosis
    ///
    /// @return  Return value indicating success or error.
    template<typename... ArgsType>
    static AxisProfileResult sendDiagnosis(const std::uint32_t mainDiagnosisCode, const std::uint32_t detailedDiagnosisCode, const char* fileName, const char* functionName, const std::uint32_t lineNumber, const char* dynamicDescription, const ArgsType&... Args)
    {
      return sendDiagnosis(mainDiagnosisCode, detailedDiagnosisCode, nullptr, fileName, functionName, lineNumber, dynamicDescription, Args...);
    }

    /// @brief Static function to send a diagnosis based on a given datalayer diagnosis flatbuffer.
    /// @param[in]   data           Variant which contains the datalayer diagnosis flatbuffer
    /// @param[in]   profileName    Name of the profile
    /// @param[in]   fileName       Filename (use __FILE__)
    /// @param[in]   functionName   Function name (use __FUNCTION__)
    /// @param[in]   lineNumber     Line number (use __LINE__)
    /// @param[in]   addDynTxt      Additional dynamic text to append to the diagnosis
    /// @param[in]   Args           Additional arguments for formatting the dynamic text
    ///
    /// @return  Return value indicating success or error.
    template<typename... ArgsType>
    static AxisProfileResult sendDiagnosis(const comm::datalayer::Variant* data, const char* profileName, const char* fileName, const char* functionName, const std::uint32_t lineNumber, const char* addDynTxt, const ArgsType&... Args)
    {
      common::log::diagnosis::DiagnosisResult diagnosisResult = common::log::diagnosis::DiagnosisResult::OKAY;
      if (getDiagnosisLog())
      {
        if (!data)
        {
          APR_TRACE_ERROR("sendDiagnosis() has been called without datalayer diagnosis flatbuffer!");
          return AxisProfileResult::ERR_EXEC_NULL;
        }

        if (data->getType() != comm::datalayer::VariantType::FLATBUFFERS)
        {
          APR_TRACE_ERROR("sendDiagnosis() has been called with wrong variant type!");
          return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
        }

        flatbuffers::Verifier verifier(data->getData(), data->getSize());
        if (!comm::datalayer::VerifyDiagnosisBuffer(verifier))
        {
          APR_TRACE_ERROR("sendDiagnosis() has been called with wrong flatbuffer (verify failed)!");
          return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
        }

        auto variantData = comm::datalayer::GetDiagnosis(data->getData());

        if (!flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_MAINDIAGNOSISCODE) ||
            !flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_DETAILEDDIAGNOSISCODE))
        {
          APR_TRACE_ERROR("sendDiagnosis() has been called without main- or detailed diagnosis code!");
          return AxisProfileResult::ERR_DATALAYER_WRONG_TYPE;
        }

        const std::uint32_t mainDiagCode = variantData->mainDiagnosisCode();
        const std::uint32_t detailedDiagCode = variantData->detailedDiagnosisCode();

        std::string respDynTxt = "";
        if (flatbuffers::IsFieldPresent(variantData, comm::datalayer::Diagnosis::VT_DYNAMICDESCRIPTION))
        {
          respDynTxt = flatbuffers::GetString(variantData->dynamicDescription());
        }

        std::string dynamicDescription = "ECat: " + respDynTxt + ", APR: " + addDynTxt;
        COMMON_LOG_DIAGNOSIS_LOG4_EXTENDED_WITH_RESULT(diagnosisResult, getDiagnosisLog(), ::common::log::diagnosis::LogType::SET_AND_RESET, mainDiagCode, detailedDiagCode, profileName, "rexroth-automation", "comm.axisprofile",
          common::log::trace::getFileName(fileName), functionName, lineNumber, dynamicDescription.c_str(), Args...);

        if (common::log::diagnosis::statusFailed(diagnosisResult))
        {
          APR_TRACE_ERROR("Failed to send diagnosis! logExtended() returned 0x%08X", diagnosisResult);
          return AxisProfileResult::ERR_SEND_DIAGNOSIS;
        }
      }
      else
      {
        APR_TRACE_ERROR("Diagnosis bundle is not loaded");
      }
      return AxisProfileResult::STS_OK;
    }

    /// @brief Static function to send a diagnosis based on a given datalayer diagnosis flatbuffer.
    /// @param[in]   data           Variant which contains the datalayer diagnosis flatbuffer
    /// @param[in]   fileName       Filename (use __FILE__)
    /// @param[in]   functionName   Function name (use __FUNCTION__)
    /// @param[in]   lineNumber     Line number (use __LINE__)
    /// @param[in]   addDynTxt      Additional dynamic text to append to the diagnosis
    /// @param[in]   Args           Additional arguments for formatting the dynamic text
    ///
    /// @return  Return value indicating success or error.
    template<typename... ArgsType>
    static AxisProfileResult sendDiagnosis(const comm::datalayer::Variant* data, const char* fileName, const char* functionName, const std::uint32_t lineNumber, const char* addDynTxt, const ArgsType&... Args)
    {
      return Dia::sendDiagnosis(data, nullptr, fileName, functionName, lineNumber, addDynTxt, Args...);
    }

    /// @brief Set the diagnosis pointer from the bundle
    /// @param[in]  registration  pointer to the diagnosis registration
    /// @param[in]  bundleRootPath input bundle path
    void setDiagnosisHandler(common::log::diagnosis::IRegistration4* registration, const std::string& bundleRootPath);

    /// @brief Remove the diagnosis pointer from the bundle
    /// @param[in]  registration  pointer to the diagnosis registration
    void removeDiagnosisHandler(common::log::diagnosis::IRegistration4* registration);

  private: // methods

    /// @brief This method is necessary to ensure that the static shared pointer symbol 's_diagnosisLog' is exported and accessible from external applications.
    /// 
    /// At first sight the static member 's_diagnosisLog' is not accessed directly from a calling application (e.g. the testbuddy). Nevertheless the compiler
    /// will inline expand some functions in the calling application which makes it access the variable directly. This results in a linker error. To avoid that
    /// we use this simple getter method that ensures an export of the symbol.
    /// For a more detailed discussion on this topic see e.g.: https://stackoverflow.com/questions/2479784/exporting-static-data-in-a-dll
    /// 
    /// @return The static shared pointer of the diagnosis log.
    static std::shared_ptr<common::log::diagnosis::ILog4> getDiagnosisLog();

  private: // data

    static std::shared_ptr<common::log::diagnosis::ILog4> s_diagnosisLog;  ///< Pointer to the diagnosis log
    std::string m_registrationFilePath {};                                 ///< path to registered diagnosis file
  };
}
