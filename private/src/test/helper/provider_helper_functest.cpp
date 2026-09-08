#include "provider_helper_functest.h"


using namespace test;

HRESULT ProviderHelperFunctest::readNodeSync(const xTST_Test* test,
                                         comm::datalayer::IClient* client,
                                         const char* url,
                                         comm::datalayer::Variant* actData,                 /* defaults: */
                                         bool expSuccess,                                   /* =true     */
                                         const comm::datalayer::Variant* expData,           /* =nullptr  */
                                         bool(*verify)(flatbuffers::Verifier &verifier))    /* =nullptr  */
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nReadSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous read on given node
  dlResult = client->readSync(url, actData);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nReadSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
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

    if (actData->getType() != expData->getType())
    {
      test->error(L"\nReceived wrong variant type!");
      return E_FAIL;
    }

    if (comm::datalayer::isArrayType(actData->getType()))
    {
      if (actData->getCount() != expData->getCount())
      {
        test->error(L"\nCount of elements do not match! Expected: %d  Actual: %d", expData->getCount(), actData->getCount());
        return E_FAIL;
      }
    }

    // If its a flatbuffer we have to do some more checks
    if (comm::datalayer::VariantType::FLATBUFFERS == actData->getType())
    {
      if (!verify)
      {
        test->error(L"\nReceived flatbuffer but verifier is missing!");
        return E_FAIL;
      }

      comm::datalayer::DlResult dlVerResult = actData->verifyFlatbuffers(verify);
      if (comm::datalayer::STATUS_FAILED(dlVerResult))
      {
        test->error(L"\nVerification of flatbuffer failed! DLResult: %ls", UTF8TOWIDE(dlVerResult.toString()));
        return E_FAIL;
      }
    }

    // Check received data
    if ( FAILED(ProviderHelperCommon::checkAndPrintResult(test, actData, expData)))
    {
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nRead should not be possible but returned data!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tRead failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperFunctest::writeNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess /*= true*/)
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nWriteSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous write on given node
  dlResult = client->writeSync(url, data);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nWriteSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tWrite was successful. <DARK_GREEN><b>OK.</b></DARK_GREEN>");
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nWrite should not be possible but returned success!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tWrite failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}


HRESULT test::ProviderHelperFunctest::removeNodeSync(const xTST_Test * test, comm::datalayer::IClient * client, const char * url, bool expSuccess /*= true*/)
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nRemoveSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous remove on given node
  dlResult = client->removeSync(url);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nRemoveSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nRemove should not be possible but returned success!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tRemove failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperFunctest::browseNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* actData,
                                           bool expSuccess /*= true*/, const comm::datalayer::Variant* expData /*= nullptr*/)
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nBrowseSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous browse on given node
  dlResult = client->browseSync(url, actData);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nBrowseSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }

    if (!expData)
    {
      test->error(L"\nNo expected data variant passed to function!");
      return E_FAIL;
    }

    if (comm::datalayer::isArrayType(actData->getType()))
    {
      if (actData->getCount() != expData->getCount())
      {
        test->error(L"\nCount of elements do not match! Expected: %d  Actual: %d", expData->getCount(), actData->getCount());
        return E_FAIL;
      }
    }

    if (comm::datalayer::VariantType::ARRAY_OF_STRING != actData->getType())
    {
      test->error(L"\nReceived wrong variant type (no array of strings)!");
      return E_FAIL;
    }

    // Check received data
    if (FAILED(ProviderHelperCommon::checkAndPrintResult(test, actData, expData)))
    {
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nBrowse should not be possible but returned data!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tBrowse failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperFunctest::createNodeSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess)
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nCreateSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous create on given node
  dlResult = client->createSync(url, data);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nCreateSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nCreate should not be possible but returned data!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tCreate failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}


HRESULT ProviderHelperFunctest::readMetadataSync(const xTST_Test* test, comm::datalayer::IClient* client, const char* url, comm::datalayer::Variant* data, bool expSuccess)
{
  comm::datalayer::DlResult dlResult;

  test->log(L"\nMetadataSync: %ls", UTF8TOWIDE(url));

  // Invoke synchronous read of metadata on given node
  dlResult = client->metadataSync(url, data);
  if (expSuccess)
  {
    if ( comm::datalayer::STATUS_FAILED(dlResult))
    {
      test->error(L"\nMetadataSync failed! Returned datalayer Statuscode: %ls", UTF8TOWIDE(dlResult.toString()));
      return E_FAIL;
    }

    if (comm::datalayer::VariantType::FLATBUFFERS != data->getType())
    {
      test->error(L"\nReceived wrong variant type (no Flatbuffer)!");
      return E_FAIL;
    }
    else
    {
      // ToDo: Check result here if possible
      test->log(L"\n\tReceived flatbuffer from datalayer. <DARK_GREEN><b>OK.</b></DARK_GREEN>");
    }
  }
  else
  {
    if ( comm::datalayer::STATUS_SUCCEEDED(dlResult))
    {
      test->error(L"\nCreate should not be possible but returned data!");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tCreate failed as expected. Returned datalayer Statuscode: %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(dlResult.toString()));
    }
  }

  return S_OK;
}
