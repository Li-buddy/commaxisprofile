#pragma once

/// @file
///
/// @brief This headerfile is part of the axisprofile library and declaresthe CategoryMappingHelper.
///
/// SPDX-FileCopyrightText: Copyright (c) 2023 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "comm/axisprofile/defines/axisprofile_statusdefs.h"
#include "profiles/value_mapping/mapping_handler_defs.h"
#include <vector>

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
  /// @brief This class used for different profile category mapping item management.
  class CategoryMappingHelper
  {
  public:// methods

    /// @brief Constructor
    CategoryMappingHelper() = default;

    /// @brief Destructor
    virtual ~CategoryMappingHelper() noexcept = default;

    /// @brief This method get available input mapping items for different profile category.
    /// @param[in]  category      Current profile category
    /// @param[out] vecInputItems The vector container
    /// @return Return value indicates success or error.
    static AxisProfileResult getAvailableInputMappingItems(const AxisProfileCategory category, std::vector<InputMappingValueID>& vecInputItems);

    /// @brief This method get available output mapping items for different profile category.
    /// @param[in]  category       Current profile category
    /// @param[out] vecOutputItems The vector container
    /// @return Return value indicates success or error.
    static AxisProfileResult getAvailableOutputMappingItems(const AxisProfileCategory category, std::vector<OutputMappingValueID>& vecOutputItems);

    /// @brief This method check whether the specific input mapping item is required.
    /// @param[in] category     The category of profile.
    /// @param[in] profileType  The protocol type of profile.
    /// @param[in] inputItem   The specific input item.
    /// @return result indicate the item is required or not.
    static bool checkInputMappingItemRequired(const AxisProfileCategory category, const AxisProfileType profileType, InputMappingValueID inputItem);

    /// @brief This method check whether the specific output mapping item is required.
    /// @param[in] category     The category of profile.
    /// @param[in] profileType  The protocol type of profile.
    /// @param[in] outputItem   The specific output item.
    /// @return result indicate the item is required or not.
    static bool checkOutputMappingItemRequired(const AxisProfileCategory category, const AxisProfileType profileType, OutputMappingValueID outputItem);

    /// @brief This method check get the group information of the specific input mapping item.
    /// @param[in] category     The category of profile.
    /// @param[in] profileType  The protocol type of profile.
    /// @param[in] inputItem   The specific input item.
    /// @return result indicate which function group the item is belongs to.
    static MappingGroup getInputMappingGroup(const AxisProfileCategory category, const AxisProfileType profileType, InputMappingValueID inputItem);

    /// @brief This method check get the group information of the specific output mapping item.
    /// @param[in] category     The category of profile.
    /// @param[in] profileType  The protocol type of profile.
    /// @param[in] outputItem   The specific output item.
    /// @return result indicate which function group the item is belongs to.
    static MappingGroup getOutputMappingGroup(const AxisProfileCategory category, const AxisProfileType profileType, OutputMappingValueID outputItem);

    /// @brief This method will get mandatory mapping item for input.
    /// @return The container with mandatory input mapping items.
    static AxisProfileResult getMandatoryInputMappingItems(const AxisProfileCategory category, std::vector<InputMappingValueID>& vecInputItems);

    /// @brief This method will get mandatory mapping item for output.
    /// @return The container with mandatory output mapping items.
    static AxisProfileResult getMandatoryOutputMappingItems(const AxisProfileCategory category, std::vector<OutputMappingValueID>& vecInputItems);

  };
}
