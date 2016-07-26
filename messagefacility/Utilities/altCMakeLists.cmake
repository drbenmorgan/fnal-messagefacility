#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#
set(messagefacility_utilities_SOURCES
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
  ErrorObj.cc
  ErrorObj.h
  ErrorObj.icc
  MessageFacilityMsg.cc
  MessageFacilityMsg.h
  SingleConsumerQ.cc
  SingleConsumerQ.h
  UnixSignalHandlers.cc
  UnixSignalHandlers.h
  do_nothing_deleter.h
  eq_nocase.cc
  eq_nocase.h
  exception.cc
  exception.h
  formatTime.cc
  formatTime.h
  possiblyAbortOrExit.h
  tinystr.h
  tinyxml.cc
  tinyxml.h
  tinyxmlerror.cc
  tinyxmlparser.cc
  )


# - Shared
add_library(MF_Utilities SHARED
  ${messagefacility_utilities_SOURCES}
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
target_link_libraries(MF_Utilities cetlib::cetlib)

# - Static
add_library(MF_Utilities-static STATIC
  ${messagefacility_utilities_SOURCES}
  )
set_target_properties(MF_Utilities-static
  PROPERTIES OUTPUT_NAME MF_Utilities
  )
target_compile_features(MF_Utilities-static PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_Utilities-static
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_Utilities-static cetlib::cetlib)

# - Install
install(TARGETS MF_Utilities MF_Utilities-static
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

#cet_make( LIBRARY_NAME MF_Utilities
#          LIBRARIES cetlib
#          ${Boost_FILESYSTEM_LIBRARY}
#          ${Boost_REGEX_LIBRARY}
#          ${Boost_SYSTEM_LIBRARY}
#          ${Boost_THREAD_LIBRARY}
#          -lpthread
#          WITH_STATIC_LIBRARY )
#
# install files
#install_headers()
#install_source()
