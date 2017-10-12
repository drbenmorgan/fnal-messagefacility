#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#
add_library(MF_Utilities SHARED
  Category.h
  DebugMacros.cc
  DebugMacros.h
  ELextendedID.cc
  ELextendedID.h
  ELlimitsTable.cc
  ELlimitsTable.h
  ELlist.h
  ELmap.cc
  ELmap.h
  ELset.h
  ELseverityLevel.cc
  ELseverityLevel.h
  ELseverityLevel.icc
  EnabledState.h
  ErrorObj.cc
  ErrorObj.h
  ErrorObj.icc
  MessageFacilityMsg.cc
  MessageFacilityMsg.h
  SingleConsumerQ.cc
  SingleConsumerQ.h
  UnixSignalHandlers.cc
  UnixSignalHandlers.h
  bold_fontify.h
  eq_nocase.cc
  eq_nocase.h
  exception.cc
  exception.h
  formatTime.cc
  formatTime.h
  )
target_include_directories(MF_Utilities
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_Utilities
  cetlib::cetlib
  Threads::Threads
  )

# - Custom catch-main (NOT YET CLEAR WHAT THIS IS FOR...)
#add_library(mf_catch_main STATIC catch-main.cc)

# - Install
install(TARGETS MF_Utilities
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

