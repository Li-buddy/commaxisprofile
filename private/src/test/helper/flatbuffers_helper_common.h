#pragma once

// ---------------------------------------------------------------------------
//
//! @file       flatbuffers_helper_common.h
//!
//! @author     DC-AE/ENG6-CN
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2021
//
// ---------------------------------------------------------------------------
//
//! This header file is part of the axisprofile library and declares a helper
//! to generate different flatbuffers.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/variant.h"

// Generated flatbuffer headers
#include "comm/ethercat/master/fbs/slave_state_generated.h"
#include "comm/ethercat/master/fbs/slave_status_generated.h"
#include "comm/ethercat/master/fbs/master_state_generated.h"
#include "comm/ethercat/master/fbs/master_status_generated.h"
#include "comm/ethercat/master/fbs/dc_slave_config_info_generated.h"

// local defines
#include "fieldbuses/fieldbus_defs.h"

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
  constexpr std::uint32_t MASTER_STATUS_INIT     = 0x00001707;   ///< Master status all bits in error state
  constexpr std::uint32_t MASTER_STATUS_INOP     = 0x00000000;   ///< Master status in OP
  constexpr std::uint32_t MASTER_STATUS_PREOP    = 0x00000026;   ///< Master status in Pre-OP
  constexpr std::uint32_t MASTER_STATUS_NOT_INIT = 0x00000002;   ///< Master status in not initial yet
  constexpr std::uint32_t DEVICE_STATUS_INIT     = 0x00000007;   ///< Device status all bits in error state
  constexpr std::uint32_t DEVICE_STATUS_INOP     = 0x00000000;   ///< Device status in OP

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class FlatbuffersHelperCommon
  //! Helper class to generate common flatbuffers.
  class FlatbuffersHelperCommon
  {
  public: // methods

    //! @brief Constructor
    FlatbuffersHelperCommon() = default;
    //! @brief Destructor
    virtual ~FlatbuffersHelperCommon() noexcept = default;

    //! @brief This is a simple helper method to generate slave state response flatbuffers for testing.
    //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
    void generateSlaveStateResponseFlatbuffer(comm::datalayer::Variant* const variant);

    //! @brief This is a simple helper method to generate master state response flatbuffers for testing.
    //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
    void generateMasterStateResponseFlatbuffer(comm::datalayer::Variant* const variant);

    //! @brief This is a simple helper method to generate slave state request flatbuffers for testing.
    //! @param[in]   variant         Variant address where the created flatbuffer will be stored.
    //! @param[in]   deviceAddress   address of the device
    void generateSlaveStateRequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t deviceAddress = 0);

    //! @brief This is a simple helper method to generate slave status request flatbuffers for testing.
    //! @param[in]   variant         Variant address where the created flatbuffer will be stored.
    //! @param[in]   deviceAddress   address of the device
    void generateSlaveStatusRequestFlatbuffer(comm::datalayer::Variant* const variant, const std::uint16_t deviceAddress = 0);

    //! @brief This is a simple helper method to generate slave status response flatbuffers for testing.
    //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
    void generateSlaveStatusResponseFlatbuffer(comm::datalayer::Variant* const variant);

    //! @brief This is a simple helper method to generate master status response flatbuffers for testing.
    //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
    void generateMasterStatusResponseFlatbuffer(comm::datalayer::Variant* const variant);

    //! @brief This is a simple helper method to generate slave DC config info response flatbuffers for testing.
    //! @param[in]   variant   Variant address where the created flatbuffer will be stored.
    void generateSlaveDcCfgInfoResponseFlatbuffer(comm::datalayer::Variant* const variant);

  public: // Data
    comm::ethercat::master::fbs::EthercatState m_slaveState = comm::ethercat::master::fbs::EthercatState::EthercatState_op;    //!< Return value when slave state is read
    comm::ethercat::master::fbs::EthercatState m_masterState = comm::ethercat::master::fbs::EthercatState::EthercatState_op;   //!< Return value when master state is read

    comm::axisprofile::SlaveStatus   m_slaveStatus {MASTER_STATUS_INOP};    ///< slave status
    comm::axisprofile::MasterStatus  m_masterStatus {DEVICE_STATUS_INOP};   ///< master status

    bool m_dcSupported {true};        ///< is DC supported
    std::uint32_t m_shiftTimeNs {600000};  ///< shift time (sync0), slave is in unit of nanoseconds
    std::uint32_t m_cycleTimeNs {2000000}; ///< cycle time (sync0), slave is in unit of nanoseconds

  private: // Methods

  };
}
