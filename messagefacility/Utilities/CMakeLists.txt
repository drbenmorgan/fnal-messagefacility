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
  UnixSignalHandlers.cc
  LIBRARIES
  cetlib
  -lpthread
  )

# install files
install_headers()
install_source()
