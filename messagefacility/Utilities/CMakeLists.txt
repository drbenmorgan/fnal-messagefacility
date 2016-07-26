# - When user supplies "-DALT_CMAKE" use the non-CET/UPS system
if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()

cet_make_library(
  LIBRARY_NAME
  MF_Utilities
  SOURCE
  DebugMacros.cc
  ELextendedID.cc
  ELlimitsTable.cc
  ELmap.cc
  ELseverityLevel.cc
  ErrorObj.cc
  eq_nocase.cc
  exception.cc
  formatTime.cc
  MessageFacilityMsg.cc
  SingleConsumerQ.cc
  tinyxml.cc
  tinyxmlerror.cc
  tinyxmlparser.cc
  UnixSignalHandlers.cc
  LIBRARIES
  cetlib
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  -lpthread
  )

# install files
install_headers()
install_source()

endif() # ALT_CMAKE
