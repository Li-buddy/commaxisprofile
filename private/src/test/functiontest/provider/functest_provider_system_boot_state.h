#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "functiontest/functest_base.h"

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

//! @class FuncTestProviderBootState
//! Functiontest of the boot state
class FuncTestProviderBootState : public FuncTestBase
{
public:
  //! Constructor
  FuncTestProviderBootState();
  //! Destructor
  virtual ~FuncTestProviderBootState() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("FuncTest.Provider.BootState");
  TEST_DECLARE_DESCRIPTION("Functiontest for profile configuration via datalayer.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_SD);

  //! @brief Init necessary motion system objects on test call.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Free memory of allocated objects in doInit() test function.
  //! @return S_OK, when deletion successful.
  HRESULT doEnd();

private: 

    //! function pointer to flatbuffer verifier
    typedef bool(*FbVerifier)(flatbuffers::Verifier &verifier); 
    //methods
    //! @brief Create and initialize flatbuffer with test data
    //! @param[in,out]    var           variant pointer which will hold generated flatbuffer
    //! @param[out]       fbVerifier    flatbuffer verifier
    //! @return                         S_OK, when tests were successful
    HRESULT createFlatbuffer(comm::datalayer::Variant* var, FbVerifier &fbVerifier);
        //! @brief Test case to read complete flatbuffer at once
    //! @param[in]    expFb       expected flatbuffer
    //! @param[in]    url         address of flatbuffer node
    //! @param[in]    fbVerifier  flatbuffer verifier
    //! @param[in]    failOnEmpty should the reading fail with an empty input buffer?
    //! @return                   S_OK, when tests were successful
    HRESULT testCompleteRead(const comm::datalayer::Variant* expFb, const std::string& url, FbVerifier &fbVerifier, bool failOnEmpty);

    //! @brief Test case to read partial flatbuffer with input flatbuffer provided
    //! @param[in]    expFb       expected flatbuffer
    //! @param[in]    url         address of flatbuffer node
    //! @param[in]    fbVerifier  flatbuffer verifier
    //! @return                   S_OK, when tests were successful
    HRESULT testPartialRead(const comm::datalayer::Variant* expFb, const std::string& url, FbVerifier &fbVerifier);

    //! @brief Test case to read invalid flatbuffer from data layer node
    //! @param[in]    url         address of flatbuffer node
    //! @return                   S_OK, when tests were successful
    HRESULT testFailRead(const std::string& url);

    // //! @brief Test case to read meta data from data layer node
    // //! @param[in]    url         address of flatbuffer node
    // //! @return                   S_OK, when tests were successful
    HRESULT testMetadata(const std::string& url);

};
