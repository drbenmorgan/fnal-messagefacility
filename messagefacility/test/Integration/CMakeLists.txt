# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test(SingleThreadMSPresence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp)

cet_make_exec(mfexample
  LIBRARIES
  MF_MessageLogger
  MF_MessageService
  fhiclcpp
  )
