#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "helper/flatbuffers_helper_coe.h"

// Test Class
#include "fieldbuses/fieldbus_adapter_coe.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestFieldbusAdapterCoE
//! Test class for the CoE fieldbus adapter class.
class TestFieldbusAdapterCoE : public xTST_Test2
{
public:
  //! c'tor
  TestFieldbusAdapterCoE();
  //! Destructor
  virtual ~TestFieldbusAdapterCoE() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Fieldbuses.AdapterCoE");
  TEST_DECLARE_DESCRIPTION("Unittest of the CoE fieldbus adapter class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_SD);

  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief End function of the test.
  //! @return S_OK, when deinitialization successful.
  HRESULT doEnd();

private: // Methods

  //! @brief    Test of methods to read/write a parameter asynchronous.
  //! @return   S_OK, if test was successful 
  HRESULT testReadWriteParamAsync();

  //! @brief    Test of methods to read/write a parameter synchronous.
  //! @return   S_OK, if test was successful 
  HRESULT testReadWriteParamSync();

  //! @brief    Test of method getParamResponseData.
  //! @return   S_OK, if test was successful 
  HRESULT testGetParamResponseData();

  //! @brief    Test of method to read the slave state synchronous.
  //! @return   S_OK, if test was successful 
  HRESULT testReadSlaveStateSync();

  //! @brief    Test of method to read the master state synchronous.
  //! @return   S_OK, if test was successful 
  HRESULT testReadMasterStateSync();

  //! @brief    Miscellaneous tests
  //! @return   S_OK, if test was successful 
  HRESULT testMiscellaneous();

private: // Data

  test::FlatbuffersHelperCoE m_fbsHelper{};   //!< Helper class to generate flatbuffers
};

