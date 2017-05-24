# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test(Presence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp::fhiclcpp)

cet_test(mfexample
  LIBRARIES
  MF_MessageLogger
  fhiclcpp::fhiclcpp)
