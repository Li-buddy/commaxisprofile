#pragma once
/// @file
///
/// This header file is part of the axisprofile library and implements the
/// data sharing handler between every profile sub-module.
///
/// SPDX-FileCopyrightText: Copyright (c) 2018 - 2025 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "i_sharing_handler.h"
#include <array>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestSharingHandler;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile
{
  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------
  /// @brief Type of share interface
  using ShareITF = std::array<IAxisProfileSharing*, SHARE_ITF_TYPE_CNT>;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief This is the class for the handling of the data sharing handler between every profile sub-module
  class AxisProfileSharingHandler : public IAxisProfileSharingHandler
  {
  public: // methods

    /// @brief Constructor
    /// @param[in]   trace     Pointer to the profile trace
    explicit AxisProfileSharingHandler(ProfileTrace* trace);

    /// @brief Destructor
    ~AxisProfileSharingHandler() noexcept override = default;

    AxisProfileResult registSharingITF(SharingInterfaceType itfType, IAxisProfileSharing* const sharingITF) final;
    void removeSharingITF(SharingInterfaceType itfType) final;

    IAxisProfileDeviceParamInternal*   getDeviceParamIF()         const final;
    IAxisProfileMappingHandler*        getMappingHandlerIF()      const final;
    IAxisProfileCommandInternal*       getCommandIF()             const final;
    IAxisProfileCyclicDataInternal*    getCyclicDataIF()          const final;
    IAxisProfileStatusControlInternal* getStatusControlIF()       const final;
    IAuxJobHandler*                    getAuxJobHandlerIF()       const final;
    IAxisProfileParamAccess*           getParamAccessIF()         const final;
    IJobMgr*                           getJobMgrIF()              const final;
    IScaling*                          getScalingIF()             const final;
    ICategoryAdapter*                  getCategoryIF()            const final;
    IAxisProfilePropertyInternal*      getPropertyIF()            const final;
    IAxisProfileCmdInternal*           getCmdIF()                 const final;  

  private:

    /// @brief This function template support return detail interface
    /// @tparam T         The class type
    /// @param[in] type   The interface type
    /// @return T         The specified type interface
    template <typename T>
    T getInternalITF(SharingInterfaceType type) const
    {
      if (type > SharingInterfaceType::ITF_TYPE_MAX)
      {
        return nullptr;
      }

      return static_cast<T>(m_sharingITFs[static_cast<std::uint16_t>(type)]);
    }

  private: // data

    friend TestSharingHandler;     // for unit test
    ProfileTrace* m_profileTrace;  ///< Pointer to the profile trace
    ShareITF m_sharingITFs;        ///< array used to store interface (only one interface for one type)
  };
}
