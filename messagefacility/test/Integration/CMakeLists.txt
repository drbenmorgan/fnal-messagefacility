# - When user supplies "-DALT_CMAKE" use the non-CET/UPS system
if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()
# ======================================================================
#
# Testing script
#
# ======================================================================

cet_test(Presence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp)

cet_make_exec(mfexample
  LIBRARIES
  MF_MessageLogger
  fhiclcpp
  )

endif() # ALT_CMAKE

