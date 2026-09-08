#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "provider/provider_helper.h"
// Test Class
#include "provider/nodes/node_transaction_based_loadsave.h"
// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace provider
    {
      enum class SaveLoadOperation;
    }
  }
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestProviderNodeTransactionLoadSave
//! Test class of the backup/restore datalayer node
class TestProviderNodeTransactionLoadSave : public xTST_Test2
{
public:
  //! Constructor
  TestProviderNodeTransactionLoadSave();
  //! Destructor
  virtual ~TestProviderNodeTransactionLoadSave() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.TestProviderNodeTransactionLoadSave");
  TEST_DECLARE_DESCRIPTION("Unittest of the datalayer node for the transaction based load and save functionality of one axisprofile.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);


  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

private: //methods

  //! @brief Check behavior of different error cases.
  //! @return  S_OK, when the test was successful.
  HRESULT testErrors();

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testNode();

  //! @brief Check behavior for a specific operation.
  //! @param[in] operation  operation to test for the node
  //! @return  S_OK, when the test was successful.
  HRESULT testOperation(comm::axisprofile::provider::PersistanceOperation operation);

  //! @brief Check behavior for a specific operation.
  //! @param[in] operation  operation to test for the node
  //! @return  S_OK, when the test was successful.
  HRESULT testOperationPath(comm::axisprofile::provider::PersistanceOperation operation);

  //! @brief Create and initialize flatbuffer with test data
  //! @param[in]      configPath    flatbuffer data: configuration path
  //! @param[in]      phase         flatbuffer data: loading phase
  //! @param[in,out]  var           variant pointer which will hold generated flatbuffer
  //! @param[in]      defaultData   optional parameter which decides if a default flatbuffer should be build
  //! @return                         S_OK, when tests were successful
  HRESULT createFlatbuffer(const std::string& configPath, const std::string& phase, comm::datalayer::Variant* var, bool defaultData = false);  

private: // data

  std::string m_profileName = "testProfile";                                                              //!< Profile name used in test
  std::string m_nodeAddressLoadSingle = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_LOAD;      //!< Node address of the test class to load a single profile
  std::string m_nodeAddressSaveSingle = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_SAVE;      //!< Node address of the test class to save a single profile
  std::string m_nodeAddressLoadSystem = comm::axisprofile::provider::NodeTree.AXISPROFILE_ADMIN_CFG_LOAD; //!< Node address of the test class to load the system
  std::string m_nodeAddressSaveSystem = comm::axisprofile::provider::NodeTree.AXISPROFILE_ADMIN_CFG_SAVE; //!< Node address of the test class to save the system
};
