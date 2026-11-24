#
# Set CMAKE version
#
cmake_minimum_required(VERSION 3.16)

#
# Project information
#
set(VERSION 21.0.8)
project(
  comm_axisprofile_project
  VERSION ${VERSION}
  DESCRIPTION "comm axisprofile system"
  LANGUAGES CXX
  )

#
# Set CMAKE Variables (UserSettings)
# Set CELIX_DEPENDENCY_DIR variable to the celix dependency directory, necessary to find the libraries
# Set CONTROLX_DEPENDENCY_DIR variable to the sysControlX dependency directory
# e.g. set(CELIX_DEPENDENCY_DIR ${CMAKE_SOURCE_DIR}/private/dependencies/libCelix)
set(DEPENDENCIES_ROOT ${CMAKE_SOURCE_DIR}/private/dependencies)

set(CELIX_DEPENDENCY_DIR           ${DEPENDENCIES_ROOT}/oss.celix)
set(CONTROLX_DEPENDENCY_DIR        ${DEPENDENCIES_ROOT}/automationframework)
set(FLATBUFFER_DEPENDENCY_DIR      ${DEPENDENCIES_ROOT}/oss.flatbuffers)
set(UBUNTU_PACKAGES_DEPENDENCY_DIR ${DEPENDENCIES_ROOT}/oss.ubuntu.packages)
set(TESTBUDDY_DEPENDENCY_DIR       ${DEPENDENCIES_ROOT}/apptestbuddy)
set(ZMQ_DEPENDENCY_DIR             ${DEPENDENCIES_ROOT}/oss.zmq)
set(DATALAYER_DEPENDENCY_DIR       ${DEPENDENCIES_ROOT}/bundle/comm.datalayer)
set(CMA_ETHERCAT_DEPENDENCY_DIR    ${DEPENDENCIES_ROOT}/bundle/comm.ethercat.master)
set(SCHEDULER_DEPENDENCY_DIR       ${DEPENDENCIES_ROOT}/bundle/common.scheduler)
set(TRACE_DEPENDENCY_DIR           ${DEPENDENCIES_ROOT}/bundle/common.log.trace)
set(DIAGNOSIS_DEPENDENCY_DIR       ${DEPENDENCIES_ROOT}/bundle/common.log.diagnosis)
set(AOX_DEPENDENCY_DIR             ${DEPENDENCIES_ROOT}/common.aox)
set(SYSTEMHANDLER_DEPENDENCY_DIR   ${DEPENDENCIES_ROOT}/bundle/common.systemhandler)
set(DIAGNOSTICS_FILES_DEPENDENCY_DIR  ${DEPENDENCIES_ROOT}/diagnostics.MOT.de_DE.en_US)
set(BOOST_DEPENDENCY_DIR           ${DEPENDENCIES_ROOT}/org.boost)
set(MOTION_LIB_TEST_DEPENDENCY_DIR ${DEPENDENCIES_ROOT}/motion.lib.test)

#
# Providing content of ubuntu package pool
#
if( UNIX )
  set(CDF_FOLDER "${UBUNTU_PACKAGES_DEPENDENCY_DIR}/public/cdf/comm/axisprofile")
  message(STATUS "CDF_FOLDER: ${CDF_FOLDER}")

  find_package(Python REQUIRED)
  execute_process(
    COMMAND ${Python_EXECUTABLE} ${UBUNTU_PACKAGES_DEPENDENCY_DIR}/public/helper/debhelper.py --folder "${CDF_FOLDER}"
    RESULT_VARIABLE PYTHON_RESULT
    OUTPUT_VARIABLE PYTHON_OUTPUT
    ERROR_VARIABLE PYTHON_ERROR
  )

  if (${PYTHON_RESULT})
    message(FATAL_ERROR ${PYTHON_ERROR})
  endif()
endif()

#
# systemd logger
#
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x64")
  set(UBUNTU_PACKAGES_LIBRARY_DIR ${UBUNTU_PACKAGES_DEPENDENCY_DIR}/public/composition/amd64/usr/lib/x86_64-linux-gnu/)
  set(LIBRARY_INCLUDES            ${UBUNTU_PACKAGES_DEPENDENCY_DIR}/public/composition/amd64/usr/include/ )
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(UBUNTU_PACKAGES_LIBRARY_DIR ${DEPENDENCIES_ROOT}/oss.ubuntu.packages/public/composition/arm64/usr/lib/aarch64-linux-gnu/)
  set(LIBRARY_INCLUDES ${UBUNTU_PACKAGES_DEPENDENCY_DIR}/public/composition/arm64/usr/include/ )
else()
  message(FATAL_ERROR "Unknown CPU System!")
endif()

#
# set libraries dirs and includes
#
set( CONTROLX_DIR                ${CONTROLX_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE} )
set( LIBRARY_DIR                 ${CONTROLX_DIR}/bin )
set( BUNDLE_DIR_CX               ${CONTROLX_DIR}/bundle )
set( BUNDLE_DIR_DEP              ${DEPENDENCIES_ROOT}/bundle )
set( FLATBUFFERS_LIBRARY_DIR     ${FLATBUFFER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( DATALAYER_LIBRARY_DIR       ${DATALAYER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( SCHEDULER_LIBRARY_DIR       ${SCHEDULER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( TRACE_LIBRARY_DIR           ${TRACE_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE} )
set( TRACE_LIBRARY_BUF_DIR       ${TRACE_DEPENDENCY_DIR}/public/lib/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( DIAGNOSIS_LIBRARY_DIR       ${DIAGNOSIS_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE} )
set( AOX_LIBRARY_DIR             ${AOX_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( ZMQ_LIBRARY_DIR             ${ZMQ_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( SYSTEMHANDLER_LIBRARY_DIR   ${SYSTEMHANDLER_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
set( MOTION_LIB_TEST_LIBRARY_DIR ${MOTION_LIB_TEST_DEPENDENCY_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})
#
# building bundles is currently supported for building in linux only
#
if( UNIX )
  include( ${CONTROLX_DEPENDENCY_DIR}/public/cmake/controlx_setting.cmake )
endif()

message( CONTROLX_DIR                = ${CONTROLX_DIR} )
message( LIBRARY_DIR                 = ${LIBRARY_DIR} )
message( BUNDLE_DIR_CX               = ${BUNDLE_DIR_CX} )
message( BUNDLE_DIR_DEP              = ${BUNDLE_DIR_DEP} )
message( FLATBUFFERS_LIBRARY_DIR     = ${FLATBUFFERS_LIBRARY_DIR})
message( UBUNTU_PACKAGES_LIBRARY_DIR = ${UBUNTU_PACKAGES_LIBRARY_DIR})

link_directories(
  ${LIBRARY_DIR}
  ${FLATBUFFERS_LIBRARY_DIR}
  ${UBUNTU_PACKAGES_LIBRARY_DIR}
  ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  ${CMAKE_SOURCE_DIR}/public/lib/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  ${DATALAYER_DEPENDENCY_DIR}
  ${TRACE_LIBRARY_DIR}
  ${TRACE_LIBRARY_BUF_DIR}
  ${DIAGNOSIS_LIBRARY_DIR}
  ${ZMQ_LIBRARY_DIR}
  ${SCHEDULER_LIBRARY_DIR}
  ${DATALAYER_LIBRARY_DIR}
  ${AOX_LIBRARY_DIR}
  ${SYSTEMHANDLER_LIBRARY_DIR}
  ${MOTION_LIB_TEST_LIBRARY_DIR}
)

#[[
See https://inside-docupedia.bosch.com/confluence/display/IAX/CX-ARC%3A+CORE+Platform #187
  - Enable all C++ Warnings
Once the warnings in dependent bundles are solved, /WX and -Werror should be enabled...
#]]
if(MSVC)
  add_compile_options(/W3) # /WX)
else()
  add_compile_options(-Wall -Wextra) # -Werror)
endif()


#
#
#
message( STATUS "============================================" )
message( STATUS "START of project ${PROJECT_NAME}" )
message( STATUS "============================================" )


#
# Building bundles is currently supported for building in linux only
#
if( !MSVC OR UNIX )
  add_celix_container( ${PROJECT_NAME}
    CXX
    BUNDLES_CX
  )
endif()

#
# Check if buildinfo.properties exists
#
if(EXISTS "${CMAKE_SOURCE_DIR}/buildinfo.properties")
  file(READ ${CMAKE_SOURCE_DIR}/buildinfo.properties TMPTXT)
  string(FIND "${TMPTXT}" "BUILDINFO_VERSION" MATCHRESULT)
  if(${MATCHRESULT} EQUAL -1)
    message( STATUS "Buildinfo version not found in buildinfo.properties" )
    file(APPEND ${CMAKE_SOURCE_DIR}/buildinfo.properties "BUILDINFO_VERSION = ${VERSION}")
  endif()
else()
  file(WRITE ${CMAKE_SOURCE_DIR}/buildinfo.properties "BUILDINFO_NAME =
BUILDINFO_ORG =
BUILDINFO_DATE =
BUILDINFO_TIME =
BUILDINFO_USERNAME =
BUILDINFO_BUILD_URL =
BUILDINFO_MACHINENAME = localbuild
BUILDINFO_GIT_COMMIT =
BUILDINFO_GIT_COMMITTER_NAME =
BUILDINFO_GIT_URL =
BUILDINFO_GIT_BRANCH =
BUILDINFO_VERSION = ${VERSION}
BUILDINFO_DOCKER_IMAGE =
")
endif()

#
# Loop over all bundles in controlX
#
set( BUNDLE_LIST "" )
file( GLOB_RECURSE BUNDLES_CX ${BUNDLE_DIR_CX}/*.zip )
foreach ( BUNDLE_CX ${BUNDLES_CX} )
  list( APPEND BUNDLE_LIST ${BUNDLE_CX} )
endforeach()

#
# add each bundle from dependencies, subfolder bundle
#
file(GLOB_RECURSE BUNDLES_DEP ${BUNDLE_DIR_DEP}/*/public/bundle/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}/*.zip)
foreach ( BUNDLE_DEP ${BUNDLES_DEP} )
  get_filename_component(BUNDLE_DEP_NAME ${BUNDLE_DEP} NAME)
  # remove controlX bundle from bundle list if exist...
  foreach ( BUNDLE ${BUNDLE_LIST} )
    get_filename_component(BUNDLE_NAME ${BUNDLE} NAME)
    if(${BUNDLE_DEP_NAME} STREQUAL "${BUNDLE_NAME}")
      list( REMOVE_ITEM BUNDLE_LIST ${BUNDLE} )
    endif()
  endforeach()
endforeach()
# ...and add bundle from subfolder
set(BUNDLE_LIST "${BUNDLE_LIST};${BUNDLES_DEP}")

#
# set flatbuffer root folder
#
set(FLATBUFFER_ROOT ${FLATBUFFER_DEPENDENCY_DIR})

# expand binary search path for flatbuffer compiler
if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
  set(MSC_HOST_CPU "x64")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x64")
  set(MSC_HOST_CPU "x64")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "AMD64")
  set(MSC_HOST_CPU "x64")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(MSC_HOST_CPU "aarch64")
else()
  set(MSC_HOST_CPU ${CMAKE_HOST_SYSTEM_PROCESSOR})
  message(SEND_ERROR "Unknown CPU System! Using fallback '${CMAKE_HOST_SYSTEM_PROCESSOR}' for 'MSC_HOST_CPU' @ ${CMAKE_CURRENT_LIST_LINE}:${CMAKE_CURRENT_LIST_FILE}.")
endif()

set(CMAKE_PROGRAM_PATH ${CMAKE_PROGRAM_PATH} ${FLATBUFFER_ROOT}/public/bin/${MSC_TARGET_OS}-${MSC_TARGET_COMPILER}-${MSC_HOST_CPU}/${CMAKE_BUILD_TYPE})
if(UNIX)
  # set executable attributes for flatbuffer compiler
  execute_process(COMMAND chmod +x ${FLATBUFFER_ROOT}/public/bin/${MSC_TARGET_OS}-${MSC_TARGET_COMPILER}-${MSC_HOST_CPU}/${CMAKE_BUILD_TYPE}/flatc)
endif()

# include flatbuffer cmake functions
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${FLATBUFFER_ROOT}/public/include)
include(${FLATBUFFER_ROOT}/public/CMake/FindFlatBuffers.cmake)


#
# Loop over all projects within sub-folders
#
# dependencies projects
file ( GLOB_RECURSE DEP_PROJECTS ${DEPENDENCIES_ROOT}/*/CMakeLists.txt )
list (FILTER DEP_PROJECTS EXCLUDE REGEX "org.boost")

# source projects
file ( GLOB_RECURSE SRC_PROJECTS ${CMAKE_SOURCE_DIR}/private/src/*/CMakeLists.txt )
# move source bundle projects build at the end
foreach ( PRJ ${SRC_PROJECTS} )
  message("**** handle " ${PRJ})
  if(PRJ MATCHES ".*bundle.*")
    message("**** matches " ${PRJ})
    list(REMOVE_ITEM SRC_PROJECTS "${PRJ}")
    set(BUNDLE_PROJECTS ${BUNDLE_PROJECTS} "${PRJ}")
  endif()
endforeach()
set(SRC_PROJECTS ${SRC_PROJECTS} ${BUNDLE_PROJECTS})

# projects in dependencies has to be built at beginning
list ( APPEND PROJECTS ${DEP_PROJECTS} ${SRC_PROJECTS} )
foreach ( PRJ ${PROJECTS} )
  message( STATUS "" )
  message( STATUS "============================================" )
  message( STATUS "Creating project for ${PRJ}" )
  message( STATUS "============================================" )

  get_filename_component( PRJ_PATH ${PRJ} DIRECTORY )
  message( STATUS " adding project " ${PRJ_PATH} )
  add_subdirectory( ${PRJ_PATH} )
endforeach()

#
# bundle list will be modified by bundles in sub directories (above, search and replace)
#
message( BUNDLE_LIST= ${BUNDLE_LIST} )

#
# reverse bundle list to check bundle dependencies
#
list(REVERSE BUNDLE_LIST)
message( BUNDLE_LIST= ${BUNDLE_LIST} )

#
# Add bundles to existing config.properties
#
if( UNIX )
  include(${CONTROLX_DEPENDENCY_DIR}/public/cmake/ctrlx_config_properties.cmake)
  ctrlx_config_properties(OPEN FILE "${CONTROLX_DIR}/config.properties")

  foreach(BUNDLE ${BUNDLE_LIST})
    ctrlx_config_properties(BUNDLE_ADD "${BUNDLE}" PREPEND)
  endforeach()

  ctrlx_config_properties(CLOSE)
endif()


#
#
#
message( STATUS "============================================" )
message( STATUS "END of project ${PROJECT_NAME}" )
message( STATUS "============================================" )
