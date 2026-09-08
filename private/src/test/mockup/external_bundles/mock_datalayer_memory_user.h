#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mock_datalayer_memory_user.h
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
//! of the datalayer memory user interface.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "comm/datalayer/datalayer_rt.h"


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

  //! @class MockMemoryUser
  //! Helper class to simulate a datalayer realtime memory user in test
  class MockMemoryUser : public comm::datalayer::IMemoryUser
  {
  public:

    //! @brief ReturnValue identifier for the mockup of the memory user
    enum ReturnValueIdentifier
    {
      BEGIN_ACCESS = 0,  //!< ID of return value for beginAccess
      END_ACCESS,        //!< ID of return value for endAccess
      GET_SIZE,          //!< ID of return value for getSize
      GET_MEMMAP         //!< ID of return value for getMemoryMap
    };

    //! @brief MockMemoryUser
    MockMemoryUser(size_t memorySize) : m_sizeReturn(memorySize)
    {
      m_dataMemory = new std::uint8_t[ memorySize ];
    }
    virtual ~MockMemoryUser()
    {
      delete[] m_dataMemory;
    }

    //! @brief all member functions are realtime capable
    comm::datalayer::DlResult beginAccess(std::uint8_t*& data, std::uint32_t /*revision*/) override { data = m_dataMemory; return m_returnResult[ReturnValueIdentifier::BEGIN_ACCESS]; }

    //! @brief all member functions are realtime capable
    comm::datalayer::DlResult endAccess() override { return m_returnResult[ReturnValueIdentifier::END_ACCESS]; }

    //! @brief all member functions are realtime capable
    comm::datalayer::DlResult getSize(size_t& sizeBytes) override { sizeBytes = m_sizeReturn; return m_returnResult[ReturnValueIdentifier::GET_SIZE]; }

    //! @brief all member functions are realtime capable
    comm::datalayer::DlResult getType(comm::datalayer::MemoryType& /*type*/) override { return comm::datalayer::DlResult::DL_UNSUPPORTED; }

    //! @brief all member functions are realtime capable
    comm::datalayer::DlResult getMemoryMap(comm::datalayer::Variant &memMap) override
    {
      if ( m_memMap )
      {
        memMap = *m_memMap;
      }
      return m_returnResult[ ReturnValueIdentifier::GET_MEMMAP ];
    }

    //! @brief mock setMemoryMap
    void setMemoryMap(comm::datalayer::Variant* memMap) { m_memMap = memMap; }

    // -------------------------------- Test methods ------------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    IdReturnValue   Identifies the method for which the return value should be changed
    //! @param[in]    result          New return code
    void setReturnValue(ReturnValueIdentifier IdReturnValue,comm::datalayer::DlResult result) { m_returnResult[IdReturnValue] = result; }

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    sizeBytes   The value that will be returned by getSize()
    void setReturnSize(size_t sizeBytes) { m_sizeReturn = sizeBytes; }

  private:
    std::uint8_t *m_dataMemory = nullptr;                    //!< Data memory pointer
    comm::datalayer::Variant* m_memMap = nullptr;       //!< Pointer to memory map variant
    size_t m_sizeReturn;                                //!< Size which will be returned by getSize()
    comm::datalayer::DlResult m_returnResult[ 4 ] = { comm::datalayer::DlResult::DL_OK };   //!< Results of member functions
  };
}


