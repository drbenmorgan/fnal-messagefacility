# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test(SingleThreadMSPresence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp::fhiclcpp)

cet_test(MultiThreadMSPresence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp::fhiclcpp)
