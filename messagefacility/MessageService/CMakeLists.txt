cet_make_library(
  LIBRARY_NAME MF_MessageService
  SOURCE
  ELadministrator.cc
  ELcollected.cc
  ELdestControl.cc
  ELdestination.cc
  ELerrorList.cc
  ELfwkJobReport.cc
  ELostreamOutput.cc
  ELrecv.cc
  ELstatistics.cc
  ELtsErrorLog.cc
  ErrorLog.cc
  HardwiredDefaults.cc
  MainThreadMLscribe.cc
  MessageDrop.cc
  MessageLoggerDefaults.cc
  MessageLoggerQ.cc
  MsgContext.cc
  MsgFormatSettings.cc
  MsgStatistics.cc
  ThreadQueue.cc
  LIBRARIES
  MF_Auxiliaries
  MF_Utilities
  fhiclcpp
  ${Boost_THREAD_LIBRARY}
  )

add_subdirectory(plugins)

install_headers()
install_source()
