#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/datalayer/datalayer_system.h"
#include "comm/datalayer/persistence_helper.h"
#include "component/axisprofile_cmp.h"
#include "component/statemachine/i_state_machine.h"
#include "helper/result_helper.h"
#include "mockup/external_bundles/mock_scheduler.h"
#include "profiles/i_axisprofile_internal.h"
#include "stdafx.h"
#include <memory>

#ifndef WIN32
#include "../../thirdparty/valgrind.h"           // some test steps are reduced when valgrind runs to keep test time low
#else
static const bool RUNNING_ON_VALGRIND = false;  ///< when we are in Windows environment, we don't run in valgrind
#endif

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
  class MockEthercatMaster;
}

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

//! @class FuncTestBase
//! Base class for all function tests
class FuncTestBase : public xTST_Test2
{
public: // Methods

  //! Constructor
  FuncTestBase();
  //! Destructor
  virtual ~FuncTestBase() noexcept override;

protected: // Methods

  //! @brief This is the entry method for all function tests. It starts the test environment.
  //! @return Return value indicating success or error.
  HRESULT startTestEnvironment();

  //! @brief This method stops the test environment.
  //! @return Return value indicating success or error.
  HRESULT stopTestEnvironment();

  //! @brief This method executes a single cycle of the axisprofile system. In the real system, this is done by the scheduler.
  //! @return Return value indicating success or error.
  HRESULT executeSingleCycle();

  //! @brief This method adds a new axisprofile to the running system. It does not connect it!
  //! @param[in]    type           Type of the new axis profile
  //! @param[in]    deviceAddress  Address of the connected device.
  //! @param[in]    profileName    Name of the profile
  //! @param[out]   axisProfile    Optional. Pointer to the new axis profile
  //! @return Return value indicating success or error.
  HRESULT addProfileToSystem(const comm::axisprofile::AxisProfileType type, const std::uint32_t deviceAddress, const std::string& profileName, comm::axisprofile::IAxisProfileInternal** axisProfile = nullptr);
    
  /// @brief Update the response source data based on profile type.
  /// @param[in] type   The type fo axisprofile.
  void updateProtocolRelatedMockData(const comm::axisprofile::AxisProfileType type);

  //! @brief This method connects an existing axisprofile.
  //! @param[in]    profileName    Name of the profile
  //! @param[out]   axisProfile    Pointer to the connected profile.
  //! @return Return value indicating success or error.
  HRESULT connectProfile(const std::string& profileName, std::shared_ptr<const IAxisProfileC>& axisProfile);

  //! @brief This method switches the system statemachine to the desired state.
  //! @param[in]   cmdState      Command state that will be used.
  //! @param[in]   targetState   Desired target state that should be set after this method is finished.
  //! @return Return value indicating success or error.
  HRESULT switchSystemStatemachine(const comm::axisprofile::StateIDType cmdState, const comm::axisprofile::StateIDType targetState);

  /// @brief This method waits for a specific result from the invoker function and checks it against the expected result.
  /// @param[in] invoker        Function to be called and checked
  /// @param[in] expRet         Expected return value
  /// @param[in] waitMs         Wait time in MS
  /// @param[in] timeoutMs      Timeout in MS
  /// @return Return value indicating success or error.
  HRESULT waitAndChecker(std::function<comm::axisprofile::AxisProfileResult(void)> invoker, comm::axisprofile::AxisProfileResult expRet, std::uint32_t waitMs = 100, uint32_t timeoutMs = 5000);

  /// @brief This method waits for a specific state from the state getter function and checks it against the expected state.
  /// @tparam STATE             Type of the state/status
  /// @param[in] stateGetter    State getter function to be called and checked
  /// @param[in] expRet         Expected return value
  /// @param[in] waitMs         Wait time in MS
  /// @param[in] timeoutMs      Timeout in MS
  /// @return Return value indicating success or error.
  template<typename STATE>
  HRESULT waitAndStateChecker(std::function<STATE(void)> stateGetter, STATE expRet, std::uint32_t waitMs = 100, uint32_t timeoutMs = 5000)
  {
    std::uint32_t cnt = timeoutMs / waitMs;

    while (cnt > 0)
    {
      auto retValue = stateGetter();
      if (retValue == expRet)
      {
        log(L"<DARK_GREEN><b>success.</b></DARK_GREEN>");
        return S_OK;
      }

      cnt--;
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    error(L"\n%ls:%d Timeout.", LITERALWIDECHAR(__FILE__), __LINE__);
    return S_OK;
  }

  /// @brief returns, if we are running in a valgrind (memcheck) environment
  ///
  /// This method can be called without a test environment started. Use case: checking for valgrind and avoid creating a test environment, when no test should be done.
  ///
  /// @return true, if we are running in a valgrind (memcheck) environment
  static bool isValgrindEnvironment()
  {
    return RUNNING_ON_VALGRIND;
  }

protected: // Data

  comm::datalayer::DatalayerSystem* m_datalayerSystem{};        //!< ServiceDependency - The datalayer system for testing
  comm::datalayer::PersistenceHelper* m_persistenceHelper{};    //!< ServiceDependency - The persistence helper for testing
  comm::axisprofile::AxisProfileCmp* m_axisProfileComponent{};  //!< The AxisProfile component
  test::MockEthercatMaster* m_mockEthercatMaster{};             //!< Mockup of the EthercastMaster bundle
  comm::datalayer::IClient* m_datalayerTestClient{};            //!< Datalayer client that can be used in test
  test::MockScheduler       m_mockScheduler;                    //!< Mockup of the scheduler bundle

private: // data

  bool m_systemRunning = false;                                 //!< Flag that indicates that the system is currently running
};
