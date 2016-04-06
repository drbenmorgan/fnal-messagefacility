add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)

# Handle installation of headers and support files here, as the current
# structure of MF is awkward (and appears to only be so for legacy reasons)
# - Awkwardnesses are:
#   - Utilities built seperately, but no need to do so
#   - MF_MessageLogger built in MessageLogger, but uses sources from
#     MessageService
#   - EldestinationTester in MessageService, but not part of Library

# Install directory for headers - do this way and here as we don't have
# optional headers so no filtering/selection required, except
# that we need to exclude several cruft directories
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.icc"
  PATTERN "ELdestinationTester.h" EXCLUDE
  PATTERN "doc" EXCLUDE
  PATTERN "examples" EXCLUDE
  PATTERN "python" EXCLUDE
  PATTERN "bin" EXCLUDE
  PATTERN ".admin" EXCLUDE
  PATTERN "plugins" EXCLUDE
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

