// ---------------------------------------------------------------------------
//
//! @file       provider_helper_common.cpp
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
//! This source file is part of the motion library and declares common
//! implementations used in both provider helpers (unittest and functiontest).
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <math.h>
#include "provider_helper_common.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;


// ---------------------------------------------------------------------------
// Class implementation
// ---------------------------------------------------------------------------

bool ProviderHelperCommon::insertProfileName(std::string& url, const std::string& profileName)
{
  auto checkBasePath = url.find(comm::axisprofile::provider::NodeTree.APR_PROFILES);
  if (checkBasePath == std::string::npos)
  {
    return false;
  }

  auto wildcardPos = checkBasePath + comm::axisprofile::provider::NodeTree.APR_PROFILES.length() + 1;
  if (url[wildcardPos] != '*')
  {
    return false;
  }

  url.replace(wildcardPos, 1, profileName);
  return true;
}


bool ProviderHelperCommon::removeLeadingSlash(std::string* str)
{
  auto slashPos = str->find("/");
  if ((slashPos == std::string::npos) || (slashPos != 0))
  {
    return false;
  }
  else
  {
    *str = str->substr(1, str->size() - 1);
    return true;
  }
}


HRESULT ProviderHelperCommon::checkAndPrintResult(const xTST_Test* test, const comm::datalayer::Variant* actData, const comm::datalayer::Variant* expData)
{
  switch (actData->getType())
  {
  case comm::datalayer::VariantType::BOOL8:
  {
    const bool convActData = *(reinterpret_cast<bool*>(actData->getData()));
    const bool convExpData = *(reinterpret_cast<bool*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %ld   Expected = %ld", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %ld   Expected = %ld <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::UINT8:
  {
    const std::uint8_t convActData = *(reinterpret_cast<std::uint8_t*>(actData->getData()));
    const std::uint8_t convExpData = *(reinterpret_cast<std::uint8_t*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %u   Expected = %u", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %u   Expected = %u <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::UINT32:
  {
    const std::uint32_t convActData = *(reinterpret_cast<std::uint32_t*>(actData->getData()));
    const std::uint32_t convExpData = *(reinterpret_cast<std::uint32_t*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %u   Expected = %u", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %u   Expected = %u <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::INT32:
  {
    const std::int32_t convActData = *(reinterpret_cast<std::int32_t*>(actData->getData()));
    const std::int32_t convExpData = *(reinterpret_cast<std::int32_t*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %d   Expected = %d", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %d   Expected = %d <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::UINT64:
  {
    const std::uint64_t convActData = *(reinterpret_cast<std::uint64_t*>(actData->getData()));
    const std::uint64_t convExpData = *(reinterpret_cast<std::uint64_t*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %lu   Expected = %lu", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %lu   Expected = %lu <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::INT64:
  {
    const std::int64_t convActData = *(reinterpret_cast<std::int64_t*>(actData->getData()));
    const std::int64_t convExpData = *(reinterpret_cast<std::int64_t*>(expData->getData()));
    if (convActData != convExpData)
    {
      test->error(L"\nWrong data! Actual = %ld   Expected = %ld", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %ld   Expected = %ld <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::FLOAT32:
  {
    const float convActData = *reinterpret_cast<float*>(actData->getData());
    const float convExpData = *reinterpret_cast<float*>(expData->getData());
    if (std::fabs(convActData - convExpData) > std::numeric_limits<float>::epsilon())
    {
      test->error(L"\nWrong data! Actual = %.4f   Expected = %.4f", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %.4f   Expected = %.4f <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::FLOAT64:
  {
    const double convActData = *reinterpret_cast<double*>(actData->getData());
    const double convExpData = *reinterpret_cast<double*>(expData->getData());
    if (std::fabs(convActData - convExpData) > std::numeric_limits<double>::epsilon())
    {
      test->error(L"\nWrong data! Actual = %.4f   Expected = %.4f", convActData, convExpData);
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %.4f   Expected = %.4f <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData, convExpData);
    }

    break;
  }
  case comm::datalayer::VariantType::STRING:
  {
    const char *convActData = *actData;
    const std::string actDataVal = convActData ? convActData : "";
    const char *convExpData = *expData;
    const std::string expDataVal = convExpData ? convExpData : "";
    if (actDataVal != expDataVal)
    {
      test->error(L"\nWrong data! Actual = %ls   Expected = %ls", UTF8TOWIDE(actDataVal), UTF8TOWIDE(expDataVal));
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tActual = %ls   Expected = %ls <DARK_GREEN><b>OK.</b></DARK_GREEN>", UTF8TOWIDE(actDataVal), UTF8TOWIDE(expDataVal));
    }

    break;
  }
  case comm::datalayer::VariantType::ARRAY_OF_FLOAT32:
  {
    const float* convActData = reinterpret_cast<float*>(actData->getData());;
    const float* convExpData = reinterpret_cast<float*>(expData->getData());;

    for (std::uint8_t i = 0; i < actData->getCount(); i++)
    {
      if (std::fabs(convActData[i] - convExpData[i]) > std::numeric_limits<float>::epsilon())
      {
        test->error(L"\nWrong data! Actual = %.4f   Expected = %.4f", convActData[i], convExpData[i]);
        return E_FAIL;
      }
      else
      {
        test->log(L"\n\tActual = %.4f   Expected = %.4f <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData[i], convExpData[i]);
      }
    }

    break;
  }
  case comm::datalayer::VariantType::ARRAY_OF_FLOAT64:
  {
    const double* convActData = reinterpret_cast<double*>(actData->getData());
    const double* convExpData = reinterpret_cast<double*>(expData->getData());

    for (std::uint8_t i = 0; i < expData->getCount(); i++)
    {
      if (std::fabs(convActData[i] - convExpData[i]) > std::numeric_limits<double>::epsilon())
      {
        test->error(L"\nWrong data! Actual = %.4f   Expected = %.4f", convActData[i], convExpData[i]);
        return E_FAIL;
      }
      else
      {
        test->log(L"\n\tActual = %.4f   Expected = %.4f <DARK_GREEN><b>OK.</b></DARK_GREEN>", convActData[i], convExpData[i]);
      }
    }

    break;
  }
  case comm::datalayer::VariantType::ARRAY_OF_STRING:
  {
    const char** convActData = *actData;
    const char** convExpData = *expData;

    bool elemFound = false;
    for (std::uint8_t outerIndex = 0; outerIndex < actData->getCount(); outerIndex++)
    {
      std::wstring actDataElem = StringHelper::utf8ToWide(convActData[outerIndex]);
      for (std::uint8_t innerIndex = 0; innerIndex < expData->getCount(); innerIndex++)
      {
        std::wstring expDataElem = StringHelper::utf8ToWide(convExpData[innerIndex]);
        if (actDataElem == expDataElem)
        {
          elemFound = true;
          break;
        }
      }
      if (elemFound)
      {
        test->log(L"\n\t%ls", actDataElem.c_str());
        elemFound = false;
      }
      else
      {
        test->error(L"\nExpected node not found: %ls", actDataElem.c_str());
        return E_FAIL;
      }
    }

    break;
  }
  case comm::datalayer::VariantType::FLATBUFFERS:
  {
    auto size = actData->getSize();
    if (expData->getSize() != size)
    {
      test->error(L"\nWrong data! Expected size %d and received flatbuffer size %d differ!", expData->getSize(), size);
      return E_FAIL;
    }

    if (memcmp(actData->getData(), expData->getData(), size))
    {
      test->error(L"\nWrong data! Expected and received flatbuffer are binary unequal! Was the flatbuffer created in the correct sequence (Inverted to fbs-file)?");
      return E_FAIL;
    }
    else
    {
      test->log(L"\n\tFlatbuffers are binary equal. <DARK_GREEN><b>OK.</b></DARK_GREEN>");
    }

    break;
  }
  default:
  {
    test->error(L"\n%s:%d  Unknown VariantType or implementation for VariantType missing (ToDo)!", LITERALWIDECHAR(__FILE__), __LINE__);
    return E_FAIL;
  }
  }

  return S_OK;
}
