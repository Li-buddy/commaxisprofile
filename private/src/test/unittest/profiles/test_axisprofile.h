#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "stdafx.h"

// Mockups
#include "mockup/mock_axisprofile_cmd.h"
#include "mockup/mock_axisprofile_category.h"

// Test Class
#include "profiles/axisprofile.h"


// ---------------------------------------------------------------------------
// Declaration of symbolic constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Declaration of data types
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Test Class
// ---------------------------------------------------------------------------

/// @brief TestAxisProfile
/// @brief Test class for the axis profile base class.
class TestAxisProfile : public xTST_Test2
{
public:
  /// constructor
  TestAxisProfile() = default;

  /// Destructor
  virtual ~TestAxisProfile() noexcept override = default;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Profiles.AxisProfile");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile base class.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);

  /// @brief   main test function for running all tests
  /// @return  S_OK, when tests were successful
  HRESULT doTest();

  /// @brief Test the axisprofile C-Interface.
  /// @return  S_OK, when tests were successful
  HRESULT testCInterface() const;

  /// @brief   Test the axisprofile base class.
  /// @return  S_OK, when tests were successful
  HRESULT testClass();

  HRESULT testForFPUError() const;

private: // Data

  std::string m_profileName = "testProfile";                                                                        //!< Profile name used in this test
  comm::axisprofile::AxisProfileType m_testProfileType = comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT;    //!< Profile type used in test
  comm::axisprofile::DriveProfileType m_testDriveProfileType = comm::axisprofile::DriveProfileType::SERCOS;      //!< Drive profile type used in test
  comm::axisprofile::NetworkTechnology m_testNetworkTechnology = comm::axisprofile::NetworkTechnology::UNDEFINED;   //!< Network technology used in test

private: // types

  /// @brief AxisProfileUnittest
  /// @brief Specialization of the test class just to overwrite the pure virtual methods and make it creatable. 
  class AxisProfileUnittest : public comm::axisprofile::AxisProfile
  {
  public: // methods

    /// @brief constructor
    /// @param[in]   profileName     Name of the profile
    /// @param[in]   type            Type of the profile
    AxisProfileUnittest(const std::string& profileName, comm::axisprofile::AxisProfileType type, const comm::axisprofile::DriveProfileType driveProfileType,
                        const comm::axisprofile::NetworkTechnology networkTechnology) : comm::axisprofile::AxisProfile(profileName, type, driveProfileType, networkTechnology)
    {
      
      m_cmd2Mock = new test::MockAxisProfileCmd();
      m_cmdIf = m_cmd2Mock;
      m_categoryMock = new test::MockCategory();
      m_profileCategory = m_categoryMock;

      m_profileCategory->registerSharingITFs(m_sharingHandler);
    }

    /// @brief destructor
    virtual ~AxisProfileUnittest() = default;

    comm::axisprofile::AxisProfileResult getDeviceState() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult init() final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult processDataAt(std::uint8_t* /*dataByteArray*/) final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    comm::axisprofile::AxisProfileResult processDataMdt(std::uint8_t* /*dataByteArray*/)  final { return comm::axisprofile::AxisProfileResult::STS_OK; }
    std::vector<std::unique_ptr<IJobObj>> onGetJobList() { std::vector<std::unique_ptr<IJobObj>> jobs; return jobs; }
    comm::axisprofile::ICategoryAdapter* getCategoryPtr(){return m_profileCategory;}
    void checkForFPUError()
    {
      AxisProfile::checkForFPUError();
    }

  public: // Data
        
    test::MockAxisProfileCmd* m_cmd2Mock {};    ///< Mockup of the new command interface used in test
    test::MockCategory* m_categoryMock {};
  };
};
