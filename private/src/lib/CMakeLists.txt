#
# Set up source files and header
#
set( TARGET_PROJECT_NAME_SHARED comm_axisprofile )
set( TARGET_PROJECT_NAME_SHARED_GCOV ${TARGET_PROJECT_NAME_SHARED}_gcov )

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if( UNIX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

file( GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/private/src/impl/*.cpp)

# This is the list of exclude files from building
# add files to be excluded, e.g.:
file(GLOB_RECURSE REMOVE_SOURCES

)
if ( ${REMOVE_SOURCES} )
  list(REMOVE_ITEM SOURCE_FILES ${REMOVE_SOURCES})
endif()

include_directories(
  PRIVATE ${CMAKE_SOURCE_DIR}/private/src/impl
  PRIVATE ${CMAKE_SOURCE_DIR}/public/include
  PRIVATE ${CMAKE_SOURCE_DIR}/public/src
  PRIVATE ${CONTROLX_DEPENDENCY_DIR}/public/include
  PRIVATE ${CELIX_DEPENDENCY_DIR}/public/include
  PRIVATE ${DATALAYER_DEPENDENCY_DIR}/public/include
  PRIVATE ${ZMQ_DEPENDENCY_DIR}/public/include
  PRIVATE ${FLATBUFFER_DEPENDENCY_DIR}/public/include
  PRIVATE ${FLATBUFFER_DEPENDENCY_DIR}/public/include
  PRIVATE ${CMA_ETHERCAT_DEPENDENCY_DIR}/public/include
  PRIVATE ${SCHEDULER_DEPENDENCY_DIR}/public/include
  PRIVATE ${TRACE_DEPENDENCY_DIR}/public/include
  PRIVATE ${DIAGNOSIS_DEPENDENCY_DIR}/public/include
  PRIVATE ${AOX_DEPENDENCY_DIR}/public/include
  PRIVATE ${SYSTEMHANDLER_DEPENDENCY_DIR}/public/include
  PRIVATE ${BOOST_DEPENDENCY_DIR}
)

# **********************************************************************
# flatbuffers files
# **********************************************************************

message( STATUS "" )
message( STATUS "Adding FBS FILES: ")
# Loop over all fbs files
set( IDL_FILES "" )
file( GLOB_RECURSE IDL_FILE ${CMAKE_SOURCE_DIR}/public/fbs/comm/axisprofile/*.fbs )
foreach ( IDL_FILE ${IDL_FILE} )
  list( APPEND IDL_FILES ${IDL_FILE} )
  message(STATUS ${IDL_FILE})
endforeach()

# build bfbs for bundle zip
build_flatbuffers( "${IDL_FILES}"
                   "${FLATBUFFER_DEPENDENCY_DIR}/public/include/flatbuffers/"
                   comm_axisprofile_Flatbuffers
                   "${FLATBUFFERS_LIBRARY_DIR}"
                   ""
                   "${CMAKE_BINARY_DIR}/celix/gen/bundles/com.boschrexroth.comm.axisprofile/content/bfbs"
                   ""
)

# build bfbs and *_generated.h for public
build_flatbuffers( "${IDL_FILES}"
                   "${FLATBUFFER_DEPENDENCY_DIR}/public/include/flatbuffers/"
                   comm_axisprofile_Flatbuffers_public
                   "${FLATBUFFERS_LIBRARY_DIR}"
                   "${CMAKE_SOURCE_DIR}/public/include/comm/axisprofile/datatypes"
                   "${CMAKE_SOURCE_DIR}/public/bfbs/comm/axisprofile"
                   ""
)
set_target_properties(${comm_axisprofile_Flatbuffers} PROPERTIES COMPILE_FLAGS "/wd4819")
set_target_properties(${comm_axisprofile_Flatbuffers_public} PROPERTIES COMPILE_FLAGS "/wd4819")
get_target_property(GENERATED_INCLUDES_DIR comm_axisprofile_Flatbuffers_public GENERATED_INCLUDES_DIR)
get_target_property(BINARY_SCHEMAS_DIR comm_axisprofile_Flatbuffers_public BINARY_SCHEMAS_DIR)
get_target_property(COPY_TEXT_SCHEMAS_DIR comm_axisprofile_Flatbuffers_public COPY_TEXT_SCHEMAS_DIR)

message( STATUS "============================================" )
message( STATUS "GENERATED_INCLUDES_DIR:  ${GENERATED_INCLUDES_DIR}" )
message( STATUS "BINARY_SCHEMAS_DIR:      ${BINARY_SCHEMAS_DIR}" )
message( STATUS "COPY_TEXT_SCHEMAS_DIR:   ${COPY_TEXT_SCHEMAS_DIR}" )
message( STATUS "============================================" )

# **********************************************************************
# flatbuffers files end
# **********************************************************************

if(MSVC)
  #
  # standard debug / release target windows
  #
  add_library(${TARGET_PROJECT_NAME_SHARED} SHARED ${SOURCE_FILES})

  add_dependencies(${TARGET_PROJECT_NAME_SHARED} comm_axisprofile_Flatbuffers)
  add_dependencies(${TARGET_PROJECT_NAME_SHARED} comm_axisprofile_Flatbuffers_public)
  include_directories(
    PRIVATE ${CMAKE_SOURCE_DIR}/private/src/bundle/win
  )

  # don't set RPATH in shared library of activator because possibility wrong build paths in *.so
  set_target_properties(${TARGET_PROJECT_NAME_SHARED} PROPERTIES SKIP_BUILD_RPATH true)
  set_target_properties(${TARGET_PROJECT_NAME_SHARED} PROPERTIES COMPILE_FLAGS "/wd4819")
  target_link_libraries( ${TARGET_PROJECT_NAME_SHARED}
    comm_datalayer
    common_log_trace_buffered_static
    common_aox
  )
else()
  #
  # standard debug / release target linux
  #
  add_library(${TARGET_PROJECT_NAME_SHARED} SHARED ${SOURCE_FILES})

  add_dependencies(${TARGET_PROJECT_NAME_SHARED} comm_axisprofile_Flatbuffers)
  add_dependencies(${TARGET_PROJECT_NAME_SHARED} comm_axisprofile_Flatbuffers_public)

  # don't set RPATH in shared library of activator because possibility wrong build paths in *.so
  set_target_properties(${TARGET_PROJECT_NAME_SHARED} PROPERTIES SKIP_BUILD_RPATH true)

  target_link_libraries(${TARGET_PROJECT_NAME_SHARED}
    common_log_trace_buffered_static
    common_aox
    pthread
    systemd
    rt
    dl
  )

  if(CMAKE_BUILD_TYPE STREQUAL "debug")
    #
    # additional gcov debug target linux
    #
    add_library(${TARGET_PROJECT_NAME_SHARED_GCOV} SHARED ${SOURCE_FILES})

    add_dependencies(${TARGET_PROJECT_NAME_SHARED_GCOV} comm_axisprofile_Flatbuffers)
    add_dependencies(${TARGET_PROJECT_NAME_SHARED_GCOV} comm_axisprofile_Flatbuffers_public)

    # don't set RPATH in shared library of activator because possibility wrong build paths in *.so
    set_target_properties(${TARGET_PROJECT_NAME_SHARED_GCOV} PROPERTIES SKIP_BUILD_RPATH true)

    target_compile_options(${TARGET_PROJECT_NAME_SHARED_GCOV} PRIVATE --coverage)

    target_link_libraries(${TARGET_PROJECT_NAME_SHARED_GCOV}
      common_log_trace_buffered_static
      common_aox
      gcov
      pthread
      systemd
      rt
      dl
    )
  endif()
endif()

#
# Install target dep. generator type
#
if(MSVC)
  # Remove MSVC devtools prefix because we use Debug / Release folder
  set( CMAKE_DEBUG_POSTFIX "" )

  # multi-configuration generator (Visual Studio)
  install( TARGETS ${TARGET_PROJECT_NAME_SHARED} CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
  )

  # copy pdb file to public (needed for debugging within a dll)
  install(
    FILES $<TARGET_PDB_FILE:${TARGET_PROJECT_NAME_SHARED}>
    DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/debug
    OPTIONAL
  )

  # Copy diagnosis.json file to public resources
  install(FILES
    ${DIAGNOSTICS_FILES_DEPENDENCY_DIR}/public/src/comm_axisprofile_diagnostics_en-US.json
    DESTINATION ${CMAKE_SOURCE_DIR}/public/resources/diagnostics
  )

else()
  # single-configuration generator (Unix Makefile)

  install( TARGETS ${TARGET_PROJECT_NAME_SHARED}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/public/bin/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
    COMPONENT ${BUNDLE_PROJECT_NAME}
  )

  if(CMAKE_BUILD_TYPE STREQUAL "debug")
    #
    # additional gcov debug target linux
    #
    install(TARGETS ${TARGET_PROJECT_NAME_SHARED_GCOV}
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
        LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
        ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/private/generated/test/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE}
        COMPONENT ${BUNDLE_PROJECT_NAME}
    )
  endif()
endif()
