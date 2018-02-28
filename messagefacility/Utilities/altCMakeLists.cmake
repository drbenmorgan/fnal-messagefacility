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

# - Custom catch-main
# Think about how to vendor catch header...
# Install in messagefacility/catch.hpp?
# It's then namespaced correctly, and users writing
# tests to use MF use exactly the right one.
add_library(mf_catch_main STATIC catch_main.cc)
target_include_directories(mf_catch_main
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
# Uses cetlib, fhicl-cpp and messagelogger privately,
# but must link to use!
target_link_libraries(mf_catch_main PRIVATE
  MF_MessageLogger
  cetlib::cetlib
  fhiclcpp::fhiclcpp
  )
# Always strip to minimize space (we don't need to debug catch!)
add_custom_command(TARGET mf_catch_main
  POST_BUILD COMMAND strip -S $<TARGET_FILE:mf_catch_main>
  )
# - Custom install of vendored catch header
install(FILES catch/catch.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)

# - Install
install(TARGETS MF_Utilities mf_catch_main
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

