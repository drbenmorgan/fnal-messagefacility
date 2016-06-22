cet_make_library(
  LIBRARY_NAME
  MF_MessageLogger
  SOURCE
  JobReport.cc
  MessageDrop.cc
  MessageLogger.cc
  MessageLoggerImpl.cc
  MessageLoggerScribe.cc
  MessageSender.cc
  MessageServicePresence.cc
  Presence.cc
  SingleThreadMSPresence.cc
  createPresence.cc
  LIBRARIES
  MF_MessageService
  MF_Utilities
  fhiclcpp
  ${Boost_THREAD_LIBRARY}
  )

cet_make_exec(ELdestinationTester
  SOURCE ELdestinationTester.cc
  LIBRARIES
  MF_MessageLogger
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

install_headers()
install_source()
