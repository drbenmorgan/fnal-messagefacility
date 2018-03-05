# Now just build single "MF_MessageLogger" lib
# from Message{Logger,Service} sources
add_library(MF_MessageLogger SHARED
  MessageLogger/MessageLogger.cc
  MessageLogger/MessageLogger.h
  MessageLogger/MessageLogger.icc
  MessageLogger/MessageLoggerDefinitions.h
  MessageLogger/MessageLoggerImpl.cc
  MessageLogger/MessageLoggerImpl.h
  MessageLogger/MessageLoggerScribe.cc
  MessageLogger/MessageLoggerScribe.h
  MessageLogger/MessageSender.cc
  MessageLogger/MessageSender.h
  MessageLogger/Presence.cc
  MessageLogger/Presence.h
  MessageService/AbstractMLscribe.h
  MessageService/ELadministrator.cc
  MessageService/ELadministrator.h
  MessageService/ELdestConfigCheck.h
  MessageService/ELdestMakerMacros.h
  MessageService/ELdestination.cc
  MessageService/ELdestination.h
  MessageService/ELostreamOutput.cc
  MessageService/ELostreamOutput.h
  MessageService/ELstatistics.cc
  MessageService/ELstatistics.h
  MessageService/MessageDrop.cc
  MessageService/MessageDrop.h
  MessageService/MessageLoggerQ.cc
  MessageService/MessageLoggerQ.h
  MessageService/MsgFormatSettings.cc
  MessageService/MsgFormatSettings.h
  MessageService/MsgStatistics.cc
  MessageService/MsgStatistics.h
  MessageService/OpCode.h
  MessageService/ThreadQueue.cc
  MessageService/ThreadQueue.h
  MessageService/default_destinations_config.h
  )
target_include_directories(MF_MessageLogger
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageLogger PUBLIC
  MF_Utilities
  cetlib_except::cetlib_except
  fhiclcpp::fhiclcpp
  TBB::tbb
  )

# Still recurse into directory to build ELdestinationTester program
add_subdirectory(MessageLogger)

# No longer anything to do in MessageService
#add_subdirectory(MessageService)

# Still build MF_Utilities
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

