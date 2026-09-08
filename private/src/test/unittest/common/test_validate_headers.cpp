#include <fstream>
#include <sstream>
#include <algorithm>

#include "motion/test/result_helper.h"
#include "test_validate_headers.h"

using namespace test;

REGISTER_TEST(TestValidateHeaders);
REGISTER_TEST_TO_GROUP(TestValidateHeaders, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestValidateHeaders, 2_UnitTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

const std::vector<std::string> TestValidateHeaders::S_GENERATED_HEADER_FILES {
  "include/comm_axisprofile_diagnostics.h",
};

const std::vector<std::string> TestValidateHeaders::S_CODE_HEADER_FILES {
  "diagnosis/axisprofile_main_diag.h",
  "diagnosis/axisprofile_detailed_diag.h",
};


const std::vector<std::string> TestValidateHeaders::S_WHITE_LIST_CODE_HEADER_DIAG_CODES {
  "0x090A02FF", //MAIN_DIAG_AXISPROFILE_TRACE_MESSAGE
  "0x090E02FF", //MAIN_DIAG_AXISPROFILE_TRACE_WARNING
  "0x090F02FF", //MAIN_DIAG_AXISPROFILE_TRACE_ERROR
};



HRESULT TestValidateHeaders::doTest()
{
  log(L"Load all generated header files...");
  DiagCodeMap generatedMain;
  DiagCodeMap generatedDetail;
  RETURN_TEST_RESULT(loadDiagCodesFromFiles(S_GENERATED_HEADER_FILES, generatedMain, generatedDetail), L"\nCan't load generated header files!", S_OK);

  log(L"Load all code header files...");
  DiagCodeMap codeMain;
  DiagCodeMap codeDetail;
  RETURN_TEST_RESULT(loadDiagCodesFromFiles(S_CODE_HEADER_FILES, codeMain, codeDetail), L"\nCan't load code header files!", S_OK);

  log(L"\n\n<b>Evaluate main diagnoses...</b>");
  LOG_TEST_RESULT(evaluate(generatedMain, codeMain), L"\nError in main diagnoses!", true);

  log(L"\n\n<b>Evaluate detail diagnoses...</b>");
  LOG_TEST_RESULT(evaluate(generatedDetail, codeDetail), L"\nError in detail diagnoses!", true);

  return S_OK;
}

HRESULT TestValidateHeaders::loadDiagCodesFromFiles(const std::vector<std::string>& files, DiagCodeMap& main, DiagCodeMap& detail)
{
  main.clear();
  detail.clear();
  for (const auto& file : files)
  {
    std::string content;
    if (loadFileToString(file, content) != S_OK)
    {
      return E_FAIL;
    }
    auto diagCodesMain = getMainDiagnoses(file, content);
    main.merge(diagCodesMain);
    auto diagCodesDetail = getDetailDiagnoses(file, content);
    detail.merge(diagCodesDetail);
  }
  return S_OK;
}

HRESULT TestValidateHeaders::loadFileToString(const std::string& file, std::string& content)
{
  log(L"\nload file %ls...", test::StringHelper::utf8ToWide(file).c_str());
  std::ifstream fs(file);
  RETURN_TEST_RESULT(fs.is_open(), L"\nCan't load file!", true);
  std::stringstream strStream;
  strStream << fs.rdbuf(); //read the file
  content = strStream.str(); //str holds the content of the file
  return S_OK;
}

TestValidateHeaders::DiagCodeMap TestValidateHeaders::getDiagnoses(const std::string& file, const std::string& headerContent, const std::string& diagStart)
{
  DiagCodeMap result;
  std::size_t offset {};
  while (true)
  {
    auto startPos = headerContent.find(diagStart, offset);
    if (startPos == std::string::npos)
    {
      break;
    }

    auto endPos = headerContent.find(";", startPos);
    if (endPos == std::string::npos)
    {
      break;
    }

    if (!isCommentedLine(headerContent, startPos))
    {
      auto diagCode = headerContent.substr(startPos, endPos - startPos);
      if (std::find(S_WHITE_LIST_CODE_HEADER_DIAG_CODES.begin(), S_WHITE_LIST_CODE_HEADER_DIAG_CODES.end(), diagCode) == S_WHITE_LIST_CODE_HEADER_DIAG_CODES.end())
      {
        std::size_t lineOfFile = std::count(headerContent.begin(), headerContent.begin() + startPos, '\n') + 1;
        result[diagCode] = std::make_pair(file, lineOfFile);
      }
    }
    offset = endPos;
  }

  return result;
}

bool TestValidateHeaders::evaluate(const DiagCodeMap& generated, const DiagCodeMap& code)
{
  auto copyOfGenerated = generated;
  // remove all diagnosis codes in code from copyOfGenerated
  for (const auto& c : code)
  {
    copyOfGenerated.erase(c.first);
  }

  if (copyOfGenerated.empty())
  {
    log(L"\n\n\tNo diagnosis codes found, that are in the generated headers, but not in the code headers.");
  }
  else
  {
    log(L"\n\n\tDiagnosis codes, that are in the generated headers, but not in the code headers (might be old codes, that are not used anymore):");
    for (const auto& c : copyOfGenerated)
    {
      log(L"\n\t\t%ls (%ls : %u)", test::StringHelper::utf8ToWide(c.first).c_str(), test::StringHelper::utf8ToWide(c.second.first).c_str(), c.second.second);
    }
  }

  auto copyOfCode = code;
  // remove all diagnosis codes in generated from copyOfCode
  for (const auto& c : generated)
  {
    copyOfCode.erase(c.first);
  }
  if (copyOfCode.empty())
  {
    log(L"\n\n\tNo diagnosis codes found, that are in the code headers, but not in the generated headers. Well done!");
  }
  else
  {
    log(L"\n\n\tDiagnosis codes, that are in the code headers, but not in the generated headers (most likely an error):");
    for (const auto& c : copyOfCode)
    {
      log(L"\n\t\t%ls (%ls : %u)", test::StringHelper::utf8ToWide(c.first).c_str(), test::StringHelper::utf8ToWide(c.second.first).c_str(), c.second.second);
    }
  }

  return copyOfCode.empty();
}

bool TestValidateHeaders::isCommentedLine(const std::string& headerContent, std::size_t ofs)
{
  while ((ofs > 0) && (headerContent[ofs] != '\n'))
  {
    if ((headerContent[ofs - 1] == '/') && (headerContent[ofs] == '/'))
    {
      return true;
    }
    --ofs;
  }
  return false;
}

TestValidateHeaders::DiagCodeMap TestValidateHeaders::getMainDiagnoses(const std::string& file, const std::string& headerContent)
{
  // all main diagnoses start with "0x090"
  return getDiagnoses(file, headerContent, "0x090");
}

TestValidateHeaders::DiagCodeMap TestValidateHeaders::getDetailDiagnoses(const std::string& file, const std::string& headerContent)
{
  // all detail diagnoses start with "0x0C5"
  return getDiagnoses(file, headerContent, "0x0C5");
}
