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
  MessageLoggerQ.cc
  MsgContext.cc
  MsgFormatSettings.cc
  MsgStatistics.cc
  ThreadQueue.cc
  LIBRARIES
  MF_Utilities
  fhiclcpp
  cetlib_except
  ${Boost_THREAD_LIBRARY}
  )

add_subdirectory(plugins)

install_headers()
install_source()
