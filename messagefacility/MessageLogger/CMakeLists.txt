# - When user supplies "-DALT_CMAKE" use the non-CET/UPS system
if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()

cet_make_exec(ELdestinationTester
  SOURCE ELdestinationTester.cc
  LIBRARIES
  MF_MessageLogger
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

install_headers()
install_source()

endif() # ALT_CMAKE
