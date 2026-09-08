#
# Project information
#
set(TARGET_PROJECT_NAME comm_axisprofile_test)
set(TARGET_PROJECT_NAME_GCOV ${TARGET_PROJECT_NAME}_gcov)

#
# The list of tests is automatically build.
#

# add recursive all cpp files from private/src/test/unittest
file( GLOB_RECURSE COMPONENTTEST_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/unittest/*.cpp )
# add recursive all cpp files from private/src/test/functiontest
file( GLOB_RECURSE FUNCTIONTEST_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/functiontest/*.cpp )
# add recursive all cpp files from public/include/comm/axisprofile/mockup
file( GLOB_RECURSE PUBLIC_MOCK_SRC_FILES ${CMAKE_SOURCE_DIR}/public/include/comm/axisprofile/mockup/*.cpp )
# add recursive all cpp files from private/src/test/mockup
file( GLOB_RECURSE PRIVATE_MOCK_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/mockup/*.cpp )
# add recursive all cpp files from private/src/test/helper
file( GLOB_RECURSE TESTHELPER_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/helper/*.cpp )


if(MSVC)
  # add recursive all cpp files from private/src/test/integrationtest when building the windows testbuddy dll
  file( GLOB_RECURSE INTEGRATIONTEST_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/integrationtest/*.cpp )
endif(MSVC)

#
# Set test related dependency
#
set(OPENSSL_DEPENDENCY_DIR ${DEPENDENCIES_ROOT}/oss.openssl)


if(MSVC)
  set( TEST_COMPONENT
    #some windows specific files
    ${CMAKE_CURRENT_LIST_DIR}/comm.axisprofile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdafx.cpp
  )
endif()

set( SOURCE_FILES
  ${TEST_COMPONENT}
  ${COMPONENTTEST_SRC_FILES}
  ${FUNCTIONTEST_SRC_FILES}
  ${INTEGRATIONTEST_SRC_FILES}
  ${TESTHELPER_SRC_FILES}
  ${TEST_FILES_ADDITIONAL}
  ${PRIVATE_MOCK_SRC_FILES}
  ${PUBLIC_MOCK_SRC_FILES}
)

# This is the list of exclude files from building
# add files to be excluded:
file(GLOB_RECURSE REMOVE_SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/template/*.cpp"
)
list(REMOVE_ITEM SOURCE_FILES ${REMOVE_SOURCES})

# This is the list of include dirs which gets exported, because it is also a dependency to the project using this lib.
set( PUBLIC_INCLUDE_DIRS
)

# This is the list of include dirs which is private to and not needed to use this lib.
set( PRIVATE_INCLUDE_DIRS
  .
  ../
  ${CMAKE_SOURCE_DIR}/private/dependencies/libbase/public/include
  ${TESTBUDDY_DEPENDENCY_DIR}/public/include
  ${TESTBUDDY_DEPENDENCY_DIR}/public/src/utility
  ${TESTBUDDY_DEPENDENCY_DIR}/public/src/linux
  ${CMAKE_SOURCE_DIR}/public/include
  ${CMAKE_SOURCE_DIR}/public/src
  ${ZMQ_DEPENDENCY_DIR}/public/include
  ${FLATBUFFER_DEPENDENCY_DIR}/public/include
  ${CMA_ETHERCAT_DEPENDENCY_DIR}/public/include
  ${SCHEDULER_DEPENDENCY_DIR}/public/include
  ${CMAKE_CURRENT_LIST_DIR}/unittest
  ${CMAKE_CURRENT_LIST_DIR}/functiontest
  ${CMAKE_CURRENT_LIST_DIR}/integrationtest
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/${MSC_TARGET_OS}
  ${CMAKE_SOURCE_DIR}/private/src/impl
  ${DATALAYER_DEPENDENCY_DIR}/public/include
  ${TRACE_DEPENDENCY_DIR}/public/include
  ${DIAGNOSIS_DEPENDENCY_DIR}/public/include
  ${CELIX_DEPENDENCY_DIR}/public/include
  ${AOX_DEPENDENCY_DIR}/public/include
  ${SYSTEMHANDLER_DEPENDENCY_DIR}/public/include
  ${BOOST_DEPENDENCY_DIR}
  ${MOTION_LIB_TEST_DEPENDENCY_DIR}/public/include
)

# This is the list of include definitions which gets exported, because it is also a dependency to the project using this lib.
set( PUBLIC_COMPILE_DEFS
  
)  
  
# This is the list of include definitions which is private to and not needed to use this lib.
set( PRIVATE_COMPILE_DEFS
  _CRT_SECURE_NO_WARNINGS
  UNICODE
)  

find_package (Threads)

#
# Generate shared library project
#
if(MSVC)
  #
  # standard debug / release target windows
  #
  add_library( ${TARGET_PROJECT_NAME}
    SHARED
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
    ${IDL_FILES}
    )

  add_dependencies(${TARGET_PROJECT_NAME} comm_axisprofile)
  set_target_properties(${TARGET_PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4819")
  target_include_directories ( ${TARGET_PROJECT_NAME}
    PUBLIC ${PUBLIC_INCLUDE_DIRS}
    PRIVATE ${PRIVATE_INCLUDE_DIRS}
    PRIVATE ${CMAKE_SOURCE_DIR}/private/src/bundle/win
  )

  target_compile_definitions ( ${TARGET_PROJECT_NAME}
    PUBLIC  ${PUBLIC_COMPILE_DEFS}
    PRIVATE ${PRIVATE_COMPILE_DEFS}
  )

  target_link_libraries ( ${TARGET_PROJECT_NAME}
    comm_axisprofile
    motion_lib_test
    comm_datalayer
    common_log_trace_buffered_static
    common_scheduler
    common_aox
    common_systemhandler
  )

else()

  #
  # standard debug / release target linux
  #
  add_executable( ${TARGET_PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
   )

  add_dependencies(${TARGET_PROJECT_NAME} comm_axisprofile)

  target_include_directories ( ${TARGET_PROJECT_NAME}
    PUBLIC ${PUBLIC_INCLUDE_DIRS}
    PRIVATE ${PRIVATE_INCLUDE_DIRS}
  )

  target_compile_definitions ( ${TARGET_PROJECT_NAME}
    PUBLIC  ${PUBLIC_COMPILE_DEFS}
    PRIVATE ${PRIVATE_COMPILE_DEFS}
  )

  target_link_libraries( ${TARGET_PROJECT_NAME}
    -rdynamic -g general
    comm_axisprofile
    motion_lib_test
    common_scheduler
    comm_datalayer
    common_aox
    ${TESTBUDDY_DEPENDENCY_DIR}/public/lib/${MSC_TARGET_NAME}/debug/libapp_testbuddy_static.a
    systemd
    pthread
    dl
    rt
  )

  if(CMAKE_BUILD_TYPE STREQUAL "debug")
    #
    # additional gcov debug target linux
    #
    add_executable(${TARGET_PROJECT_NAME_GCOV}
      ${INCLUDE_FILES}
      ${SOURCE_FILES}
    )

    add_dependencies(${TARGET_PROJECT_NAME_GCOV} comm_axisprofile_gcov)

    target_include_directories ( ${TARGET_PROJECT_NAME_GCOV}
      PUBLIC ${PUBLIC_INCLUDE_DIRS}
      PRIVATE ${PRIVATE_INCLUDE_DIRS}
    )
    
    target_compile_definitions ( ${TARGET_PROJECT_NAME_GCOV}
      PUBLIC  ${PUBLIC_COMPILE_DEFS}
      PRIVATE ${PRIVATE_COMPILE_DEFS}
    )
    
    target_compile_options( ${TARGET_PROJECT_NAME_GCOV} PRIVATE --coverage )
    
    target_link_libraries( ${TARGET_PROJECT_NAME_GCOV}
      -rdynamic -g general
      comm_axisprofile_gcov
      motion_lib_test
      common_scheduler
      comm_datalayer
      common_aox
      common_log_trace
      ${TESTBUDDY_DEPENDENCY_DIR}/public/lib/${MSC_TARGET_NAME}/debug/libapp_testbuddy_static.a
      systemd
      pthread
      gcov
      rt
      dl
    )

  endif()
endif()


#
# Install target dep. generator type
#
install(DIRECTORY
  ${CMAKE_SOURCE_DIR}/public/bfbs
  DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
)

if(MSVC)
  # Remove MSVC devtools prefix because we use Debug / Release folder
  set( CMAKE_DEBUG_POSTFIX "" )
  
  # multi-configuration generator (Visual Studio)
  install( TARGETS ${TARGET_PROJECT_NAME} CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    COMPONENT ${BUNDLE_PROJECT_NAME}
  )
  install( TARGETS ${TARGET_PROJECT_NAME} CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    COMPONENT ${BUNDLE_PROJECT_NAME}
  )

  # Copy shared libraries for tests to test folder
  file( GLOB _openssl_files ${OPENSSL_DEPENDENCY_DIR}/public/bin/win-msvc-${MSC_TARGET_CPU}/release/*.dll)
  install(FILES
    ${_openssl_files}
    ${DATALAYER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/comm_datalayer.dll
    ${TRACE_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/common_log_trace.dll
    ${TRACE_DEPENDENCY_DIR}/public/lib/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/common_log_trace_buffered_static.lib
    ${DIAGNOSIS_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/common_log_diagnosis.dll
    ${ZMQ_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/libzmq.dll
    ${SCHEDULER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/common_scheduler.dll
    ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/comm_axisprofile.dll
    ${AOX_LIBRARY_DIR}/common_aox.dll
    ${SYSTEMHANDLER_LIBRARY_DIR}/common_systemhandler.dll
    ${MOTION_LIB_TEST_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/motion_lib_test.dll
    DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  )

  if(CMAKE_BUILD_TYPE STREQUAL "debug")
    install( FILES
      ${TRACE_DEPENDENCY_DIR}/public/lib/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/common_log_trace_buffered_static.pdb
     ${MOTION_LIB_TEST_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/motion_lib_test.pdb
    DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE} )
  endif()

else()
  # single-configuration generator (Unix Makefile)
  install( TARGETS ${TARGET_PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    COMPONENT ${BUNDLE_PROJECT_NAME}
  )

  file(GLOB FLATBUFFERS_LIBRARY  ${FLATBUFFERS_LIBRARY_DIR}/libflatbuffers.so*)

  # Copy shared libraries for tests to test folder
  install(FILES
        ${ZMQ_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/libzmq.so.5
        ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/libcomm_axisprofile.so
        ${MOTION_LIB_TEST_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/libmotion_lib_test.so
    DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  )

  install(DIRECTORY
         ${SCHEDULER_LIBRARY_DIR}
         ${DATALAYER_LIBRARY_DIR}
         ${AOX_LIBRARY_DIR}
         ${TRACE_LIBRARY_DIR}
         ${DIAGNOSIS_LIBRARY_DIR}
         ${SYSTEMHANDLER_LIBRARY_DIR}
     DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}
  )

  if(CMAKE_BUILD_TYPE STREQUAL "debug")
    #
    # additional gcov debug target linux
    #
    install(TARGETS ${TARGET_PROJECT_NAME_GCOV}
      RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
      LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
      ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
      COMPONENT ${BUNDLE_PROJECT_NAME}
    )
  endif()
endif()

# Copy the generated header files for diagnosis and the header files of the repo to test folder (into sub folders "include" and "diagnosis")
install(DIRECTORY ${DIAGNOSTICS_FILES_DEPENDENCY_DIR}/public/include
  DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/public/src/diagnosis
  DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  FILES_MATCHING PATTERN "*.h"
)

