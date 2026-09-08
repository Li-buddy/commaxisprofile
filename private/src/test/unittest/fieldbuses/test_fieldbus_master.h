#pragma once
#include "stdafx.h"
#include <memory>
#include <vector>
#include "fieldbuses/fieldbus_master.h"

//
// test class
//
class TestFieldbusMaster : public xTST_Test2
{
public:
  TestFieldbusMaster() = default;
  virtual ~TestFieldbusMaster() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Fieldbuses.Master");
  TEST_DECLARE_DESCRIPTION("Test of fieldbus master class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  HRESULT doTest() final;
  
private:  // methods

  /// @brief   test miscellaneous methods.
  /// @return  S_OK, when the test was successful
  HRESULT testMiscellaneous();

  /// @brief   test realtime datalayer access methods.
  /// @return  S_OK, when the test was successful
  HRESULT testRtDataAccess();

private:  // data

  std::string m_masterName {"myMaster"};  ///< name of the master
  std::unique_ptr<::comm::axisprofile::FieldbusMaster> m_fieldbusMaster; ///< test object
  const static std::vector<::comm::axisprofile::MasterRtDataMap> MOCK_RT_DATA_MAP; ///< realtime data map of master
};
