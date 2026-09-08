#
# Building bundles is currently supported for building in linux only
#
if(UNIX)

  #
  # Part of Manifestfile
  #
  set(BUNDLEX_NAME "com.boschrexroth.comm.axisprofile")
  set(BUNDLEX_DISPLAY_NAME "com_boschrexroth_comm_axisprofile")
  set(BUNDLEX_VERSION ${VERSION}) #Set at CMakeLists.txt can be modified if more than one bundle is deployed
  set(BUNDLEX_DESCRIPTION "ControlX axis profile.")

  #
  # Set up source files and header
  #
  set(ACTIVATOR_NAME "axisprofile_activator")
  set(BUNDLEX_PUBLIC_INSTALL_DIR ${CMAKE_SOURCE_DIR}/public/bundle/${MSC_TARGET_NAME}/${CMAKE_BUILD_TYPE})

  add_library(${ACTIVATOR_NAME} SHARED
    activator.cpp
    comm_axisprofile_bundle.cpp
  )

  #
  # link static dependency manager to activator
  #
  include(bundlex)

  #
  # build dependencies before build activator and bundle
  #
  add_dependencies( ${ACTIVATOR_NAME} comm_axisprofile )

  # don't set RPATH in shared library of activator because possibility wrong build paths in *.so
  set_target_properties(${ACTIVATOR_NAME} PROPERTIES SKIP_BUILD_RPATH true)

  #
  #
  #
  target_include_directories( ${ACTIVATOR_NAME}
    PRIVATE ${CONTROLX_DEPENDENCY_DIR}/public/include/
    PRIVATE ${DATALAYER_DEPENDENCY_DIR}/public/include/
    PRIVATE ${TRACE_DEPENDENCY_DIR}/public/include
    PRIVATE ${DIAGNOSIS_DEPENDENCY_DIR}/public/include
    PRIVATE ${CMAKE_SOURCE_DIR}/private/src/bundle
    PRIVATE ${CMAKE_SOURCE_DIR}/private/src/impl
    PRIVATE ${CMAKE_SOURCE_DIR}/public/include
    PRIVATE ${CMAKE_SOURCE_DIR}/public/src
    PRIVATE ${ZMQ_DEPENDENCY_DIR}/public/include
    PRIVATE ${CELIX_DEPENDENCY_DIR}/public/include
    PRIVATE ${FLATBUFFER_DEPENDENCY_DIR}/public/include
    PRIVATE ${CMA_ETHERCAT_DEPENDENCY_DIR}/public/include
    PRIVATE ${SCHEDULER_DEPENDENCY_DIR}/public/include
    PRIVATE ${AOX_DEPENDENCY_DIR}/public/include
    PRIVATE ${SYSTEMHANDLER_DEPENDENCY_DIR}/public/include
  )

  # Comment / uncomment STATIC libraries of components here
  set(CMP_STATIC_LIB_LIST
    comm_axisprofile_static
  )
  foreach ( CMP_STATIC_LIB ${CMP_STATIC_LIB_LIST} )
    list(APPEND CMP_STATIC_LIB_LIST_MOD -Wl,--whole-archive ${CMP_STATIC_LIB})
  endforeach()
  list(APPEND CMP_STATIC_LIB_LIST_MOD -Wl,--no-whole-archive)

  target_link_libraries(${ACTIVATOR_NAME}
    # Comment / uncomment SHARED libraries of components here
    comm_axisprofile

    # Comment / uncomment STATIC libraries of components here
    #${CMP_STATIC_LIB_LIST_MOD}
  )

  add_celix_bundle(${BUNDLEX_NAME}
    SYMBOLIC_NAME ${BUNDLEX_DISPLAY_NAME}
    VERSION ${BUNDLEX_VERSION}
    ACTIVATOR ${ACTIVATOR_NAME}
    DESCRIPTION ${BUNDLEX_DESCRIPTION}
    EXPORT_LIBRARIES
      # Comment / uncomment SHARED libraries of components here
      # Note: EXPORT_LIBRARIES are loaded before activator is loaded!
      # Note: EXPORT_LIBRARIES be aware about the load order (dependencies)!
      comm_axisprofile
  )

  #
  # Add buildinfo.properties to bundle if exist
  #
  celix_bundle_files( ${BUNDLEX_NAME}
    ${CMAKE_SOURCE_DIR}/buildinfo.properties
    DESTINATION "META-INF"
  )

  #
  # Copy diagnosis JSON file to bundle resources
  #
  celix_bundle_files(${BUNDLEX_NAME}
    ${DIAGNOSTICS_FILES_DEPENDENCY_DIR}/public/src/comm_axisprofile_diagnostics_en-US.json
    DESTINATION "resources/diagnostics"
  )

  #
  # Install target dep. generator type
  #
  # single-configuration generator (Unix Makefile)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLEX_NAME}.zip DESTINATION ${BUNDLEX_PUBLIC_INSTALL_DIR}/)

  # remove current bundle name from list if exist...
  foreach ( BUNDLE ${BUNDLE_LIST} )
    get_filename_component(LOCAL_BUNDLE_NAME ${BUNDLE} NAME)
    if(NOT ${LOCAL_BUNDLE_NAME} STREQUAL "${BUNDLEX_NAME}.zip")
      set(BUNDLE_LIST_TEMP "${BUNDLE_LIST_TEMP};${BUNDLE}")
    endif()
  endforeach()
  # ...and add current bundle name
  set(BUNDLE_LIST "${BUNDLE_LIST_TEMP};${BUNDLEX_PUBLIC_INSTALL_DIR}/${BUNDLEX_NAME}.zip" PARENT_SCOPE)
else()
  message( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
  message( STATUS "Creating a bundle is currently only supported with linux environment" )
  message( STATUS "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
endif()
