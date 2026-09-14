#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and defines CanOpen set operation mode job.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "canopen_job_base.h"
#include "axisprofile_internal_defs.h"
#include "profiles/profile_defs.h"

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::canopen
{
  /// @brief This is class of job object of CANOpen set operation mode.
  class CanopenJobSetOpMode : public CanopenJobBase
  {
  public: // methods

    /// @brief Constructor
    /// @param[in] profileInfo    Reference to the Profile Basic Information
    /// @param[in] shareHandle    Pointer to the sharing handler
    explicit CanopenJobSetOpMode(ProfileBasicInfo& profileInfo,
                                 IAxisProfileSharingHandler* shareHandle = nullptr,
                                 const AcknowledgeTelegram* telegramAt = nullptr,
                                 MasterDataTelegram* telegramMdt = nullptr);

    /// @brief Destructor
    virtual ~CanopenJobSetOpMode() noexcept override = default;

    /// @brief This static method returns job name
    /// @return job name
    static const char* jobName() { return "CanopenJobSetOpMode"; }

    JobIDType getID() final { return jobName(); }

  protected: // methods

    AxisProfileResult onSetArgument(IContainer* arg) final;
    AxisProfileResult onJobEnterActivating() final;
    AxisProfileResult onJobActivating() final;
    AxisProfileResult onJobEnterAborting() final;

  private: // data

    bool checkOpModeAvailable(AvailabilityInfo supported, bool assignedInput, bool assignedOutput,
                              OpModeErrorInfo& errorInfo) const;
    bool checkOperationModeSupported(AxisProfileOperationMode opMode, OpModeErrorInfo& errorInfo) const;
    AxisProfileResult determineCanopenOpMode(AxisProfileOperationMode opMode, std::uint8_t& mode) const;
    AxisProfileResult writeModeToPdo();
    AxisProfileResult writeModeBySdo();
    AxisProfileResult verifyOperationMode();

    AxisProfileOperationMode m_requestedOpMode {AxisProfileOperationMode::UNDEFINED};  ///< Requested operation mode
    std::uint8_t m_requestedCiaMode {};                                                ///< Converted CiA-402 mode
    std::uint8_t m_feedbackCiaMode {};                                                 ///< Mode read back from the drive
    bool m_outputModeMapped {false};                                                    ///< True when operation mode is PDO-mapped
    AsyncCommandStatemachine m_readOpModeReq {};                                       ///< Async read state
    AsyncCommandStatemachine m_writeOpModeReq {};                                      ///< Async write state
    const AcknowledgeTelegram* m_telegramAT {};                                        ///< AT telegram
    MasterDataTelegram* m_telegramMDT {};                                              ///< MDT telegram
  };
}
