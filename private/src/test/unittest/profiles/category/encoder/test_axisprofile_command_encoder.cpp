#include "test_axisprofile_command_encoder.h"
#include "helper/result_helper.h"
#include "mockup/mock_axisprofile_mapping_handler.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestAxisProfileCommandEncoder);
REGISTER_TEST_TO_GROUP(TestAxisProfileCommandEncoder, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCommandEncoder, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileCommandEncoder, 4_EncoderTest);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test


HRESULT TestAxisProfileCommandEncoder::doInit()
{
  HRESULT res = S_OK;

  log(L"\nInit test cases...");
  m_profileInfo = std::make_shared<ProfileBasicInfo>();
  m_telegramMDT = std::make_shared<MasterDataTelegram>();
  m_telegramAT = std::make_shared<AcknowledgeTelegram>();
  m_auxHandler = std::make_shared<AuxJobHandler>();
  m_profileInfo->m_deviceAddress = 1001;
  m_category = std::make_unique<EncoderCategoryAdapterCoE>(*m_profileInfo.get(), m_telegramAT.get(), m_telegramMDT.get());
  m_sharingHandler = std::make_unique<AxisProfileSharingHandler>(&m_profileInfo->m_profileTrace);
  m_category->registerSharingITFs(m_sharingHandler.get());
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_CATEGORY, m_category.get());
  return res;
}

HRESULT TestAxisProfileCommandEncoder::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestAxisProfileCommandEncoder interface");
  log(L"\n----------------------------------------------------------------\n");

  log(L"\n\t Test of abortAxisProfileCommand...");
  AxisProfileCommandType cmdType {};
  auto result = m_category->getCommandInternal()->abortAxisProfileCommand(cmdType);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of activateCyclicStandStillCheck...");
  result = m_category->getCommandInternal()->activateCyclicStandStillCheck();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of addAuxCyclicJobs without auxhandler");
  result = m_category->getCommandInternal()->addAuxCyclicJobs();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\n\t Test of addAuxCyclicJobs...");
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_AUXJOB, m_auxHandler.get());
  result = m_category->getCommandInternal()->addAuxCyclicJobs();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);
  m_auxHandler->runAllAuxJobs();

  log(L"\n\t Test of booting...");
  result = m_category->getCommandInternal()->booting();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of checkAxisHomed without mapping handler");
  bool homed {};
  result = m_category->getCommandInternal()->checkAxisHomed(homed);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOTIMPLEMENTED);

  log(L"\n\t Test of checkIfDeviceHalt...");
  bool halted {};
  result = m_category->getCommandInternal()->checkIfDeviceHalt(halted);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of checkStandStill...");
  bool standstill {};
  result = m_category->getCommandInternal()->checkStandStill(&standstill);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of clearDeviceError...");
  result = m_category->getCommandInternal()->clearDeviceError();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of deactivateCyclicStandStillCheck...");
  result = m_category->getCommandInternal()->deactivateCyclicStandStillCheck();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getAxisProfileCommandState...");
  m_category->getCommandInternal()->getAxisProfileCommandState(cmdType);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getCInterface...");
  m_category->getCommandInternal()->getCInterface();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getClampingTorque...");
  double torque {};
  result = m_category->getCommandInternal()->getClampingTorque(&torque);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getStandStillWindow...");
  double windows {};
  result = m_category->getCommandInternal()->getStandStillWindow(&windows);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of referencingAxis...");
  double pos {};
  AxisProfileEncoder encoder {};
  result = m_category->getCommandInternal()->referencingAxis(pos, encoder);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of resetAbsolutePosition...");
  result = m_category->getCommandInternal()->resetAbsolutePosition();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of resetAllAxisprofileCommands...");
  result = m_category->getCommandInternal()->resetAllAxisprofileCommands();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of resetAxisProfileCommandState...");
  result = m_category->getCommandInternal()->resetAxisProfileCommandState(cmdType);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of resetReferencingAxis...");
  result = m_category->getCommandInternal()->resetReferencingAxis();
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setAbsolutePosition without mapping");
  result = m_category->getCommandInternal()->setAbsolutePosition(pos, encoder, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_EXEC_NULL);
    
  log(L"\n\t Test of setAbsolutePosition...");
  test::MockAxisProfileMappingHandler mappingHandlerMockup {};
  m_sharingHandler->registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);
  result = m_category->getCommandInternal()->setAbsolutePosition(pos, encoder, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_OK);

  log(L"\n\t Test of setAbsolutePosition...");
  auto& assigned = mappingHandlerMockup.getProfileAssignedInfo();
  assigned.m_input.m_isInReference = true;
  assigned.m_output.m_setReference = true;
  assigned.m_output.m_referenceOffset = true;
  result = m_category->getCommandInternal()->setAbsolutePosition(pos, encoder, true);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::STS_PENDING);

  log(L"\n\t Test of checkAxisHomed...");
  result = m_category->getCommandInternal()->checkAxisHomed(homed);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOTIMPLEMENTED);

  log(L"\n\t Test of setAndCheckClampingTorque...");
  result = m_category->getCommandInternal()->setAndCheckClampingTorque(torque);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setAndCheckPositiveStop...");
  AxisProfilePositiveStop stop {};
  result = m_category->getCommandInternal()->setAndCheckPositiveStop(stop);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setAndCheckStandStillWindow...");
  result = m_category->getCommandInternal()->setAndCheckStandStillWindow(windows);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of setReferencingPosition...");
  RefMode mode {};
  result = m_category->getCommandInternal()->setReferencingPosition(pos, encoder, mode);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  log(L"\n\t Test of getCommandsAtDeterminedState...");
  AxisprofileCommandState state {};
  std::vector<AxisProfileCommandType> vec {};
  result = m_category->getCommandInternal()->getCommandsAtDeterminedState(state, vec);
  RETURN_TEST_RESULT(result, L"\nUnexpected return value", AxisProfileResult::ERR_NOT_SUPPORTED);

  while (true)
  {
    m_auxHandler->runAllAuxJobs();
    if (AxisprofileCommandState::CMD_STATE_IDLE == m_category->getCommandInternal()->getAxisProfileCommandState(AxisProfileCommandType::CMD_RESET_ABSOLUTE_POSITION))
    {
      break;
    }
  }

  return res;
}
