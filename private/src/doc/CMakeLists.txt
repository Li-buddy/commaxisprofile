#
# Generate Doxygen documentation
#
if(UNIX)
  message(STATUS "Doxygen disabled for Linux")
  return()
endif()

# Define project
project("${PROJECT_NAME}_docs"
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "${PROJECT_DESCRIPTION} Documentation"
  LANGUAGES NONE)

include("$ENV{DEVTOOLS}/tool_doxygen/doxygen.cmake")

# interface file to be generated doc
set(_interface_files
  "${CMAKE_SOURCE_DIR}/public/include/comm/axisprofile/defines"
  "${CMAKE_SOURCE_DIR}/public/include/comm/axisprofile/interface")

# dependency header files from external
set(_related_interface_files)
file(GLOB header_files_external
  "${DATALAYER_DEPENDENCY_DIR}/public/include/comm/datalayer/*.h"
  "${CMA_ETHERCAT_DEPENDENCY_DIR}/public/include/comm/ethercat/master/*.h"
  "${SCHEDULER_DEPENDENCY_DIR}/public/include/common/scheduler/*.h"
  "${TRACE_DEPENDENCY_DIR}/public/include/common/log/trace/*.h"
  "${DIAGNOSIS_DEPENDENCY_DIR}/public/include/common/log/diagnosis/*.h"
  "${SYSTEMHANDLER_DEPENDENCY_DIR}/public/include/common/scheduler/*.h")
list(APPEND _related_interface_files ${header_files_external})

# dependency header files from internal
set(_related_interface_files_internal)
file(GLOB header_files_private
  "${CMAKE_SOURCE_DIR}/private/src/impl/*/*.h"
  "${CMAKE_SOURCE_DIR}/public/include/comm/axisprofile/*/*.h")

# all dependency header files
list(APPEND _related_interface_files_internal
  ${_related_interface_files}
  ${header_files_private})

set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)

generate_doc(
  INTERFACE
  IGNORE_FAILURE
  DESTINATION
  "${CMAKE_SOURCE_DIR}/public/doc"
  MAIN_PAGE
  "${CMAKE_SOURCE_DIR}/readme.md"
  CODE_PATH
  ${_interface_files}
  RELATED_INTERFACE_FILES
  ${_related_interface_files})

generate_doc(
  SOURCE_CODE
  IGNORE_FAILURE
  DESTINATION
  "${CMAKE_SOURCE_DIR}/private/generated/doc"
  MAIN_PAGE
  "${CMAKE_SOURCE_DIR}/readme.md"
  CODE_PATH
  "${CMAKE_SOURCE_DIR}/private/src"
  RELATED_INTERFACE_FILES
  ${_related_interface_files_internal})
