#pragma once

#include "stdafx.h"
#include <vector>
#include <map>
#include <string>

//
// Test class for validating diagnosis headers
//
class TestValidateHeaders : public xTST_Test2
{
public: // methods
  /// @brief Constructor
  TestValidateHeaders() = default;
  /// @brief Destructor
  virtual ~TestValidateHeaders() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Base.TestValidateHeaders");
  TEST_DECLARE_DESCRIPTION("Test class for validating diagnosis headers");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_XQF);

  HRESULT doTest() final;

private: // types

  using DiagCodeMap = std::map<std::string, std::pair<std::string, std::size_t>>;   ///< data type for diagnosis codes

private: // variables

  const static std::vector<std::string> S_GENERATED_HEADER_FILES;              ///< vector of the generated diagnosis header files
  const static std::vector<std::string> S_CODE_HEADER_FILES;                   ///< vector of the code diagnosis header files in common.axisprofile
  const static std::vector<std::string> S_WHITE_LIST_CODE_HEADER_DIAG_CODES;   ///< a white list of diagnosis codes of header files in common.axisprofile, that should be ignored

private: // methods

  /// @brief load the diagnosis codes of all given files into maps
  /// @param[in]  files         these files should be loaded
  /// @param[out] main          main diagnosis codes of the files are put here
  /// @param[out] detail        detail diagnosis codes of the files are put here
  /// @return  S_OK, when successful
  HRESULT loadDiagCodesFromFiles(const std::vector<std::string>& files, DiagCodeMap& main, DiagCodeMap& detail);

  /// @brief load a single file into a string
  /// @param[in]  file          this file should be loaded
  /// @param[out] content       the content of the file is returned here
  /// @return  S_OK, when successful
  HRESULT loadFileToString(const std::string& file, std::string& content);

  /// @brief get the main diagnosis codes from the given header file content
  /// @param[in] file           file name
  /// @param[in] headerContent  content of one or more header files
  /// @return the main diagnosis codes from the given header file content
  DiagCodeMap getMainDiagnoses(const std::string& file, const std::string& headerContent);

  /// @brief get the detail diagnosis codes from the given header file content
  /// @param[in] file           file name
  /// @param[in] headerContent  content of one or more header files
  /// @return the detail diagnosis codes from the given header file content
  DiagCodeMap getDetailDiagnoses(const std::string& file, const std::string& headerContent);

  /// @brief common method to look for some search string in the given content string and extract the string until the next semicolon.
  /// @param[in] file           file name
  /// @param[in] headerContent  content of one or more header files
  /// @param[in] diagStart      start string of a diagnosis code
  /// @return all found substrings
  DiagCodeMap getDiagnoses(const std::string& file, const std::string& headerContent, const std::string& diagStart);

  /// @brief Check, if the diagnosis codes of the generated headers fit to the codes of the code headers
  /// @param[in] generated      diagnosis codes of the generated headers
  /// @param[in] code           diagnosis codes of the code headers
  /// @return true, when okay
  bool evaluate(const DiagCodeMap& generated, const DiagCodeMap& code);

  /// @brief Check, if the given line (indicated by a position in the multi-line string) is commented (contains "//")
  /// @param[in] headerContent  content of one or more header files
  /// @param[in] ofs            offset in the headerContent (multi-line string)
  /// @return true, if the given line is commented (contains "//")
  bool isCommentedLine(const std::string& headerContent, std::size_t ofs);
};
