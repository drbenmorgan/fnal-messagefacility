#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#
add_library(MF_Utilities SHARED
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
  eq_nocase.cc
  eq_nocase.h
  exception.cc
  exception.h
  formatTime.cc
  formatTime.h
  )
target_compile_features(MF_Utilities
  PUBLIC
    ${messagefacility_COMPILE_FEATURES}
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

# - Install
install(TARGETS MF_Utilities
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

