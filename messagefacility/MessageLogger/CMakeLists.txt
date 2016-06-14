cet_make_library(
  LIBRARY_NAME MF_MessageLogger
  SOURCE
  ExceptionMessages.cc
  JobReport.cc
  LoggedErrorsSummary.cc
  MessageLogger.cc
  MessageLoggerImpl.cc
  MessageLoggerScribe.cc
  MessageSender.cc
  MessageServicePresence.cc
  Presence.cc
  SilentMLscribe.cc
  SingleThreadMSPresence.cc
  LIBRARIES
  MF_MessageService
  fhiclcpp
  ${Boost_THREAD_LIBRARY}
  )

cet_make_exec( ELdestinationTester
  SOURCE ELdestinationTester.cc
  LIBRARIES
  MF_MessageLogger
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

install_headers()
install_source()
