#pragma once

/// @file
///
/// @brief This header file is part of the axisprofile library and declares a mockup of the provider manager.
///
/// SPDX-FileCopyrightText: Copyright (c) 2019-2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages.
/// All rights reserved in the event of the grant of a patent, utility model or design.


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "provider/provider_mgr.h"

// ---------------------------------------------------------------------------
// Forward references
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
  // Declaration of macros
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  //! @class MockProviderMgr
  //! @brief This class implements a mockup for the provider manager.
  class MockProviderMgr : public comm::axisprofile::provider::ProviderMgr
  {
  public: // methods
    //! @brief Destructor
    virtual ~MockProviderMgr() noexcept override = default;

    void init(comm::axisprofile::IAxisProfileComponent *cmpAxisProfile) override { m_cmpMockAxisProfile = cmpAxisProfile; }
    //! @brief This method adds a new node to the providers.    
    void addNode(const ::std::string &/*address*/, comm::axisprofile::provider::INodeBase* /*newNode*/) override {};
    comm::axisprofile::IAxisProfileComponent* getAxisProfileCmp() const override { return m_cmpMockAxisProfile; }
    bool run() override { return m_returnValue; }
    bool stop() override { return m_returnValue; }

    //! @brief register a flatbuffer type to a node in the datalayer
    //! @return false on error
    bool registerTypes(const ::std::string &/*bfbsPath*/, const ::std::string &/*nodeAddress*/) override { return m_returnValue;  }

    void registerScopes() override {}
    void unregisterScopes() override {}

    // ----------------------------- Test methods ---------------------------------

    //! @brief This method can be used by tests to change the behavior of the member functions
    //!        by changing the returned value.
    //!
    //! @param[in]    result   New return code
    void setReturnValue(bool result) { m_returnValue = result; }

  protected: // data

    bool m_returnValue = true;   //!< Result of member functions

  private:

    comm::axisprofile::IAxisProfileComponent *m_cmpMockAxisProfile{};   //!< Pointer to the axisprofile component (usually the mockup is used)

  };
}
