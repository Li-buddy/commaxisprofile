#pragma once

// ---------------------------------------------------------------------------
//
//! @file       i_axisprofile_manager_internal.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axis profile library and declares the internal
//! interface of the axis profile manager.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/interfaces/i_axisprofile_manager.h"
#include "axisprofile_internal_defs.h"
#include "profiles/i_axisprofile_internal.h"
#include <vector>
#include <string>
#include <map>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
namespace comm
{
  namespace datalayer
  {
    class IDataLayerFactory;
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
    // Forward references
    // ---------------------------------------------------------------------------
    class IAxisProfile;
    class IFieldbusMaster;
    class IBackgroundThread;
    struct ProfileInfo;

    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------
    /// @brief Type of container of fieldbus instance name and its object
    using ContainerFieldbusInst = std::map<std::string, IFieldbusMaster*>;

    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class IAxisProfileManagerInternal
    //! This is the internal interface of the axisprofile manager.
    class IAxisProfileManagerInternal : public IAxisProfileManager
    {
    public: // methods
      virtual ~IAxisProfileManagerInternal() noexcept override = default;

      //! @brief This method calls the processDataAT() methods of all registered axisprofiles.
      //!        The received AT telegram is taken and all the information is copied to the internal
      //!        data structures. This has to be done at the beginning of every motion cycle.
      //!
      //! @return      Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult processDataAt() = 0;

      //! @brief This method calls the processDataMDT() methods of all registered axisprofiles.
      //!        The MDT telegram which has to be send is prepared by copying the information from the
      //!        internal data structures to the telegram data container. This has to be done at the end
      //!        of every motion cycle.
      //!
      //! @return      Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult processDataMdt() = 0;

      //! @brief Set the datalayer pointer for the axis profile manager
      //! @param[in]  datalayer  new pointer to the data layer
      //! @param[in]  bkThread   background thread
      virtual void setDataLayer(comm::datalayer::IDataLayerFactory* datalayer, IBackgroundThread* bkThread = nullptr) = 0;

      //! @brief This method initializes the axis profile manager.
      //! @param[in]   abortInitialization   Flag indicating if the initialization should be aborted (e.g. used to exit the infinite while loop)
      //! @param[in]   cycleTime  System cycle time set to profile
      //! @return  Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult initManager(const bool& abortInitialization, double cycleTime) = 0;

      //! @brief This method searches for an existing profile by the given name.
      //! @param[in]   profileName   Name of the profile.
      //! @return  Pointer to the profile or nullptr if not found.
      virtual IAxisProfile* getProfileByName(const char* profileName) = 0;

      //! @brief This method returns a vector of the names of all current profiles.
      //! @param[out] profileNames        list of current profile names
      virtual void getProfileNames(std::vector<std::string> &profileNames) = 0;

      //! @brief This method sets the address of the input buffer for the given profile.
      //! @param[in]   profileName      Name of the profile.
      //! @param[in]   inputBufferURI   Address of the input buffer.
      //! @return  Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult setProfileInputBuffer(const char* profileName, const char* inputBufferURI) = 0;

      //! @brief This method returns the currently configured input buffer of the requested axis profile.
      //! @param[in]   profileName   The name of the profile for which the input buffer URI should be returned
      //! @return The URI of the input buffer of the requested profile or a nullptr if the profile could not be found.
      virtual const char* getProfileInputBuffer(const char* profileName) = 0;

      //! @brief This method sets the address of the output buffer for the given profile.
      //! @param[in]   profileName       Name of the profile.
      //! @param[in]   outputBufferURI   Address of the output buffer.
      //! @return  Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult setProfileOutputBuffer(const char* profileName, const char* outputBufferURI) = 0;

      //! @brief This method returns the currently configured output buffer of the requested axis profile.
      //! @param[in]   profileName   The name of the profile for which the output buffer URI should be returned
      //! @return The URI of the output buffer of the requested profile or a nullptr if the profile could not be found.
      virtual const char* getProfileOutputBuffer(const char* profileName) = 0;

      //! @brief This method gets pointer to get the state of statemachine state
      virtual const StateMachineState* &getStateMachineStatusIF() = 0;

      /// @brief  This method executes auxiliary joblist. Functions in joblist will be called cyclically in RT-Thread at end of each RT call.
      ///         This function will always run when the system state is SETUP or OPERATION .
      ///         if job list is empty,this function will do nothing.
      /// @return Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult processAuxJobs() = 0;
            
      /// @brief This method gets the fieldbus instances name and its object.
      /// @param[in]   mapInstances   The fieldbus instances container
      virtual void getFieldbusInstances(ContainerFieldbusInst& mapInstances) = 0;

      /// @brief Reset FPU error flag member inside all profile and the cpu.
      /// @return Return value indicating success (>=0) or error (<0).
      virtual AxisProfileResult resetAllFPUFlag() = 0;

      /// @brief This method runs all jobs of all profiles.
      virtual void runAllJobs() = 0;

      /// @brief Execute all check callback functions registered by external bundle.
      /// @return  Return value indicating success or error.
      virtual AxisProfileResult execExternalChecking() = 0;

      /// @brief Get all activated profile.
      /// @param[out] profiles  Holds all activated profiles
      virtual void getActivatedProfiles(std::vector<IAxisProfile*> &profiles) = 0;

      /// @brief Collect necessary information about device, topology.
      /// @return  Return value indicating success or error.
      virtual AxisProfileResult collectAllDeviceInfo(const bool& abortInitialization) = 0;

      /// @brief This method will check the consitency between profile configuration and device actual data.
      /// @return Return value indicating success or error.
      virtual AxisProfileResult checkConsistency() = 0;
    };
  }
}
