#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_mapping_handler.h
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
//! interface the profile mapping handler.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/i_axisprofile_sharing.h"
#include "mapping_handler_defs.h"
#include "profiles/profile_defs.h"
#include <vector>
#include <set>


// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace datalayer
  {
    struct MemoryMap;
  }
}


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------
    //! This macro prints the given parameter as string.
    #define _CLASS_NAME_(x)   (           # x )
    //! This macro maps the given parameter to a macro that prints it as string.
    #define CLASS_NAME(x)     (_CLASS_NAME_(x))

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------
        
    // ---------------------------------------------------------------------------
    // Forward references
    // ---------------------------------------------------------------------------
    class IAxisProfileInternal;
    
    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class IAxisProfileMappingHandler
    //! This is the interface of the profile mapping handler
    class IAxisProfileMappingHandler :public IAxisProfileSharing
    {
    public: // methods

      //! @brief Destructor
      virtual ~IAxisProfileMappingHandler() noexcept override = default;

      //! @brief This method returns the class name.
      //! @return   Return class name.
      static const char* getClassName(void) { return ( CLASS_NAME(IAxisProfileMappingHandler) ); }

      //! @brief This method is used to map a single input value.
      //! For example you can configure that datalayer address "mybundle/cmd-values/xy" should be used as position value in the axisprofile.
      //! @param[in]   valueID    Identificator of the value (Meaning)
      //! @param[in]   valueURI   Datalayer URI of the value
      //! @return Return value indicating success or error.
      virtual AxisProfileResult configureInputValue(InputMappingValueID valueID, const char* valueURI) = 0;

      //! @brief This method is used to map a vector of input values. It calls the method 'configureInputValue()' of this class to configure
      //! the single inputs.
      //! @param[in]   inputConfig    Vector containing the mapping configuration data
      //! @return Return value indicating success or error.
      virtual AxisProfileResult configureInputValues(const std::vector<InputMappingEntry>& inputConfig) = 0;

      //! @brief This method returns a vector of all current input mappings.
      //! @return Vector of all current input mappings.
      virtual const std::vector< InputMappingEntry> getConfiguredInputs() const = 0;         

      /// @brief This method returns a vector of all available intput mappings.
      /// @return Vector of all current output mappings.
      virtual const std::vector< InputMappingEntry> getAvailableInputs() const = 0;

      //! @brief This method clears the current input mapping configuration.
      //! Important note: Before the invocation of this method will have any effect, you will need to call 'initRealTimeMappingInput()' again!
      virtual void clearInputConfiguration() = 0;

      //! @brief This method returns the URI that is currently mapped to an input value. If no configuration could be found it returns a nullptr.
      //! @param[in]   valueID    Identificator of the value
      //! @return The datalayer URI that is mapped to the input or a nullptr.
      virtual const char* getConfiguredInputURI(InputMappingValueID valueID) const = 0;

      //! @brief This method is used to map a single output value.
      //! For example you can configure that the position value in the axisprofile should be written to the datalayer address "mybundle/cmd-values/xy".
      //! @param[in]   valueID    Identificator of the value (Meaning)
      //! @param[in]   valueURI   Datalayer URI of the value
      //! @return Return value indicating success or error.
      virtual AxisProfileResult configureOutputValue(OutputMappingValueID valueID, const char* valueURI) = 0;

      //! @brief This method is used to map a vector of output values. It calls the method 'configureOutputValue()' of this class to configure
      //! the single outputs.
      //! @param[in]   outputConfig    Vector containing the mapping configuration data
      //! @return Return value indicating success or error.
      virtual AxisProfileResult configureOutputValues(const std::vector<OutputMappingEntry>& outputConfig) = 0;

      //! @brief This method returns a vector of all current output mappings.
      //! @return Vector of all current output mappings.
      virtual const std::vector< OutputMappingEntry> getConfiguredOutputs() const = 0;

      /// @brief This method returns a vector of all available output mappings.
      /// @return Vector of all current output mappings.
      virtual const std::vector< OutputMappingEntry> getAvailableOutputs() const = 0;

      //! @brief This method clears the current output mapping configuration.
      //! Important note: Before the invocation of this method will have any effect, you will need to call 'initRealTimeMappingOutput()' again!
      virtual void clearOutputConfiguration() = 0;

      //! @brief This method returns the URI that is currently mapped to an output value. If no configuration could be found it returns a nullptr.
      //! @param[in]   valueID    Identificator of the value
      //! @return The datalayer URI that is mapped to the output or a nullptr.
      virtual const char* getConfiguredOutputURI(OutputMappingValueID valueID) const = 0;

      //virtual AxisProfileResult setValidConfig(const char* validURI, std::uint32_t value, std::uint32_t mask);

      //! @brief This method reads the variable information from the datalayer RT memory map for every input configuration found in 'm_inputMapping'.
      //! The information is used to initialize the vector 'm_outputMappingRT' that is used for realtime data processing.
      //! @param[in]   memoryMap   The memory map that contains the input variable information
      //! @return Return value indicating success or error.
      virtual AxisProfileResult initRealTimeMappingInput(const comm::datalayer::MemoryMap* memoryMap) = 0;

      //! @brief This method reads the variable information from the datalayer RT memory map for every output configuration found in 'm_outputMapping'.
      //! The information is used to initialize the vector 'm_inputMappingRT' that is used for realtime data processing.
      //! @param[in]   memoryMap   The memory map that contains the output variable information
      //! @return Return value indicating success or error.
      virtual AxisProfileResult initRealTimeMappingOutput(const comm::datalayer::MemoryMap* memoryMap) = 0;

      //! @brief This methods return the 'm_inputMappingRT' vector that contains the realtime information of all input values.
      //! @return Pointer to the vector that contains the realtime information of all input values.
      virtual const std::vector< InputMappingInfoRT>& getInputRealtimeInfo() const = 0;

      //! @brief This methods return the 'm_outputMappingRT' vector that contains the realtime information of all output values.
      //! @return Pointer to the vector that contains the realtime information of all output values.
      virtual const std::vector< OutputMappingInfoRT>& getOutputRealtimeInfo() const = 0;

      //! @brief This method gets profile variable assigned information
      //! @return     Return the assigned information of all profile variable.
      virtual const ProfileAssignedInfo& getProfileAssignedInfo() const = 0;

      //! @brief This method is used to map an input item type.
      //! @param[in]   valueID      Identificator of the value (Meaning)
      //! @param[in]   mappingGroup  mapping group
      //! @return      Return value indicating success or error.
      virtual AxisProfileResult configureInputCfgInfo(InputMappingValueID valueID, MappingCfgInfo mappingGroup) = 0;
      
      //! @brief This method is used to map an output item type.
      //! @param[in]   valueID      Identificator of the value (Meaning)
      //! @param[in]   mappingGroup  mapping group
      //! @return      Return value indicating success or error.
      virtual AxisProfileResult configureOutputCfgInfo(OutputMappingValueID valueID, MappingCfgInfo mappingGroup) = 0;

      /// @brief This method check name consistence with device name and check name duplication from input information.
      /// @param[in]     pProfile      Current profile pointer.
      /// @param[in,out] nCount        The reference counting of current item, each item can be mapped only once.
      /// @param[in,out] setMapping    The input mapping item which has been mapped.
      /// @return                      Return value indicating success or error.
      virtual AxisProfileResult checkInputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping) = 0;

      /// @brief This method check name consistence with device name and check name duplication from output information.
      /// @param[in]     pProfile      Current profile pointer.
      /// @param[in,out] nCount        The reference counting of current item, each item can be mapped only once.
      /// @param[in,out] setMapping    The output mapping item which has been mapped.
      /// @return                      Return value indicating success or error.
      virtual AxisProfileResult checkOutputMappingItems(IAxisProfileInternal* const pProfile, size_t& nCount, std::set<std::string>& setMapping) = 0;
    };
  }
}
