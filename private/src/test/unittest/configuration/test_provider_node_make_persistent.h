#pragma once

#include "stdafx.h"
#include "provider/provider_helper.h"
// Test Class
#include "provider/nodes/node_cfg_make_persistent.h"

//
// test class
//
class TestNodeCfgMakePersistent : public xTST_Test2
{
public:
  TestNodeCfgMakePersistent() = default;
  virtual ~TestNodeCfgMakePersistent() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.TestNodeCfgMakePersistent");
  TEST_DECLARE_DESCRIPTION("Test of NodeCfgMakePersistent class");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_XQF);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PFL);

  HRESULT doInit() final;
  HRESULT doTest() final;

private:

  /// @brief Check behavior of different error cases.
  /// @return  S_OK, when the test was successful.
  HRESULT testErrors();

  /// @brief Check behavior of a successfull node request.
  /// @return  S_OK, when the test was successful.
  HRESULT testNode();

  /// @brief Check behavior for a specific operation.
  /// @return  S_OK, when the test was successful.
  HRESULT testOperation();

  /// @brief Check behavior for a specific operation.
  /// @return  S_OK, when the test was successful.
  HRESULT testOperationPath();

  /// @brief Create and initialize flatbuffer with test data
  /// @param[in]      configPath    flatbuffer data: configuration path
  /// @param[in]      phase         flatbuffer data: loading phase
  /// @param[in,out]  var           variant pointer which will hold generated flatbuffer
  /// @param[in]      defaultData   optional parameter which decides if a default flatbuffer should be build
  /// @return                         S_OK, when tests were successful
  HRESULT createFlatbuffer(const std::string& configPath, const std::string& phase, comm::datalayer::Variant* var, bool defaultData = false);

private: // data

  std::string m_profileName = "testProfile";                                                                            ///< Profile name used in test
  std::string m_nodeAddressLoadSingle = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_LOAD;                    ///< Node address of the test class to load a single profile
  std::string m_nodeAddressMakePersistent = comm::axisprofile::provider::NodeTree.AXISPROFILE_ADMIN_CFG_MAKEPERSISTENT; ///< Node address of the test class to make persistent the system
};
