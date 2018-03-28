include(CetMake)

#MessageService/ELadministrator.cc
#MessageLogger/Presence.cc
#MessageLogger/MessageLoggerImpl.cc
#MessageLogger/MessageLoggerScribe.cc
#MessageLogger/MessageSender.cc
#MessageService/MessageLoggerQ.cc
#MessageService/MsgFormatSettings.cc
#MessageService/MsgStatistics.cc
#MessageLogger/MFGlobals.cc
#MessageLogger/MFPerThread.cc

cet_make_library(
  LIBRARY_NAME
    MF_MessageLogger
  SOURCE
    MessageLogger/MessageLogger.cc
    MessageLogger/MFConfig.cc
    MessageService/ELdestination.cc
    MessageService/ELostreamOutput.cc
    MessageService/ELstatistics.cc
  LIBRARIES
    MF_Utilities
    cetlib_except
    fhiclcpp
    hep_concurrency
    ${TBB}
)

add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
add_subdirectory(plugins)
# Testing
add_subdirectory(test)
