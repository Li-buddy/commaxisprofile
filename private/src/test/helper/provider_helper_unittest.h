#pragma once

// ---------------------------------------------------------------------------
//
//! @file       provider_helper_unittest.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2020
//
// ---------------------------------------------------------------------------
//
//! This is a helper class to execute and evaluate datalayer node operations
//! in unittests.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider_helper_common.h"
#include "provider/nodes/i_node_base.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  //! @class ProviderHelperUnittest
  //! Provider helper functions for unittests without a datalayer system.
  class ProviderHelperUnittest
  {

  public: // methods
    //! @brief destructor
    virtual ~ProviderHelperUnittest() noexcept = default;

    //! @brief This method performs a synchronous read on a given non-flatbuffer datalayer node and checks the result.
    //! @param[in]      test            Pointer to the test for all outputs.
    //! @param[in]      node            Pointer to the test node.
    //! @param[in]      url             Datalayer node URL reference.
    //! @param[out]     actData         The variant where the read data will be stored.
    //! @param[in]      expSuccess      Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expStatusCode   Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @param[in]      expData         Optional. Expected variant after reading. Needed only when expSuccess is 'true'.
    //! @param[in]      verify          Optional. The verifier is needed to check the received data. Needed only when expSuccess is 'true' and the expData is a Flatbuffer.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onRead(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, comm::datalayer::Variant* actData,
      bool expSuccess = true, comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK, const comm::datalayer::Variant* expData = nullptr,
      bool(*verify)(flatbuffers::Verifier &verifier) = nullptr);

    //! @brief This method performs a synchronous write on a given datalayer node and checks the result.
    //! @param[in]      test            Pointer to the test for all outputs.
    //! @param[in]      node            Pointer to the test node.
    //! @param[in]      url             Datalayer node URL reference.
    //! @param[out]     data            Variant where the received data is written to.
    //! @param[in]      expSuccess      Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expStatusCode   Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onWrite(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, comm::datalayer::Variant* data, bool expSuccess = true,
      comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK);

    //! @brief This method performs a synchronous remove on a given datalayer node and checks the result.
    //! @param[in]      test            Pointer to the test for all outputs.
    //! @param[in]      node            Pointer to the test node.
    //! @param[in]      url             Datalayer node URL reference.
    //! @param[in]      expSuccess      Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expStatusCode   Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onRemove(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, bool expSuccess = true,
      comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK);

    //! @brief This method performs a synchronous brows on a given datalayer node, checks the result and prints it.
    //! @param[in]      test            Pointer to the test for all outputs.
    //! @param[in]      node            Pointer to the test node.
    //! @param[in]      url             Datalayer node URL reference.
    //! @param[in]      expSuccess      Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expData         Optional. Expected variant after reading. Needed only when expSuccess is 'true'.
    //! @param[in]      expStatusCode   Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onBrowse(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, bool expSuccess = true,
      const comm::datalayer::Variant* expData = nullptr, comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK);

    //! @brief This method performs a synchronous create on a given datalayer node, checks the result and prints it.
    //! @param[in]      test            Pointer to the test for all outputs.
    //! @param[in]      node            Pointer to the test node.
    //! @param[in]      url             Datalayer node URL reference.
    //! @param[in,out]  data            Variant containing the data needed for creation. The result will also be written to this container.
    //! @param[in]      expSuccess      Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expStatusCode   Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onCreate(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, const comm::datalayer::Variant* data, bool expSuccess = true,
      comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK);

    //! @brief This method performs a synchronous read of metadata on a given datalayer node, checks the result and prints it.
    //! @param[in]      test               Pointer to the test for all outputs.
    //! @param[in]      node               Pointer to the test node.
    //! @param[in]      url                Datalayer node URL reference.
    //! @param[in]      expSuccess         Optional. Specifies if the result of browse is expected to succeed (true) or fail (false).
    //! @param[in]      expectFlatbuffer   Flag that indicates if the returned variant must be a flatbuffer (true) or not (false).
    //! @param[in]      expStatusCode      Optional. Expected return status code. Needed only when expSuccess is 'false'.
    //! @return  Return value indicating success (true) or error (false).
    static HRESULT onMetadata(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, bool expSuccess = true, bool expectFlatbuffer = true,
      comm::datalayer::DlResult expStatusCode = comm::datalayer::DlResult::DL_OK);

    //! @brief This is a simple response callback lambda function that can be used to call datalayer function in a test.
    //! @param[in]   status   Datalayer return status code
    //! @param[in]   source   Variant that is set and returned by the called datalayer node
    static comm::datalayer::IClient::ResponseCallback m_callback;

    static comm::datalayer::Variant m_returnVariant;   //!< Return variant which is used inside of the m_callback lambda
    static comm::datalayer::DlResult m_returnStatus;   //!< Return status which is used inside of the m_callback lambda
  };
}
