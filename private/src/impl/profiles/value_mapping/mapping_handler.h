#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mapping_handler.h
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
//! This header file is part of the axisprofile library and implements the
//! mapping of internal values to datalayer RT values and vice versa.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_mapping_handler.h"
#include "profiles/data_sharing/i_sharing_handler.h"
#include "profiles/category/i_category_adapter.h"
#include <map>
#include <string>


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------
    class ProfileTrace;

    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class AxisProfileMappingHandler
    //! This is the class for the handling of the value mapping to dlrt.
    class AxisProfileMappingHandler : public IAxisProfileMappingHandler
    {
    public: // methods

      //! @brief Constructor
      /// @param[in]  profileBasicInfo   Struct containing all basic information for the profile.
      explicit AxisProfileMappingHandler(ProfileBasicInfo& profileBasicInfo);

      //! @brief Destructor
      virtual ~AxisProfileMappingHandler() noexcept override;

      AxisProfileResult configureInputValue(InputMappingValueID valueID, const char* valueURI) override;
      AxisProfileResult configureInputValues(const std::vector<InputMappingEntry>& inputConfig) override;
      const std::vector< InputMappingEntry> getConfiguredInputs() const override;
      const std::vector< InputMappingEntry> getAvailableInputs() const override;
      void clearInputConfiguration() override;
      const char* getConfiguredInputURI(InputMappingValueID valueID) const override;
      AxisProfileResult configureOutputValue(OutputMappingValueID valueID, const char* valueURI) override;
      AxisProfileResult configureOutputValues(const std::vector<OutputMappingEntry>& outputConfig) override;
      const std::vector< OutputMappingEntry> getConfiguredOutputs() const override;
      const std::vector< OutputMappingEntry> getAvailableOutputs() const override;
      void clearOutputConfiguration() override;
      const char* getConfiguredOutputURI(OutputMappingValueID valueID) const override;
      AxisProfileResult initRealTimeMappingInput(const comm::datalayer::MemoryMap* memoryMap) override;
      AxisProfileResult initRealTimeMappingOutput(const comm::datalayer::MemoryMap* memoryMap) override;
      const std::vector< InputMappingInfoRT>& getInputRealtimeInfo() const override;
      const std::vector< OutputMappingInfoRT>& getOutputRealtimeInfo() const override;
      const ProfileAssignedInfo& getProfileAssignedInfo() const override;
      AxisProfileResult configureInputCfgInfo(InputMappingValueID valueID, MappingCfgInfo mappingGroup) override;
      AxisProfileResult configureOutputCfgInfo(OutputMappingValueID valueID, MappingCfgInfo mappingGroup) override;
      AxisProfileResult checkInputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping) final;
      AxisProfileResult checkOutputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping) final;

    protected:

      void setSharingHandler(IAxisProfileSharingHandler* handler) final;

    private: // method

      //! @brief Determine profile variables if cyclic parameter is assigned in input profile variable
      //! @return   Return value indicating success or error.
      AxisProfileResult determineInputAssigned();

      //! @brief Determine profile variables if cyclic parameter is assigned in output profile variable
      //! @return   Return value indicating success or error.
      AxisProfileResult determineOutputAssigned();

      //! @brief Template to configure mapping items
      //! @param[in]   valueID      value ID.
      //! @param[in]   mappingValue mapping value which is mapped with value id.
      //! @param[out]  mappingMap   map which contains the general mapping data.
      //! @return      Return value indicating success or error.
      template <typename T_ValueID, typename T_MappingValue, typename T_Map>
      AxisProfileResult mappingConfig(T_ValueID valueID, T_MappingValue mappingValue, T_Map& mappingMap);

    private: // data

      ProfileTrace*        m_profileTrace{};    ///< Pointer to the profile trace
      AxisProfileType      m_axisProfileType;   ///< Type of the profile
      //ICategoryAdapter*&   m_profileCategory;  ///< Reference to profile category pointer

      //! Map that contains the general input mapping. This container is only used in non-realtime context.
      //! Key:   Input value ID (Meaning)
      //! Value: Datalayer URI
      std::map< InputMappingValueID, std::string> m_inputMapping{};

      std::map< InputMappingValueID, MappingCfgInfo> m_inputMappingCfgInfo {};  //Map which stored mapping group and required information with value ID

      //! Map that contains the general output mapping. This container is only used in non-realtime context.
      //! Key:   Output value ID (Meaning)
      //! Value: Datalayer URI
      std::map< OutputMappingValueID, std::string> m_outputMapping{};

      std::map< OutputMappingValueID, MappingCfgInfo> m_outputMappingCfgInfo{}; //Map which stored mapping group and required information with value ID

      //! Vector that contains the realtime information of all currently mapped input values. This container is used in realtime context.
      std::vector< InputMappingInfoRT> m_inputMappingRT{};
      //! Vector that contains the realtime information of all currently mapped output values. This container is used in realtime context.
      std::vector< OutputMappingInfoRT> m_outputMappingRT{};

      ProfileAssignedInfo m_profileAssignedInfo{ false }; ///< Check if profile variable is assigned
      IAxisProfileSharingHandler* m_sharingHandler{};    ///< mediator for sharing data 
    };
  }
}
