#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerS
#

set(MF_MessageLogger_Logger_SOURCES
  AbstractMLscribe.cc
  AbstractMLscribe.h
  ConfigurationHandshake.h
  ELextendedID.cc
  ELextendedID.h
  ELlist.h
  ELmap.cc
  ELmap.h
  ELseverityLevel.cc
  ELseverityLevel.h
  ELseverityLevel.icc
  ELstring.cc
  ELstring.h
  ErrorObj.cc
  ErrorObj.h
  ErrorObj.icc
  ErrorSummaryEntry.h
  ExceptionMessages.cc
  ExceptionMessages.h
  JobReport.cc
  JobReport.h
  LoggedErrorsSummary.cc
  LoggedErrorsSummary.h
  MessageDrop.cc
  MessageDrop.h
  MessageFacilityMsg.cc
  MessageFacilityMsg.h
  MessageLogger.cc
  MessageLogger.h
  MessageLoggerQ.cc
  MessageLoggerQ.h
  MessageSender.cc
  MessageSender.h
  SilentMLscribe.cc
  SilentMLscribe.h
  ThreadSafeErrorLog.icc
  )

set(MF_MessageLogger_Service_SOURCES
  ../MessageService/ELadministrator.cc
  ../MessageService/ELadministrator.h
  ../MessageService/ELassertN.h
  ../MessageService/ELcollected.cc
  ../MessageService/ELcollected.h
  ../MessageService/ELcontextSupplier.h
  ../MessageService/ELdestConfigCheck.h
  ../MessageService/ELdestControl.cc
  ../MessageService/ELdestControl.h
  ../MessageService/ELdestMakerMacros.h
  ../MessageService/ELdestination.cc
  ../MessageService/ELdestination.h
  #../MessageService/ELdestinationTester.cc
  #../MessageService/ELdestinationTester.h
  ../MessageService/ELerrorList.cc
  ../MessageService/ELerrorList.h
  ../MessageService/ELfwkJobReport.cc
  ../MessageService/ELfwkJobReport.h
  ../MessageService/ELlimitsTable.cc
  ../MessageService/ELlimitsTable.h
  ../MessageService/ELostreamOutput.cc
  ../MessageService/ELostreamOutput.h
  ../MessageService/ELrecv.cc
  ../MessageService/ELrecv.h
  ../MessageService/ELsender.h
  ../MessageService/ELset.h
  ../MessageService/ELstatistics.cc
  ../MessageService/ELstatistics.h
  ../MessageService/ELtsErrorLog.cc
  ../MessageService/ELtsErrorLog.h
  ../MessageService/ELtsErrorLog.icc
  ../MessageService/ErrorLog.cc
  ../MessageService/ErrorLog.h
  ../MessageService/ErrorLog.icc
  ../MessageService/HardwiredDefaults.cc
  ../MessageService/MainThreadMLscribe.cc
  ../MessageService/MainThreadMLscribe.h
  ../MessageService/MessageLogger.cc
  ../MessageService/MessageLogger.h
  ../MessageService/MessageLoggerDefaults.cc
  ../MessageService/MessageLoggerDefaults.h
  ../MessageService/MessageLoggerScribe.cc
  ../MessageService/MessageLoggerScribe.h
  ../MessageService/MessageServicePresence.cc
  ../MessageService/MessageServicePresence.h
  ../MessageService/MsgContext.cc
  ../MessageService/MsgContext.h
  ../MessageService/MsgFormatSettings.cc
  ../MessageService/MsgFormatSettings.h
  ../MessageService/MsgStatistics.cc
  ../MessageService/MsgStatistics.h
  ../MessageService/Presence.cc
  ../MessageService/Presence.h
  ../MessageService/SingleThreadMSPresence.cc
  ../MessageService/SingleThreadMSPresence.h
  ../MessageService/ThreadQueue.cc
  ../MessageService/ThreadQueue.h
  ../MessageService/ThreadSafeErrorLog.h
  )

# - Shared
add_library(MF_MessageLogger SHARED
  ${MF_MessageLogger_Logger_SOURCES}
  ${MF_MessageLogger_Service_SOURCES}
  )
target_compile_features(MF_MessageLogger PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_MessageLogger
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_MessageLogger
  MF_Utilities
  fhiclcpp::fhiclcpp
  cetlib::cetlib
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
  MF_Utilities-static
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  Boost::thread
  )












#FILE (GLOB MLsrcFiles *.cc)
#FILE (GLOB MSsrcFiles ../MessageService/*.cc)
#
#LIST ( REMOVE_ITEM MSsrcFiles ${CMAKE_CURRENT_SOURCE_DIR}/../MessageService/ELdestinationTester.cc )
#
#cet_make_library( LIBRARY_NAME MF_MessageLogger
#                  SOURCE  ${MLsrcFiles} ${MSsrcFiles}
#        	  LIBRARIES
#                  MF_Utilities
#                  fhiclcpp
#                  ${Boost_THREAD_LIBRARY} )
#
# install files
#install_headers()
#install_source()
