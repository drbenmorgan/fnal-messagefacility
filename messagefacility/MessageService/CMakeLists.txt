#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
set(MF_MessageService_SOURCES
  AbstractMLscribe.h
  ConfigurationHandshake.h
  ELadministrator.cc
  ELadministrator.h
  ELassertN.h
  ELcollected.cc
  ELcollected.h
  ELcontextSupplier.h
  ELdestConfigCheck.h
  ELdestControl.cc
  ELdestControl.h
  ELdestMakerMacros.h
  ELdestination.cc
  ELdestination.h
  ELerrorList.cc
  ELerrorList.h
  ELfwkJobReport.cc
  ELfwkJobReport.h
  ELostreamOutput.cc
  ELostreamOutput.h
  ELrecv.cc
  ELrecv.h
  ELsender.h
  ELstatistics.cc
  ELstatistics.h
  ELtsErrorLog.cc
  ELtsErrorLog.h
  ELtsErrorLog.icc
  ErrorLog.cc
  ErrorLog.h
  ErrorLog.icc
  HardwiredDefaults.cc
  MainThreadMLscribe.cc
  MainThreadMLscribe.h
  MessageDrop.cc
  MessageDrop.h
  MessageLoggerDefaults.cc
  MessageLoggerDefaults.h
  MessageLoggerQ.cc
  MessageLoggerQ.h
  MsgContext.cc
  MsgContext.h
  MsgFormatSettings.cc
  MsgFormatSettings.h
  MsgStatistics.cc
  MsgStatistics.h
  ThreadQueue.cc
  ThreadQueue.h
  ThreadSafeErrorLog.h
  )

# - Shared
add_library(MF_MessageService SHARED
  ${MF_MessageService_SOURCES}
  )
target_compile_features(MF_MessageService PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_MessageService
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageService
  MF_Auxiliaries MF_Utilities
  fhiclcpp::fhiclcpp
  Boost::thread
  )

# - Static
add_library(MF_MessageService-static STATIC
  ${MF_MessageService_SOURCES}
  )
set_target_properties(MF_MessageService-static
  PROPERTIES OUTPUT_NAME MF_MessageService
  )
target_compile_features(MF_MessageService-static PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_MessageService-static
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageService-static
  MF_Auxiliaries MF_Utilities
  fhiclcpp::fhiclcpp
  Boost::thread
  )

# - Install
install(TARGETS MF_MessageService MF_MessageService-static
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

# - Service plugins
add_subdirectory(plugins)


