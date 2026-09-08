#pragma once

// ---------------------------------------------------------------------------
//
//! @file       provider_helper_common.h
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
//! This header file is part of the axis profile library and declares common
//! implementations used in both provider helpers (unittest and functiontest).
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include <string>
#include "provider/provider_helper.h"

// External Includes
#include "comm/datalayer/variant.h"


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

  //! @brief Converting ascii literals to wchar
  #define LWIDECHAR(text) L##text
  //! @brief Converting ascii literals to wchar
  #define LITERALWIDECHAR(text) LWIDECHAR(text)
  
  //! @brief Simple macro that checks an HRESULT status code and stores it on error.
  //! This macro requires the local variable 'HRESULT hResult'!
  #define PROVIDER_CHECK(status) \
  {\
    if (FAILED(status))\
    {\
      error(L"\nError at %ls:%d",LITERALWIDECHAR(__FILE__),__LINE__);\
      hResult = status;\
    }\
  }
  
  //! @brief Simple macro that checks an HRESULT status code and returns on error.
  #define PROVIDER_CHECK_AND_RET(status) \
  {\
    if (FAILED(status))\
    {\
      error(L"\nReturning at %ls:%d",LITERALWIDECHAR(__FILE__),__LINE__);\
      return status;\
    }\
  }


  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Forward references
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class ProviderHelperCommon
  //! Common provider helper methods for unittests and functiontests
  class ProviderHelperCommon
  {

  public: // methods
    //! @brief destructor
    virtual ~ProviderHelperCommon() noexcept = default;

    //! @brief Replaces the wildcard in the given url with a profile name.
    //! @param[in,out]  url           Url in which the wildcard should be replaced.
    //! @param[in]      profileName   Name of the profile.
    //! @return  Return value indicating success (true) or error (false).
    static bool insertProfileName(std::string& url, const std::string& profileName);

    //! @brief Removes a leading slash (if present) on a given string.
    //! @param[in,out]  str          String in which the slash should be removed.
    //! @return  Return value indicating success (true) or error (false).
    static bool removeLeadingSlash(std::string* str);

    //! @brief This method checks if the received variant matches the expected variant and prints the result.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[out]     actData      Variant where the received data is written to.
    //! @param[in]      expData      Optional. Expected variant after reading. Needed only when expSuccess is 'true'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT checkAndPrintResult(const xTST_Test* test, const comm::datalayer::Variant* actData, const comm::datalayer::Variant* expData);
  };
}
