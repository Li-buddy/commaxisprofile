#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"


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

//! @class TestAxisProfileExternalMockups
//! Test class for external mockups of this bundle.
class TestAxisProfileExternalMockups : public xTST_Test2
{
public:
  //! constructor
  TestAxisProfileExternalMockups();

  //! Destructor
  virtual ~TestAxisProfileExternalMockups() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.ExternalMockups");
  TEST_DECLARE_DESCRIPTION("Unittest of axisprofile external mockups.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief   This method tests the mockup for the system interface.
  //! @return  S_OK, when tests were successful
  HRESULT testSystemMockup();

  //! @brief   This method tests the mockup for the profile manager.
  //! @return  S_OK, when tests were successful
  HRESULT testProfileManagerMockup();

  //! @brief   This method tests the mockup for the axis profile interface.
  //! @return  S_OK, when tests were successful
  HRESULT testAxisProfileMockup();

  //! @brief   This method tests the mockup for the cyclic data interface of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testCyclicDataMockup();

  //! @brief   This method tests the mockup for the command interface of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testCommandMockup();

  //! @brief   This method tests the mockup for the status-/controlword interface of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testStatusControlMockup();

  //! @brief   This method tests the mockup for the diagnostic message interface of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testDiagMsgMockup();

  //! @brief   This method tests the mockup for the device parameter of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testDeviceParamMockup();

  //! @brief   This method tests the mockup for the property interface of the axis profile.
  //! @return  S_OK, when tests were successful
  HRESULT testPropertyMockup();
};

