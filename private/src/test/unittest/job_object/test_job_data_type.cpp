
#include "helper/result_helper.h"
#include "job_object/job_data_type.h"
#include "test_job_data_type.h"

using namespace test;
using namespace comm::axisprofile;

REGISTER_TEST(TestJobDataType);
REGISTER_TEST_TO_GROUP(TestJobDataType, 1_AutoTest);
REGISTER_TEST_TO_GROUP(TestJobDataType, 2_UnitTest);
REGISTER_TEST_TO_GROUP(TestJobDataType, JobTest);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Test

/// @brief Structure for test
struct TestStruct
{
  std::int32_t myInt;   ///< Integer value
  double myDouble;      ///< Double value
  std::string myString; ///< String value
};

/// @brief declare named types for testing
DECLARE_NAMED_TYPE(MyTestDouble, double);
DECLARE_NAMED_TYPE(MyTestIntger, std::int32_t);
DECLARE_NAMED_TYPE(MyTestString, std::string);
DECLARE_NAMED_TYPE(MyTestStruct, TestStruct);

HRESULT TestJobDataType::doTest()
{
  HRESULT res = S_OK;

  log(L"\n\n----------------------------------------------------------------");
  log(L"\n  Test of the TestJobDataType");
  log(L"\n----------------------------------------------------------------\n");

  res |= doTestCase();

  return res;
}

HRESULT TestJobDataType::doTestCase()
{
  log(L"\nTest base type ...");
  double expDouble = 3.14;
  std::int32_t expInt = 123;
  std::string expString = "Hello";
  TestStruct expStruct = {expInt, expDouble, expString};

  log(L"\nConstrue types...");
  MyTestDouble myDouble(expDouble);
  MyTestIntger myInt(expInt);
  MyTestString myString(expString);
  MyTestStruct myStruct(expStruct);
  log(L"\nId of type of MyTestDouble is %u", myDouble.getID());
  log(L"\nId of type of MyTestIntger is %u", myInt.getID());
  log(L"\nId of type of MyTestString is %u", myString.getID());
  log(L"\nId of type of MyTestStruct is %u", myStruct.getID());

  log(L"\nTest get const value...");
  LOG_TEST_RESULT_FLT(myDouble.get(), L"\nGet an unexpected result!", expDouble, 1E-10);
  LOG_TEST_RESULT(myInt.get(), L"\nGet an unexpected result!", expInt);
  LOG_TEST_RESULT_STRING(myString.get(), L"\nGet an unexpected result!", expString);
  LOG_TEST_RESULT_FLT(myStruct.get().myDouble, L"\nGet an unexpected result!", expDouble, 1E-10);
  LOG_TEST_RESULT(myStruct.get().myInt, L"\nGet an unexpected result!", expInt);
  LOG_TEST_RESULT_STRING(myStruct.get().myString, L"\nGet an unexpected result!", expString);

  log(L"\nTest get reference value...");
  LOG_TEST_RESULT_FLT(myDouble.getRef(), L"\nGet an unexpected result!", expDouble, 1E-10);
  LOG_TEST_RESULT(myInt.getRef(), L"\nGet an unexpected result!", expInt);
  LOG_TEST_RESULT_STRING(myString.getRef(), L"\nGet an unexpected result!", expString);
  LOG_TEST_RESULT_FLT(myStruct.getRef().myDouble, L"\nGet an unexpected result!", expDouble, 1E-10);
  LOG_TEST_RESULT(myStruct.getRef().myInt, L"\nGet an unexpected result!", expInt);
  LOG_TEST_RESULT_STRING(myStruct.getRef().myString, L"\nGet an unexpected result!", expString);

  log(L"\nTest set value...");
  TestStruct expStruct1 = {321, 3.21, "321"};
  myStruct.set(expStruct1);
  LOG_TEST_RESULT_FLT(myStruct.getRef().myDouble, L"\nGet an unexpected result!", expStruct1.myDouble, 1E-10);
  LOG_TEST_RESULT(myStruct.getRef().myInt, L"\nGet an unexpected result!", expStruct1.myInt);
  LOG_TEST_RESULT_STRING(myStruct.getRef().myString, L"\nGet an unexpected result!", expStruct1.myString);

  log(L"\nTest safe cast success...");
  IContainer* myData = new MyTestStruct(expStruct1);
  auto castedData = MyTestStruct::safeCast(myData);
  LOG_TEST_RESULT_SET_PTR(castedData , L"\nGet an unexpected result!");
  LOG_TEST_RESULT_FLT(castedData->getRef().myDouble, L"\nGet an unexpected result!", expStruct1.myDouble, 1E-10);
  LOG_TEST_RESULT(castedData->getRef().myInt, L"\nGet an unexpected result!", expStruct1.myInt);
  LOG_TEST_RESULT_STRING(castedData->getRef().myString, L"\nGet an unexpected result!", expStruct1.myString);

  log(L"\nTest safe cast failed: cast an other type to my type...");
  auto castedData1 = MyTestString::safeCast(myData);
  LOG_TEST_RESULT_PTR(castedData1, L"\nGet an unexpected result!", nullptr);

  delete myData;
  return S_OK;
}
