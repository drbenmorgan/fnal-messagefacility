cet_make_library(
  LIBRARY_NAME
  MF_MessageLogger
  SOURCE
  JobReport.cc
  MessageLogger.cc
  MessageLoggerImpl.cc
  MessageSender.cc
  Presence.cc
  ThreadSafeLogMessageLoggerScribe.cc
  LIBRARIES
  MF_MessageService
  ${TBB}
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
