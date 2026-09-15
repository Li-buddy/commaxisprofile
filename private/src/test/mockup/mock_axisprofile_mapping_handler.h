#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_axisprofile_mapping_handler.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2018
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a mockup
//! of the axisprofile mapping handler.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "profiles/value_mapping/i_mapping_handler.h"

// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace test
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockAxisProfileMappingHandler
  //! Mockup of the axisprofile mapping handler.
  class MockAxisProfileMappingHandler : public comm::axisprofile::IAxisProfileMappingHandler
  {
  public:
    //! @brief constructor
    MockAxisProfileMappingHandler() = default;
    //! @brief destructor
    virtual ~MockAxisProfileMappingHandler() noexcept override = default;

    //! @brief This method is used to map a single input value.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult configureInputValue(comm::axisprofile::InputMappingValueID /*valueID*/, const char* /*valueURI*/) override { return m_returnValue; }

    //! @brief This method is used to map a single input value.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult configureInputValues(const std::vector<comm::axisprofile::InputMappingEntry>& /*inputConfig*/) override { return m_returnValue; }

    //! @brief This method returns a vector of all current input mappings.
    //! @return Vector of all current input mappings.    
    const std::vector< comm::axisprofile::InputMappingEntry> getConfiguredInputs() const override { return {}; }

    //! @brief This method returns a vector of all available input mappings.
    //! @return Vector of all available input mappings.    
    const std::vector< comm::axisprofile::InputMappingEntry> getAvailableInputs() const override { return {}; }

    //! @brief This method clears the current input mapping configuration.   
    void clearInputConfiguration() override {}

    //! @brief This method returns the URI that is currently mapped to an input value. If no configuration could be found it returns a nullptr.
    //! @return The datalayer URI that is mapped to the input or a nullptr.
    const char* getConfiguredInputURI(comm::axisprofile::InputMappingValueID /*valueID*/) const override { return nullptr; }

    //! @brief This method is used to map a single output value.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult configureOutputValue(comm::axisprofile::OutputMappingValueID /*valueID*/, const char* /*valueURI*/) override { return m_returnValue; }

    //! @brief This method is used to map a single output value.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult configureOutputValues(const std::vector<comm::axisprofile::OutputMappingEntry>& /*outputConfig*/) override { return m_returnValue; }

    //! @brief This method returns a vector of all current output mappings.
    //! @return Vector of all current output mappings.    
    const std::vector< comm::axisprofile::OutputMappingEntry> getConfiguredOutputs() const override { return {}; }

    //! @brief This method returns a vector of all available output mappings.
    //! @return Vector of all available output mappings.    
    const std::vector< comm::axisprofile::OutputMappingEntry> getAvailableOutputs() const override { return {}; }

    //! @brief This method clears the current output mapping configuration. 
    void clearOutputConfiguration() override {}

    //! @brief This method returns the URI that is currently mapped to an output value. If no configuration could be found it returns a nullptr.
    //! @return The datalayer URI that is mapped to the output or a nullptr.
    const char* getConfiguredOutputURI(comm::axisprofile::OutputMappingValueID /*valueID*/) const override { return nullptr; }


    //! @brief This method reads the variable information from the datalayer RT memory map for every input configuration found in 'm_inputMapping'.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult initRealTimeMappingInput(const comm::datalayer::MemoryMap* /*memoryMap*/) override { return m_returnValue; }

    //! @brief This method reads the variable information from the datalayer RT memory map for every output configuration found in 'm_outputMapping'.
    //! @return Return value indicating success or error.
    comm::axisprofile::AxisProfileResult initRealTimeMappingOutput(const comm::datalayer::MemoryMap* /*memoryMap*/) override { return m_returnValue; }

    //! @brief This methods return the 'm_inputMappingRT' vector that contains the realtime information of all input values.
    //! @return Pointer to the vector that contains the realtime information of all inpute values.
    const std::vector< comm::axisprofile::InputMappingInfoRT>& getInputRealtimeInfo() const override { static std::vector< comm::axisprofile::InputMappingInfoRT> dummy{}; return dummy; }

    //! @brief This methods return the 'm_outputMappingRT' vector that contains the realtime information of all output values.
    //! @return Pointer to the vector that contains the realtime information of all output values.
    const std::vector< comm::axisprofile::OutputMappingInfoRT>& getOutputRealtimeInfo() const override { static std::vector< comm::axisprofile::OutputMappingInfoRT> dummy{}; return dummy; }

    comm::axisprofile::AxisProfileResult configureInputCfgInfo(comm::axisprofile::InputMappingValueID /*valueID*/, comm::axisprofile::MappingCfgInfo /*mappingGroup*/) override { return m_returnValue; }

    comm::axisprofile::AxisProfileResult configureOutputCfgInfo(comm::axisprofile::OutputMappingValueID /*valueID*/, comm::axisprofile::MappingCfgInfo /*mappingGroup*/) override { return m_returnValue; }

    /// @brief This method checks whether profile variable is assigned
    /// @return     Return the assigned information of all profile variable.
    const comm::axisprofile::ProfileAssignedInfo& getProfileAssignedInfo() const override { return m_profileAssignedInfo; }
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* /*handler*/)override {};
    comm::axisprofile::ProfileAssignedInfo& getProfileAssignedInfo() { return m_profileAssignedInfo; }
    comm::axisprofile::AxisProfileResult checkInputMappingItems(comm::axisprofile::IAxisProfileInternal* const /*pProfile*/, size_t& /*nCount*/, std::set<std::string>& /*setMapping*/)override { return m_returnValue; }
    comm::axisprofile::AxisProfileResult checkOutputMappingItems(comm::axisprofile::IAxisProfileInternal* const /*pProfile*/, size_t& /*nCount*/, std::set<std::string>& /*setMapping*/)override { return m_returnValue; }
    
    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions by changing the returned value.
    //! @param[in]    result   New return code
    void setReturnValue(comm::axisprofile::AxisProfileResult result) { m_returnValue = result; }

    /// @brief This method can be used by tests to change the behavior of the member functions by changing the returned value.
    /// @param[in]    result   New return code
    void setReturnValue(const comm::axisprofile::ProfileAssignedInfo& result) { m_profileAssignedInfo = result; }

    /// @brief This method can be used to by tests to set mapping type for mapping items
    /// @param[in]    mappingGroup Mapping type to be set
    void setMappingType(comm::axisprofile::MappingGroup mappingGroup) { m_mappingType = mappingGroup; }

  protected: // data

    comm::axisprofile::AxisProfileResult m_returnValue = comm::axisprofile::AxisProfileResult::STS_OK; ///< Result of member functions
    comm::axisprofile::ProfileAssignedInfo m_profileAssignedInfo{ false };                               ///< Check if profile variable is assigned 
    comm::axisprofile::MappingGroup m_mappingType{ comm::axisprofile::MappingGroup::BASE };               ///< Mapping type
  };
}
