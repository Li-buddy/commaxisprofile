#pragma once

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "comm/axisprofile/datatypes/valueMapping_generated.h"
#include "profiles/category/category_mapping_helper.h"
#include "provider/provider_helper.h"
#include "stdafx.h"
#include <map>

// Mockups
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_profile_trace.h"

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

//! @class TestProviderProfilesOutputMapping
//! Test datalayer node to read/write the complete output mapping.
class TestProviderProfilesOutputMapping : public xTST_Test2
{
public:
  //! Constructor
  TestProviderProfilesOutputMapping();
  //! Destructor
  virtual ~TestProviderProfilesOutputMapping() noexcept override;

  //
  // interface functions
  //
  TEST_DECLARE_NAME("Unittest.Provider.Profiles.OutputMapping");
  TEST_DECLARE_DESCRIPTION("Unittest of the axisprofile datalayer node, to read/write the complete output mapping.");
  TEST_DECLARE_TIMEOUT_MINUTES(1);
  TEST_DECLARE_RESPONSIBLE_EMAIL(EMAIL_PEG);
  TEST_DECLARE_SUBSTITUTE_EMAIL(EMAIL_RAK);


  //! @brief Init function of the test.
  //! @return S_OK, when initialization successful. 
  HRESULT doInit();

  //! @brief   main test function for running all tests
  //! @return  S_OK, when tests were successful
  HRESULT doTest();

  //! @brief Create a default output mapping variant for testing.
  //! @param[in]   builder           Top level flatbuffer builder
  //! @param[in]   outputMapping     Output mapping used to generate the flatbuffer.
  //! @param[in]   leaveFieldEmpty   Leave a flatbuffer field empty for error simulation.
  //! @return  The finished mapping flatbuffer as flatbuffers offset.
  static struct flatbuffers::Offset<comm::axisprofile::fbtypes::valueMapping> createOutputMappingFlatbuffer(flatbuffers::FlatBufferBuilder& builder,
                                                                                                            std::vector<comm::axisprofile::OutputMappingEntry>& outputMapping, bool leaveFieldEmpty = false);

private: //methods

  //! @brief Check behavior of different error cases.
  //! @return  S_OK, when the test was successful.
  HRESULT testErrors();

  //! @brief Check behavior of a successfull node request.
  //! @return  S_OK, when the test was successful.
  HRESULT testNode();

  //! @brief Create a default output mapping variant for testing.
  //! @param[in]   leaveFieldEmpty   Leave a flatbuffer field empty for error simulation.
  //! @return  The finished mapping flatbuffer as variant.
  comm::datalayer::Variant createMappingFB(bool leaveFieldEmpty = false);

private: // Data

  std::string m_profileName = "testProfile";                                                      //!< Profile name used in test
  std::string m_nodeAddress = comm::axisprofile::provider::NodeTree.APR_PROFILES_CFG_OUTMAPPING;  //!< Node address of the test class

private: // Types

  //! @class MockAxisProfileMappingHandlerSpecial
  //! Specialization of the axis profile maping handler mockup, needed for this test.
  class MockAxisProfileMappingHandlerSpecial : public test::MockAxisProfileMappingHandler
  {
  public: // methods

    //! @brief Constructor
    MockAxisProfileMappingHandlerSpecial(): test::MockAxisProfileMappingHandler()
    {}

    //! @brief Destructor
    virtual ~MockAxisProfileMappingHandlerSpecial() noexcept
    {
      m_outputMapping.clear();
    }

    const std::vector< comm::axisprofile::OutputMappingEntry> getConfiguredOutputs() const final
    {
      std::vector<comm::axisprofile::OutputMappingEntry> outputMapping {};
      for (auto const& mappingItem : m_outputMapping)
      {
        outputMapping.emplace_back(mappingItem.first, mappingItem.second,
                                   comm::axisprofile::CategoryMappingHelper::getOutputMappingGroup(m_profileCategory, m_axisProfileType, mappingItem.first),
                                   comm::axisprofile::CategoryMappingHelper::checkOutputMappingItemRequired(m_profileCategory, m_axisProfileType, mappingItem.first));
      }
      return outputMapping;
    }

  public: // data

    std::map< comm::axisprofile::OutputMappingValueID, std::string> m_outputMapping {};                                                 ///< Map that contains the general output mapping
    std::map< comm::axisprofile::OutputMappingValueID, comm::axisprofile::MappingCfgInfo> m_outputMappingCfgInfo {};                    ///< Map which stored mapping type with value ID  };
    comm::axisprofile::AxisProfileType               m_axisProfileType {comm::axisprofile::AxisProfileType::SERVODRIVE_OVER_ETHERCAT}; ///< The profile type
    comm::axisprofile::AxisProfileCategory           m_profileCategory {comm::axisprofile::AxisProfileCategory::CATEGORY_DRIVE};       ///< The profile category
  };
};
