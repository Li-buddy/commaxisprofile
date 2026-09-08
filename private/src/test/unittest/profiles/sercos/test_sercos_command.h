#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "helper/flatbuffers_helper_soe.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace test
{
  class MockDatalayerClient;
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @brief TestSercosCommand
//! Test class for sercos commands of in the axisprofile.
class TestSercosCommand : public xTST_Test2
{
public: // types
  //! @brief This struct contains the test data for a variant that is returned by the datalayer
  struct TestDataVariantU16
  {
    const wchar_t* m_name;          //!< name of the variant type
    std::uint16_t m_data;                //!< data to write in the variant
    bool m_addData;                 //!< add data to variant?
  };

  //! @brief This struct contains the test data for a variant that is returned by the datalayer
  struct TestDataVariantU32
  {
    const wchar_t *m_name;          //!< name of the variant type
    std::uint32_t m_data;                //!< data to write in the variant
    bool m_addData;                 //!< add data to variant?
  };

public: // methods

  //! Constructor
  TestSercosCommand() = default;
  //! Destructor
  virtual ~TestSercosCommand() noexcept override = default;

  TEST_DECLARE_NAME("Unittest.Profiles.Sercos.Command");
  TEST_DECLARE_DESCRIPTION("Unittest of the Sercos profile command class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PFL);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_LLH);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest() final;

private: //method

  //! @brief    Test miscellaneous
  //! @return   S_OK, if test was successful 
  HRESULT testMiscellaneous() const;

  //! @brief    Test command general procedure tests
  //! @return   S_OK, if test was successful 
  HRESULT testCmdGernalProcuder();

  //! @brief   Test simple commands
  //! @return   S_OK, if test was successful 
  HRESULT testSimpleCommands();

  //! @brief   Test set absolute position
  //! @return   S_OK, if test was successful 
  HRESULT testParameterCommands();

  //! @brief   Test rising and timeout error case of all commands
  //! @return   S_OK, if test was successful 
  HRESULT testRisingAndTimeoutError();

  //! @brief   This method sets a test variant for the client response (async calls).
  //! @param[in]    client      data layer client to execute datalayer access
  //! @param[in]    data        test data for the variant
  void setResponseVariant(test::MockDatalayerClient* const client, const TestDataVariantU16 &data);

  //! @brief   This method sets a test variant for the client response (async calls).
  //! @param[in]    client      data layer client to execute datalayer access
  //! @param[in]    data        test data for the variant
  void setResponseVariantFourBytes(test::MockDatalayerClient* const client, const TestDataVariantU32 & data);

  //! @brief   Test set absolute position init.
  //! @return   S_OK, if test was successful 
  HRESULT testInitSetAbs();

private: //data

  test::FlatbuffersHelperSoE m_fbsHelper{};          //!< Helper class to generate flatbuffers
  const std::string m_profileName = "testProfile";   //!< Name of the test axis profile
};
