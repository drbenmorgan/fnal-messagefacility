cet_make_library(
  LIBRARY_NAME MF_MessageService
  SOURCE
  ELadministrator.cc
  ELdestControl.cc
  ELdestination.cc
  ELfwkJobReport.cc
  ELostreamOutput.cc
  ELrecv.cc
  ELstatistics.cc
  ErrorLog.cc
  HardwiredDefaults.cc
  MainThreadMLscribe.cc
  MessageLoggerDefaults.cc
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

install_headers()
install_source()
