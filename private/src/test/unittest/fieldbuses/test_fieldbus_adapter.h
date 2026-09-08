#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------


// Test Class
#include "stdafx.h"
#include "fieldbuses/fieldbus_adapter.h"
#include "helper/device_fbs_data_helper.h"

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

/// @brief Test class for the fieldbus adapter base class.
class TestFieldbusAdapter : public xTST_Test2
{
public:
  /// c'tor
  TestFieldbusAdapter();
  /// Destructor
  virtual ~TestFieldbusAdapter() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Fieldbuses.AdapterBase");
  TEST_DECLARE_DESCRIPTION("Unittest of the fieldbus adapter base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  /// @brief Init function of the test.
  /// @return S_OK, when initialization successful. 
  HRESULT doInit();

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief End function of the test.
  /// @return S_OK, when deinitialization successful.
  HRESULT doEnd();

private: // Methods

  /// @brief    Test of methods to read/write a parameter asynchronous.
  /// @return   S_OK, if test was successful 
  HRESULT testReadWriteParamAsync();

  /// @brief    Test of methods to read/write a parameter synchronous.
  /// @return   S_OK, if test was successful 
  HRESULT testReadWriteParamSync();

  /// @brief Test all other methods of the test class.
  /// @return  S_OK, when tests were successful
  HRESULT testSlaveInfoFbs();

  /// @brief Test all other methods of the test class.
  /// @return  S_OK, when tests were successful
  HRESULT testSlaveAddressMappingFbs();

  /// @brief Test all other methods of the test class.
  /// @return  S_OK, when tests were successful
  HRESULT testMiscellaneous();

  /// @brief Test master status.
  /// @return  S_OK, when tests were successful
  HRESULT testMasterStatus();

  /// @brief Test slave status.
  /// @return  S_OK, when tests were successful
  HRESULT testSlaveStatus();

  /// @brief Test slave distribution clock config information.
  /// @return  S_OK, when tests were successful
  HRESULT testReadSlaveDcCfgInfo();

  /// @brief Test parameter list and raw data conversion methods.
  /// @return  S_OK, when tests were successful
  HRESULT testParamListConversion();

  /// @brief Generate slave info
  /// @param[in,out] variantCfgedSlaveInfo The variant data about cfg.
  /// @param[in] slaveName                 The mock slave name.
  /// @param[in] protocol                  The protocol type.
  void generateCfgSlaveInfo(comm::datalayer::Variant& variantCfgedSlaveInfo, std::string& slaveName, std::uint32_t protocol);

private: // Types

  /// @class FieldbusAdapterUnittest
  /// Specialization of the test class to access protected members and make it creatable.
  class FieldbusAdapterUnittest : public comm::axisprofile::FieldbusAdapter
  {
  public: // Methods

    /// @brief Constructor
    /// @param[in]   trace   Pointer to the profile trace
    explicit FieldbusAdapterUnittest(comm::axisprofile::ProfileTrace* trace) : comm::axisprofile::FieldbusAdapter(trace) {}

    /// @brief Destructor
    virtual ~FieldbusAdapterUnittest() = default;

    comm::axisprofile::AxisProfileResult getParamResponseData(const comm::datalayer::Variant* /*variantData*/, std::uint8_t* /*bufPtr*/, std::size_t /*bufLen*/, std::size_t& /*readLen*/) final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult readSingleParamAsync(comm::axisprofile::ParameterResponseCallbackData* const responseCallbackData, const comm::axisprofile::ParamReqDataBase& /*readData*/) final
    {
      return FieldbusAdapter::executeDatalayerReadAsync(responseCallbackData, m_dummyAddress);
    }

    comm::axisprofile::AxisProfileResult writeSingleParamAsync(comm::axisprofile::ParameterResponseCallbackData* const responseCallbackData, const comm::axisprofile::ParamReqDataBase& /*writeData*/) final
    {
      return FieldbusAdapter::executeDatalayerWriteAsync(responseCallbackData, m_dummyAddress);
    }

    comm::axisprofile::AxisProfileResult readSingleParamSync(const comm::axisprofile::ParamReqDataBase& /*readData*/, std::uint8_t* /*bufPtr*/, std::size_t /*bufLen*/, std::size_t& /*readLen*/) final
    {
      return FieldbusAdapter::executeDatalayerReadSync(nullptr, m_dummyAddress);
    }

    comm::axisprofile::AxisProfileResult writeSingleParamSync(const comm::axisprofile::ParamReqDataBase& /*writeData*/) final
    {
      return FieldbusAdapter::executeDatalayerWriteSync(nullptr, m_dummyAddress);
    }

    comm::axisprofile::AxisProfileResult readSlaveStateSync(const comm::axisprofile::ReadSlaveStateDataBase& /*readData*/, std::string* /*state*/) final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

    comm::axisprofile::AxisProfileResult readFieldbusMasterStateSync(std::string* /*state*/) final
    {
      return comm::axisprofile::AxisProfileResult::STS_OK;
    }

  private: // Data

    std::string m_dummyAddress = "";   ///< Dummy address string
  };
};

