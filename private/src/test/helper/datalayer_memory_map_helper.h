#pragma once

// ---------------------------------------------------------------------------
//
//! @file       datalayer_memory_map_helper.h
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
//! This header file is part of the axisprofile library and declares a
//! helper to generate a valid datalayer memory map for testing.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"
#include "flatbuffers/flatbuffers.h"
#include "comm/datalayer/memory_map_generated.h"
#include "comm/datalayer/variant.h"
#include <vector>


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

  //! @class MemoryMapHelper
  //! Helper class to simulate the datalayer memory map.
  class MemoryMapHelper 
  {
  public: // methods

    //! @brief Constructor
    MemoryMapHelper() = default;
    //! @brief Destructor
    virtual ~MemoryMapHelper() noexcept = default;

    //! @brief This method adds a new variable to the vector that is used to create the memory map when
    //!        createMemoryMap() gets executed.
    //! @param[in]   name        The name of the new variable
    //! @param[in]   bitOffset   The bit offset
    //! @param[in]   bitSize     The bit size
    //! @param[in]   dlType      The datalayer variable type string (can be found in datalayer_types.h)
    void addVariable(const char* name, std::uint32_t bitOffset, std::uint32_t bitSize, const std::string& dlType);

    //! @brief This method creates the memory map by using all variables stored in the variable vector.
    //! @param[in]   revision   Revision used for the CreateMemoryMapDirect() call
    //! @return   Reference to the created memory map
    comm::datalayer::Variant& createMemoryMap(std::uint32_t revision);

    //! @brief This method clears the internal data of this class. Always invoke this method before starting
    //!        the configuration of a new memory map.
    void clearMemMapData();

  private: // data

    std::vector<flatbuffers::Offset<comm::datalayer::Variable>> m_memMapVariables{};   //!< Vector containing all memory map variables
    flatbuffers::FlatBufferBuilder m_fbBuilder{};                                      //!< Flatbuffer builder
    comm::datalayer::Variant m_memoryMap{};                                            //!< The final memory map
  };
}
