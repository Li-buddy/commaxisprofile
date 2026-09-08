#include "mockup/mock_axisprofile_device_param.h"
#include "mockup/mock_axisprofile_command.h"
#include "mockup/mock_axisprofile_mapping_handler.h"
#include "mockup/mock_profile_trace.h"
#include "test_sharing_handler.h"
#include "helper/result_helper.h"
// Test Class
#include "profiles/data_sharing/sharing_handler.h"


REGISTER_TEST(TestSharingHandler);
REGISTER_TEST_TO_GROUP(TestSharingHandler, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestSharingHandler, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestSharingHandler, Profiles);

using namespace comm::axisprofile;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestSharingHandler::doTest()
{
  HRESULT returnVal = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile sharing handler.");
  log(L"\n----------------------------------------------------------------");

  returnVal |= testInterface();

  return returnVal;
}

HRESULT TestSharingHandler::testInterface()
{
  HRESULT result = S_OK;
  test::MockAxisProfileTrace profileTraceMockup{};
  test::MockAxisProfileDeviceParam deviceParamMockup{};
  test::MockAxisProfileMappingHandler mappingHandlerMockup{};
  test::MockAxisProfileCommand commandIf {};

  AxisProfileSharingHandler sharingHandler(&profileTraceMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, &deviceParamMockup);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_COMMAND, &commandIf);
  sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_MAPHANDLER, &mappingHandlerMockup);

  log(L"\n\n<b>----- Test all class interface methods -----</b>");

  log(L"\nCall getDeviceParamIF().");
  const IAxisProfileDeviceParamInternal* deviceParamIF = sharingHandler.getDeviceParamIF();
  LOG_TEST_RESULT_SET_PTR(deviceParamIF, L"\nThe device parameter is a null-pointer!");

  log(L"\nCall getMappingHandlerIF().");
  const IAxisProfileMappingHandler* mappingHandlerIF = sharingHandler.getMappingHandlerIF();
  LOG_TEST_RESULT_SET_PTR(mappingHandlerIF, L"\nThe mapping handler is a null-pointer!");

  log(L"\nTest register an wrong interface type...");
  auto aprRes = sharingHandler.registSharingITF(static_cast<SharingInterfaceType>(-1), &commandIf);
  RETURN_TEST_RESULT(aprRes, L"\nUnexpected return value!", AxisProfileResult::ERR_WRONGDATATYPE);

  log(L"\nTest remove an wrong interface type...");
  sharingHandler.removeSharingITF(static_cast<SharingInterfaceType>(-1));

  log(L"\nTest register an empty interface...");
  aprRes = sharingHandler.registSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM, nullptr);
  RETURN_TEST_RESULT(aprRes, L"\nUnexpected return value!", AxisProfileResult::ERR_EXEC_NULL);

  log(L"\nTest remove an exiting interface...");
  sharingHandler.removeSharingITF(SharingInterfaceType::ITF_TYPE_DEVICEPARAM);
  log(L"\nCheck it has been removed...");
  auto itf = sharingHandler.getDeviceParamIF();
  LOG_TEST_RESULT_PTR(itf, L"\nUnexpected interface", nullptr);

  log(L"\nTest get and none exiting interface...");
  auto cmdItf = sharingHandler.getInternalITF<IAxisProfileCommandInternal* const>(static_cast<SharingInterfaceType>(-1));
  LOG_TEST_RESULT_PTR(cmdItf, L"\nUnexpected interface", nullptr);

  return result;
}
