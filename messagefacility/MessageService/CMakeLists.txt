# - When user supplies "-DALT_CMAKE" use the non-CET/UPS system
if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()

cet_make_library(
  LIBRARY_NAME MF_MessageService
  SOURCE
  ELadministrator.cc
  ELdestination.cc
  ELfwkJobReport.cc
  ELostreamOutput.cc
  ELrecv.cc
  ELstatistics.cc
  ErrorLog.cc
  MainThreadMLscribe.cc
  MessageLoggerQ.cc
  MsgContext.cc
  MsgFormatSettings.cc
  MsgStatistics.cc
  ThreadQueue.cc
  LIBRARIES
  MF_Utilities
  fhiclcpp
  ${Boost_THREAD_LIBRARY}
  )

add_subdirectory(plugins)

install_headers()
install_source()

endif() # ALT_CMAKE
