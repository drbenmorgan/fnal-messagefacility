#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#
set(messagefacility_utilities_SOURCES
  DebugMacros.cc
  DebugMacros.h
  JobMode.h
  SingleConsumerQ.cc
  SingleConsumerQ.h
  UnixSignalHandlers.cc
  UnixSignalHandlers.h
  do_nothing_deleter.h
  exception.cc
  exception.h
  formatTime.cc
  formatTime.h
  tinystr.h
  tinyxml.cc
  tinyxml.h
  tinyxmlerror.cc
  tinyxmlparser.cc
  )


add_library(MF_Utilities SHARED
  ${messagefacility_utilities_SOURCES}
  )
target_compile_features(MF_Utilities PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_Utilities
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_Utilities cetlib::cetlib)






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
