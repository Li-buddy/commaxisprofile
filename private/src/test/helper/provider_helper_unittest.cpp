// ---------------------------------------------------------------------------
//
//! @file       provider_helper_unittest.cpp
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
#include "provider_helper_unittest.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

// Static member initialization
comm::datalayer::Variant ProviderHelperUnittest::m_returnVariant{};
comm::datalayer::DlResult ProviderHelperUnittest::m_returnStatus{};
comm::datalayer::IClient::ResponseCallback ProviderHelperUnittest::m_callback = [](comm::datalayer::DlResult status, const comm::datalayer::Variant *source)
{
  if (source)
  {
    ProviderHelperUnittest::m_returnVariant = *source;
  }
  ProviderHelperUnittest::m_returnStatus = status;
};


HRESULT ProviderHelperUnittest::onRead(const xTST_Test* test,
                                           comm::axisprofile::provider::INodeBase* node,
                                           const std::string& url,                           /* defaults: */
                                           comm::datalayer::Variant* actData,                /* =nullptr  */
                                           bool expSuccess,                                  /* =true     */
                                           comm::datalayer::DlResult expStatusCode,          /* =DL_OK    */
                                           const comm::datalayer::Variant* expData,          /* =nullptr  */
                                           bool(*verify)(flatbuffers::Verifier &verifier))   /* =nullptr  */
{
  test->log(L"\nonRead: %ls", UTF8TOWIDE(url.c_str()));

  node->onRead(url, nullptr, m_callback);
  if (expSuccess)
  {
    *actData = m_returnVariant;

    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonRead failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }

    if (!expData)
    {
      test->error(L"\nNo expected data variant passed to function!");
      return E_FAIL;
    }

    if (comm::datalayer::VariantType::UNKNOWN == expData->getType())
    {
      test->error(L"\nVariantType not passed to function! Can not check received data.");
      return E_FAIL;
    }

    if (m_returnVariant.getType() != expData->getType())
    {
      test->error(L"\nReceived wrong variant type!");
      return E_FAIL;
    }

    if (comm::datalayer::isArrayType(m_returnVariant.getType()))
    {
      if (m_returnVariant.getCount() != expData->getCount())
      {
        test->error(L"\nCount of elements do not match! Expected: %d  Actual: %d", expData->getCount(), m_returnVariant.getCount());
        return E_FAIL;
      }
    }

    // If its a flatbuffer we have to do some more checks
    if (comm::datalayer::VariantType::FLATBUFFERS == m_returnVariant.getType())
    {
      if (!verify)
      {
        test->error(L"\nReceived flatbuffer but verifier is missing!");
        return E_FAIL;
      }

      comm::datalayer::DlResult dlResult = m_returnVariant.verifyFlatbuffers(verify);
      if (comm::datalayer::STATUS_FAILED(dlResult))
      {
        test->error(L"\nVerification of flatbuffer failed! DLResult: 0x%04X", dlResult);
        return E_FAIL;
      }
    }

    // Check received data
    if ( FAILED(ProviderHelperCommon::checkAndPrintResult(test, &m_returnVariant, expData)))
    {
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonRead should not be possible but returned data!");
      return E_FAIL;
    }
    else if (m_returnStatus != expStatusCode)
    {
      test->error(L"\nonRead did not fail with expected error. act: %ls, exp: %ls!", UTF8TOWIDE(m_returnStatus.toString()), UTF8TOWIDE(expStatusCode.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonRead failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperUnittest::onWrite(const xTST_Test* test,
                                            comm::axisprofile::provider::INodeBase* node,
                                            const std::string& url,
                                            comm::datalayer::Variant* data,            /* defaults: */
                                            bool expSuccess,                           /* =true     */
                                            comm::datalayer::DlResult /*expStatusCode*/)   /* =DL_OK    */
{
  test->log(L"\nonWrite: %ls", UTF8TOWIDE(url.c_str()));

  node->onWrite(url, data, m_callback);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonWrite failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonWrite was successful. <DARK_GREEN><b>OK.</b></DARK_GREEN>");
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonWrite should not be possible but returned success!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonWrite failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperUnittest::onRemove(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, bool expSuccess, comm::datalayer::DlResult expStatusCode)
{
  test->log(L"\nonRemove: %ls", UTF8TOWIDE(url.c_str()));

  node->onRemove(url, m_callback);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonRemove failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonRemove should not be possible but returned success!");
      return E_FAIL;
    }
    else if (m_returnStatus != expStatusCode)
    {
      test->error(L"\nonRemove did not fail with expected status code! Exp: %ls  Act: %ls", UTF8TOWIDE(expStatusCode.toString()), UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonRemove failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperUnittest::onBrowse(const xTST_Test* test,
                                             comm::axisprofile::provider::INodeBase* node,
                                             const std::string& url,                    /* defaults: */
                                             bool expSuccess,                           /* =true     */
                                             const comm::datalayer::Variant* expData,   /* =nullptr  */
                                             comm::datalayer::DlResult expStatusCode)   /* =DL_OK*   */
{
  test->log(L"\nonBrowse: %ls", UTF8TOWIDE(url.c_str()));

  node->onBrowse(url, m_callback);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonBrowse failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }

    if (!expData)
    {
      test->error(L"\nNo expected data variant passed to function!");
      return E_FAIL;
    }

    if (comm::datalayer::isArrayType(m_returnVariant.getType()))
    {
      if (m_returnVariant.getCount() != expData->getCount())
      {
        test->error(L"\nCount of elements do not match! Expected: %d  Actual: %d", expData->getCount(), m_returnVariant.getCount());
        return E_FAIL;
      }
    }

    if (comm::datalayer::VariantType::ARRAY_OF_STRING != m_returnVariant.getType())
    {
      test->error(L"\nReceived wrong variant type (no array of strings)!");
      return E_FAIL;
    }

    // Check received data
    if (FAILED(ProviderHelperCommon::checkAndPrintResult(test, &m_returnVariant, expData)))
    {
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonBrowse should not be possible but returned data!");
      return E_FAIL;
    }
    else if (m_returnStatus != expStatusCode)
    {
      test->error(L"\nonBrowse did not fail with expected status code! Exp: %ls  Act: %ls", UTF8TOWIDE(expStatusCode.toString()), UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonBrowse failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperUnittest::onCreate(const xTST_Test* test,
                                             comm::axisprofile::provider::INodeBase* node,
                                             const std::string& url,
                                             const comm::datalayer::Variant* data,            /* defaults: */
                                             bool expSuccess,                                 /* =true     */
                                             comm::datalayer::DlResult expStatusCode)         /* =DL_OK    */
{
  test->log(L"\nonCreate: %ls", UTF8TOWIDE(url.c_str()));

  node->onCreate(url, data, m_callback);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonCreate failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonCreate should not be possible but returned success!");
      return E_FAIL;
    }
    else if (m_returnStatus != expStatusCode)
    {
      test->error(L"\nonCreate did not fail with expected status code! Exp: %ls  Act: %ls", UTF8TOWIDE(expStatusCode.toString()), UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonCreate failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperUnittest::onMetadata(const xTST_Test* test, comm::axisprofile::provider::INodeBase* node, const std::string& url, bool expSuccess, bool expectFlatbuffer, comm::datalayer::DlResult expStatusCode)
{
  test->log(L"\nonMetadata: %ls", UTF8TOWIDE(url.c_str()));

  node->onMetadata(url, m_callback);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(m_returnStatus))
    {
      test->error(L"\nonMetadata failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }

    if (expectFlatbuffer && (comm::datalayer::VariantType::FLATBUFFERS != m_returnVariant.getType()))
    {
      test->error(L"\nReceived wrong variant type (no Flatbuffer)!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tReceived flatbuffer from node. <DARK_GREEN><b>OK.</b></DARK_GREEN>");
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(m_returnStatus))
    {
      test->error(L"\nonMetadata should not be possible but returned data!");
      return E_FAIL;
    }
    else if (m_returnStatus != expStatusCode)
    {
      test->error(L"\nonMetadata did not fail with expected status code! Exp: %ls  Act: %ls", UTF8TOWIDE(expStatusCode.toString()), UTF8TOWIDE(m_returnStatus.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tonMetadata failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(m_returnStatus.toString()));
    }
  }

  return S_OK;
}
