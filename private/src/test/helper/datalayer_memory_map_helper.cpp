// ---------------------------------------------------------------------------
//
//! @file       datalayer_memory_map_helper.cpp
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
//! This source file is part of the axisprofile library and declares a
//! helper to generate a valid datalayer memory map for testing.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "datalayer_memory_map_helper.h"


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
using namespace test;


void MemoryMapHelper::addVariable(const char* name, std::uint32_t bitOffset, std::uint32_t bitSize, const std::string& dlType)
{
  flatbuffers::Offset<flatbuffers::String> stringType = m_fbBuilder.CreateString(dlType);

  std::string convName(name);
  auto nameOffset = m_fbBuilder.CreateString(convName);
  comm::datalayer::VariableBuilder variableBuilder(m_fbBuilder);
  variableBuilder.add_name(nameOffset);
  variableBuilder.add_bitoffset(bitOffset);
  variableBuilder.add_bitsize(bitSize);
  variableBuilder.add_type(stringType);
  auto variable = variableBuilder.Finish();
  m_memMapVariables.push_back(variable);
}

comm::datalayer::Variant& MemoryMapHelper::createMemoryMap(std::uint32_t revision)
{
  auto variables = m_fbBuilder.CreateVectorOfSortedTables(&m_memMapVariables);
  comm::datalayer::MemoryMapBuilder memMap(m_fbBuilder);
  memMap.add_revision(revision);
  memMap.add_variables(variables);
  m_fbBuilder.Finish(memMap.Finish());
  m_memoryMap.copyFlatbuffers(m_fbBuilder);
  m_fbBuilder.Clear();

  return m_memoryMap;
}

void MemoryMapHelper::clearMemMapData()
{
  m_memMapVariables.clear();
  m_fbBuilder.Clear();
}
