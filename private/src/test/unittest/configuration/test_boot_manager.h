#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "component/statemachine/statemachine_defs.h"
#include "comm/ethercat/master/fbs/master_state_generated.h"
#include "comm/datalayer/datalayer.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm { namespace axisprofile { class IAxisProfileComponent; } }

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class TestConfigurationBootManager
//! Test class for the boot manager of the axis profile bundle
class TestConfigurationBootManager : public xTST_Test2
{
public:
  //! Constructor
  TestConfigurationBootManager();
  //! Destructor
  virtual ~TestConfigurationBootManager() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Configuration.Bootmanager");
  TEST_DECLARE_DESCRIPTION("Unittest for the boot manager of the axis profile bundle.");
  TEST_DECLARE_TIMEOUT_MINUTES(3);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_RAK);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_PEG);

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

  //! @brief Test all other methods of the test class.
  //! @return  S_OK, when tests were successful
  HRESULT testMiscellaneous();

  //! @brief Test the initialization of the class.
  //! @return  S_OK, when the test was successful.
  HRESULT testInitialization();

  //! @brief Test the method which checks the fieldbus status.
  //! @return  S_OK, when the test was successful.
  HRESULT testCheckFieldbusStatus();

  //! @brief Test the method which checks the device states.
  //! @return  S_OK, when the test was successful.
  HRESULT testCheckDeviceStatus();

  //! @brief Test the method which checks the fieldbus and device status.
  //! @return  S_OK, when the test was successful.
  HRESULT testCheckFieldbusAndDeviceStatus();

  //! @brief Test the method which checks scheduler state.
  //! @return  S_OK, when the test was successful.
  HRESULT testSchedulerCallableInRunState();  

  //! @brief Test the method which executes the internal bootup.
  //! @return  S_OK, when the test was successful.
  HRESULT testInternalBootup();

  //! @brief Test the method of getMaxBootingTimeout
  //! @return  S_OK, when the test was successful.
  HRESULT testGetMaxBootingTimeout();

  //! @brief Test the method of resetAllProfiles
  //! @return  S_OK, when the test was successful.
  HRESULT testResetAllProfiles();

  //! @brief Helper method that waits until the boot thread has executed the statemachine transition.
  //! @param[in]   cmp         Component mockup pointer
  //! @param[in]   expState    Expected state ID
  //! @param[in]   timeoutMS   Timeout in MS
  //! @return  S_OK, when the test was successful.
  HRESULT waitForStateSwitch(comm::axisprofile::IAxisProfileComponent *cmp, comm::axisprofile::StateIDType expState, std::uint32_t timeoutMS);

  //! @brief This is a simple helper method to generate slave state response flatbuffers for testing.
  //! @param[out]  state   master op state
  //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
  void generateMasterStateResponseFlatbuffer(comm::ethercat::master::fbs::EthercatState state, comm::datalayer::Variant* variant);

};
