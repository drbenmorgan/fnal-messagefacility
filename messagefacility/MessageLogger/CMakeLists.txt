#-----------------------------------------------------------------------
# MessageLogger Library
set(MF_MessageLogger_Logger_SOURCES
  ErrorSummaryEntry.h
  ExceptionMessages.cc
  ExceptionMessages.h
  JobReport.cc
  JobReport.h
  LoggedErrorsSummary.cc
  LoggedErrorsSummary.h
  MessageLogger.cc
  MessageLogger.h
  MessageLoggerImpl.cc
  MessageLoggerImpl.h
  MessageLoggerScribe.cc
  MessageLoggerScribe.h
  MessageSender.cc
  MessageSender.h
  MessageServicePresence.cc
  MessageServicePresence.h
  Presence.cc
  Presence.h
  SilentMLscribe.cc
  SilentMLscribe.h
  SingleThreadMSPresence.cc
  SingleThreadMSPresence.h
  ThreadSafeErrorLog.icc
  )

# - Shared
add_library(MF_MessageLogger SHARED
  ${MF_MessageLogger_Logger_SOURCES}
  )
target_compile_features(MF_MessageLogger PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_MessageLogger
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageLogger
  MF_MessageService
  fhiclcpp::fhiclcpp
  Boost::thread
  )

# - Static
add_library(MF_MessageLogger-static STATIC
  ${MF_MessageLogger_Logger_SOURCES}
  ${MF_MessageLogger_Service_SOURCES}
  )
set_target_properties(MF_MessageLogger-static
  PROPERTIES OUTPUT_NAME MF_MessageLogger
  )
target_compile_features(MF_MessageLogger-static PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_MessageLogger-static
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageLogger-static
  MF_MessageService-static
  fhiclcpp::fhiclcpp
  Boost::thread
  )


# - ELdestinationTester exe
add_executable(ELdestinationTester ELdestinationTester.cc ELdestinationTester.h)
target_link_libraries(ELdestinationTester
  MF_MessageLogger
  Boost::filesystem
  Boost::program_options
  )

# - Install
install(TARGETS MF_MessageLogger MF_MessageLogger-static ELdestinationTester
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )


