#pragma once

// ---------------------------------------------------------------------------
//
//! @file       provider_helper_functest.h
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
//! This header file is part of the axisprofile library and implements helper
//! functions to execute and check datalayer requests.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider_helper_common.h"


// ---------------------------------------------------------------------------
// Declaration of macros
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class ProviderHelperFunctest
  //! Helper functions for datalayer provider tests
  class ProviderHelperFunctest
  {

  public: // methods
    //! @brief destructor
    virtual ~ProviderHelperFunctest() noexcept = default;

    //! @brief This method performs a synchronous read on a given non-flatbuffer datalayer node and checks the result.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[out]     actData      Variant where the received data is written to.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expData      Optional. Expected variant after reading. Needed only when expSuccess is 'true'.
    //! @param[in]      verify       Optional. The verifier is needed to check the received data. Needed only when expSuccess is 'true' and the expData is a Flatbuffer.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT readNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* actData,
                                bool expSuccess = true, const comm::datalayer::Variant* expData = nullptr,
                                bool(*verify)(flatbuffers::Verifier &verifier) = nullptr);

    //! @brief This method performs a synchronous write on a given datalayer node and checks the result.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[out]     data         Variant with the data to write.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT writeNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess = true);

    //! @brief This method performs a synchronous remove on a given datalayer node and checks the result.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT removeNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, bool expSuccess = true);

    //! @brief This method performs a synchronous brows on a given datalayer node, checks the result and prints it.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[out]     actData      Variant where the received data is written to.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expData      Optional. Expected variant after reading. Needed only when expSuccess is 'true'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT browseNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* actData,
                                  bool expSuccess = true, const comm::datalayer::Variant* expData = nullptr);

    //! @brief This method performs a synchronous create on a given datalayer node, checks the result and prints it.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[in,out]  data         Variant containing the data needed for creation. The result will also be written to this container.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT createNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess = true);

    //! @brief This method performs a synchronous read of metadata on a given datalayer node, checks the result and prints it.
    //! @param[in]      test         Pointer to the test for all outputs.
    //! @param[in]      client       Datalayer client reference.
    //! @param[in]      url          Datalayer node URL reference.
    //! @param[in,out]  data         Variant containing the data needed for creation. The result will also be written to this container.
    //! @param[in]      expSuccess   Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT readMetadataSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess = true);
  };
}
