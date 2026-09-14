#pragma once

/// @file
///
/// @brief This file is part of the axisprofile library and implements a mockup for
/// the boot parameter snapshot interface.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/i_device_boot_param_snapshot.h"

namespace test
{
  class MockDeviceBootParmSnapshot : public comm::axisprofile::IDeviceBootParamSnapshot
  {
  public:
    explicit MockDeviceBootParmSnapshot(comm::axisprofile::ProfileBasicInfo& profileBasicInfo)
      : m_profileBasicInfo(profileBasicInfo)
    {
    }

    ~MockDeviceBootParmSnapshot() noexcept override = default;

    void setBootParamSnapshot(const comm::axisprofile::BootParamSnapshot& snapshot) override
    {
      m_snapshot = snapshot;
    }

    const comm::axisprofile::BootParamSnapshot& getBootParamSnapshot() const override
    {
      return m_snapshot;
    }

    void setReturnValue(comm::axisprofile::AxisProfileResult result)
    {
      m_returnValue = result;
    }

  private:
    void setSharingHandler(comm::axisprofile::IAxisProfileSharingHandler* handler) override
    {
      m_sharingHandler = handler;
    }

    comm::axisprofile::AxisProfileResult m_returnValue {comm::axisprofile::AxisProfileResult::STS_OK};
    comm::axisprofile::BootParamSnapshot m_snapshot {};
    comm::axisprofile::ProfileBasicInfo& m_profileBasicInfo;
    comm::axisprofile::IAxisProfileSharingHandler* m_sharingHandler {};
  };
}
