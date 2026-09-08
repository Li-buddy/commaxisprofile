#include "axisprofile_internal_defs.h"
#include "comm/axisprofile/defines/axisprofile_defs.h"
#include "helper/result_helper.h"
#include "profiles/profile_defs.h"
#include "profiles/value_mapping/mapping_handler_defs.h"
#include "test_axisprofile_defs.h"

using namespace comm::axisprofile;


REGISTER_TEST(TestAxisProfileDefs);
REGISTER_TEST_TO_GROUP(TestAxisProfileDefs, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDefs, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestAxisProfileDefs, General);


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor

TestAxisProfileDefs::TestAxisProfileDefs()
{
}

TestAxisProfileDefs::~TestAxisProfileDefs(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

HRESULT TestAxisProfileDefs::doTest()
{
  HRESULT hResult = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test the axisprofile defines.");
  log(L"\n----------------------------------------------------------------\n");

  hResult |= testPublicDefs();
  hResult |= testInternalDefs();
  hResult |= testProfileDefs();
  hResult |= testMappingHandleDefs(); 
  return hResult;
}

HRESULT TestAxisProfileDefs::testPublicDefs()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the public (external) defines</b>");

  log(L"\nTest the method stringToAxisProfileType().");
  AxisProfileType expType = AxisProfileType::UNDEFINED;
  AxisProfileType profileType = stringToAxisProfileType(nullptr);
  if (expType != profileType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, profileType);
    res = E_FAIL;
  }

  profileType = stringToAxisProfileType("Test");
  if (expType != profileType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, profileType);
    res = E_FAIL;
  }

  expType = AxisProfileType::SERVODRIVE_OVER_ETHERCAT;
  profileType = stringToAxisProfileType(AprTypeStrings.SERVODRIVE_OVER_ETHERCAT.c_str());
  if (expType != profileType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, profileType);
    res = E_FAIL;
  }

  expType = AxisProfileType::CANOPEN_OVER_ETHERCAT;
  profileType = stringToAxisProfileType(AprTypeStrings.CANOPEN_OVER_ETHERCAT.c_str());
  if (expType != profileType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, profileType);
    res = E_FAIL;
  }

  log(L"\nTest the method axisProfileTypeToString().");

  std::uint32_t index = 0;
  std::uint32_t unknownCount = 0;
  while (true)
  {
    AxisProfileType type = static_cast<AxisProfileType>(index);
    const char* conversionResult = axisProfileTypeToString(type);

    if (!conversionResult)
    {
      error(L"\nConversion result is a nullptr!");
      res = E_FAIL;
      break;
    }

    std::string convResult(conversionResult);
    log(L"\nConverted type string: %ls", UTF8TOWIDE(convResult.c_str()));

    if (convResult.compare(AprTypeStrings.UNKNOWN) == 0)
    {
      unknownCount++;
    }

    if (unknownCount == 2)
    {
      if (index == 1)
      {
        error(L"\nOnly received 'Unknown' two times!");
        res = E_FAIL;
        break;
      }
      // The second Unknown means: End of AxisProfileType enum
      break;
    }

    index++;
  }

  log(L"\nTest the method axisprofilePowerModeToString().");
  std::uint32_t powermodeIndex = 0;
  std::uint32_t undefineCount = 0;
  while (true)
  {
    AxisProfilePowerMode mode = static_cast<AxisProfilePowerMode>(powermodeIndex);
    const char* conversionResult = axisprofilePowerModeToString(mode);

    if (!conversionResult)
    {
      error(L"\nConversion result is a nullptr!");
      res = E_FAIL;
      break;
    }

    std::string convResult(conversionResult);
    log(L"\nConversion mode strin: %ls", UTF8TOWIDE(convResult.c_str()));

    if (convResult.compare("UNDEFINED") == 0)
    {
      undefineCount++;
    }

    if (undefineCount == 2)
    {
      if (powermodeIndex == 1)
      {
        error(L"\nOnly received 'Undefine' two times!");
        res = E_FAIL;
        break;
      }
      // The second Unknown means: End of AxisProfileType enum
      break;
    }

    powermodeIndex++;
  }
  
  log(L"\nTest the method stringToAxisProfileCategory().");
  AxisProfileCategory expCategory = AxisProfileCategory::UNDEFINED;
  AxisProfileCategory profileCategory = stringToAxisProfileCategory(nullptr);
  
  RETURN_TEST_RESULT(profileCategory, L"\n  Returned category is not expected category.", expCategory);

  profileCategory = stringToAxisProfileCategory("Test");
  RETURN_TEST_RESULT(profileCategory, L"\n  Returned category is not expected category.", expCategory);

  expCategory = AxisProfileCategory::CATEGORY_ENCODER;
  profileCategory = stringToAxisProfileCategory(AprCategoryStrings.CATEGORY_ENCODER.c_str());
  RETURN_TEST_RESULT(profileCategory, L"\n  Returned category is not expected category.", expCategory);

  expCategory = AxisProfileCategory::CATEGORY_DRIVE;
  profileCategory = stringToAxisProfileCategory(AprCategoryStrings.CATEGORY_DRIVE.c_str());
  RETURN_TEST_RESULT(profileCategory, L"\n  Returned category is not expected category.", expCategory);

  log(L"\nDummy test of enum to string...");
  log(L"\n\taxisprofileRefModeToString on SET_POS_DEVICE, result %s", UTF8TOWIDE(axisprofileRefModeToString(RefMode::SET_POS_DEVICE)));
  log(L"\n\taxisprofileRefModeToString on SET_POS_PROFILE, result %s", UTF8TOWIDE(axisprofileRefModeToString(RefMode::SET_POS_PROFILE)));
  log(L"\n\taxisprofileRefModeToString on SET_NONE, result %s", UTF8TOWIDE(axisprofileRefModeToString(RefMode::SET_NONE)));
  log(L"\n\taxisprofileRefModeToString on -1, result %s", UTF8TOWIDE(axisprofileRefModeToString(static_cast<RefMode>(-1))));

  log(L"\n\taxisProfileCategoryTypeToString on CATEGORY_DRIVE, result %s", UTF8TOWIDE(axisProfileCategoryTypeToString(AxisProfileCategory::CATEGORY_DRIVE)));
  log(L"\n\taxisProfileCategoryTypeToString on CATEGORY_ENCODER, result %s", UTF8TOWIDE(axisProfileCategoryTypeToString(AxisProfileCategory::CATEGORY_ENCODER)));
  log(L"\n\taxisProfileCategoryTypeToString on -1, result %s", UTF8TOWIDE(axisProfileCategoryTypeToString(static_cast<AxisProfileCategory>(-1))));

  log(L"\n\topModeSupportedErrorTypeToString on ERROR_NONE, result %s", UTF8TOWIDE(opModeSupportedErrorTypeToString(OpModeSupportedErrorType::ERROR_NONE)));
  log(L"\n\topModeSupportedErrorTypeToString on ERROR_NOT_SUPPORT, result %s", UTF8TOWIDE(opModeSupportedErrorTypeToString(OpModeSupportedErrorType::ERROR_NOT_SUPPORT)));
  log(L"\n\topModeSupportedErrorTypeToString on ERROR_DRIVE_CONFIG, result %s", UTF8TOWIDE(opModeSupportedErrorTypeToString(OpModeSupportedErrorType::ERROR_DRIVE_CONFIG)));
  log(L"\n\topModeSupportedErrorTypeToString on ERROR_CYCLIC_DATA_MISSING, result %s", UTF8TOWIDE(opModeSupportedErrorTypeToString(OpModeSupportedErrorType::ERROR_CYCLIC_DATA_MISSING)));
  log(L"\n\topModeSupportedErrorTypeToString on -1, result %s", UTF8TOWIDE(opModeSupportedErrorTypeToString(static_cast<OpModeSupportedErrorType>(-1))));

  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_NONE, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_NONE)));
  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_NOT_PROVIDED, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_NOT_PROVIDED)));
  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_MASTER_STATUS, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_MASTER_STATUS)));
  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_SLAVE_STATUS, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_SLAVE_STATUS)));
  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_CFG_MAPPING_ERROR, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_CFG_MAPPING_ERROR)));
  log(L"\n\tdataAccessCommonErrorTypeToString on ERROR_BEYOND_THE_LIMIT, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(DataAccessCommonErrorType::ERROR_BEYOND_THE_LIMIT)));
  log(L"\n\tdataAccessCommonErrorTypeToString on -1, result %s", UTF8TOWIDE(dataAccessCommonErrorTypeToString(static_cast<DataAccessCommonErrorType>(-1))));

  log(L"\n\taxisProfileScalingStrategyToString on SOE_STANDARD, result %s", UTF8TOWIDE(axisProfileScalingStrategyToString(AxisProfileScalingStrategy::SOE_STANDARD)));
  log(L"\n\taxisProfileScalingStrategyToString on COE_CALC_DRIVE, result %s", UTF8TOWIDE(axisProfileScalingStrategyToString(AxisProfileScalingStrategy::COE_CALC_DRIVE)));
  log(L"\n\taxisProfileScalingStrategyToString on COE_CALC_CONTROL, result %s", UTF8TOWIDE(axisProfileScalingStrategyToString(AxisProfileScalingStrategy::COE_CALC_CONTROL)));
  log(L"\n\taxisProfileScalingStrategyToString on IO_DEVICE, result %s", UTF8TOWIDE(axisProfileScalingStrategyToString(AxisProfileScalingStrategy::DEVICE_IO)));
  log(L"\n\taxisProfileScalingStrategyToString on -1, result %s", UTF8TOWIDE(axisProfileScalingStrategyToString(static_cast<AxisProfileScalingStrategy>(-1))));

  log(L"\n\taxisProfileOpModeToString on -1, result %s", UTF8TOWIDE(axisProfileOpModeToString(static_cast<AxisProfileOperationMode>(-1))));

  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_NO_INFO, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_NO_INFO)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_NONE, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_NONE)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_LIGHT, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_LIGHT)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_FATAL, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_FATAL)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_CRITICAL, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_CRITICAL)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on ERROR_FIELDBUS, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(AxisProfileDeviceErrorType::ERROR_FIELDBUS)));
  log(L"\n\taxisProfileDeviceErrorTypeToString on -1, result %s", UTF8TOWIDE(axisProfileDeviceErrorTypeToString(static_cast<AxisProfileDeviceErrorType>(-1))));

  log(L"\n\tprobeIndexToString on PROBE_1, result %s", UTF8TOWIDE(probeIndexToString(ProbeIndex::PROBE_1)));
  log(L"\n\tprobeIndexToString on PROBE_2, result %s", UTF8TOWIDE(probeIndexToString(ProbeIndex::PROBE_1)));
  log(L"\n\tprobeIndexToString on -1, result %s", UTF8TOWIDE(probeIndexToString(static_cast<ProbeIndex>(-1))));

  for (std::uint16_t i = CMD_MIN; i <= CMD_MAX+1; i++)
  {
    log(L"\n\tcmdTypeToString on i, result %s", UTF8TOWIDE(cmdTypeToString(static_cast<CmdType>(i))));
  }
  
  for (std::uint16_t i = STATE_MIN; i <= STATE_MAX+1; i++)
  {
    log(L"\n\tcmdStateToString on i, result %s", UTF8TOWIDE(cmdStateToString(static_cast<CmdState>(i))));
  }

  log(L"\n");
  return res;
}

HRESULT TestAxisProfileDefs::testInternalDefs()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the internal defines</b>");

  log(L"\nConvert all DriveProfilTypes to string and back.");
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(DriveProfileType::MAX_VAL); i++)
  {
    DriveProfileType driveProfileType = static_cast<DriveProfileType>(i);
    std::string profileTypeNameString = driveProfileTypeToString(driveProfileType);
    DriveProfileType profileTypeEnum = stringToDriveProfileType(profileTypeNameString.c_str());
    if (profileTypeEnum != driveProfileType)
    {
      error(L"\nConversion of drive profile type %ls failed!", UTF8TOWIDE(profileTypeNameString.c_str()));
      res = E_FAIL;
    }
  }

  log(L"\nConvert all NetworkTechnologies to string and back.");
  for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(NetworkTechnology::MAX_VAL); i++)
  {
    NetworkTechnology networkTechnology = static_cast<NetworkTechnology>(i);
    std::string networkTechnologyString = networkTechnologyToString(networkTechnology);
    NetworkTechnology networkTechnologyEnum = stringToNetworkTechnology(networkTechnologyString.c_str());
    if (networkTechnologyEnum != networkTechnology)
    {
      error(L"\nConversion of network technology %ls failed!", UTF8TOWIDE(networkTechnologyString.c_str()));
      res = E_FAIL;
    }
  }

  log(L"\n");
  return res;
}

HRESULT TestAxisProfileDefs::testProfileDefs()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the profile defines</b>");

  log(L"\nTest the method deviceSerieToString().");
  std::uint32_t index = 0;
  std::uint32_t undefCount = 0;
  while (true)
  {
    const char* deviceSerie = deviceSerieToString(static_cast<DeviceSeries>(index));
    if (!deviceSerie)
    {
      error(L"\nConversion result is a nullptr!");
      res = E_FAIL;
      break;
    }

    std::string deviceSerieInString(deviceSerie);
    log(L"\nConverted type string: %ls", UTF8TOWIDE(deviceSerieInString.c_str()));

    if (deviceSerieInString.compare("Undefined") == 0)
    {
      undefCount++;
    }

    if (undefCount == 2)
    {
      if (index == 1)
      {
        error(L"\nOnly received 'Undefine' two times!");
        res = E_FAIL;
        break;
      }
      // The second Undefine means: End of DeviceSeries enum
      break;
    }
    index++;
  }

  log(L"\n");
  return res;
}

HRESULT TestAxisProfileDefs::testMappingHandleDefs()
{
  HRESULT res = S_OK;
  log(L"\n<b>Test of the profile mapping handle defines</b>");
  
  log(L"\nTest the method stringToMappingGroup().");
  auto expType = MappingGroup::UNKNOWN;
  auto mappingType = stringToMappingGroup(nullptr);
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  mappingType = stringToMappingGroup("Test");
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::BASE;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.BASE.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::BASE;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.BASE.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::BASE;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.BASE.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::VELOCITY;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.VELOCITY.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::HOMING;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.HOMING.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::PROBE;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.PROBE.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::SAFE_MOTION;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.SAFE_MOTION.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::USER_PDO;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.USER_PDO.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  expType = MappingGroup::SIGNAL_WORD;
  mappingType = stringToMappingGroup(AprMappingGroupStrings.SIGNAL_WORD.c_str());
  if (expType != mappingType)
  {
    error(L"Returned type is not expected type. Exp: %d, Act: %d", expType, mappingType);
    res = E_FAIL;
  }

  log(L"\nTest the method mappingGroupToGroupMask().");
  std::uint32_t index = 0;
  std::uint32_t defaultCount = 0;
  while (true)
  {
    auto mappingGroupMask = mappingGroupToGroupMask(static_cast<MappingGroup>(index));

    if (mappingGroupMask == 0)
    {
      defaultCount++;
    }

    if (defaultCount == 2)
    {
      if (index == 1)
      {
        error(L"\nOnly received default two times!");
        res = E_FAIL;
        break;
      }
      // The second Undefine means: End of DeviceSeries enum
      break;
    }
    index++;
  }
  log(L"\n");
  return res;
}
