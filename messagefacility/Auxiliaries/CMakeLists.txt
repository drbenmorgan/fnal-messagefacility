cet_make_library(
  LIBRARY_NAME MF_Auxiliaries
  SOURCE
  ELextendedID.cc
  ELlimitsTable.cc
  ELmap.cc
  ELseverityLevel.cc
  ErrorObj.cc
  MessageFacilityMsg.cc
  LIBRARIES
  MF_Utilities
  fhiclcpp )

install_headers()
install_source()
