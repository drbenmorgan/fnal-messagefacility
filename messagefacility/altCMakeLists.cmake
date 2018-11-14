# Now just build single "MF_MessageLogger" lib
# from Message{Logger,Service} sources
add_library(MF_MessageLogger SHARED
  MessageLogger/MFConfig.cc
  MessageLogger/MFConfig.h
  MessageLogger/MessageLogger.cc
  MessageLogger/MessageLogger.h
  MessageService/ELdestination.cc
  MessageService/ELdestination.h
  MessageService/ELostreamOutput.cc
  MessageService/ELostreamOutput.h
  MessageService/ELstatistics.cc
  MessageService/ELstatistics.h
  Utilities/ELextendedID.cc
  Utilities/ELextendedID.h
  Utilities/ELseverityLevel.cc
  Utilities/ELseverityLevel.h
  Utilities/ErrorObj.cc
  Utilities/ErrorObj.h
  Utilities/exception.cc
  Utilities/exception.h

  )
target_include_directories(MF_MessageLogger
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageLogger PUBLIC
  cetlib_except::cetlib_except
  fhiclcpp::fhiclcpp
  hep_concurrency::hep_concurrency
  TBB::tbb
  Threads::Threads
  )

# For mf_cactch_main
add_subdirectory(Utilities)

# Plugins
add_subdirectory(plugins)

# Testing
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

#-----------------------------------------------------------------------
# Installation
#
install(TARGETS MF_MessageLogger
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

# Handle installation of headers and support files here, as the current
# Install directory for headers - do this way and here as we don't have
# optional headers so no filtering/selection required, except
# that we need to exclude the ELdestinationTester and catch headers and several cruft directories
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.icc"
  PATTERN "ELdestinationTester.h" EXCLUDE
  PATTERN "catch" EXCLUDE
  PATTERN "doc" EXCLUDE
  PATTERN "examples" EXCLUDE
  PATTERN "python" EXCLUDE
  PATTERN "bin" EXCLUDE
  PATTERN ".admin" EXCLUDE
  PATTERN "test" EXCLUDE
  )

# Support files
#-----------------------------------------------------------------------
# Create exports file(s)
include(CMakePackageConfigHelpers)

# - Common to both trees
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
  )

# - Build tree (EXPORT only for now, config file needs some thought,
#   dependent on the use of multiconfig)
export(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  )

# - Install tree
configure_package_config_file("${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/InstallCMakeFiles/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  )

install(
  FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/InstallCMakeFiles/${PROJECT_NAME}Config.cmake"
  DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

install(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  )

