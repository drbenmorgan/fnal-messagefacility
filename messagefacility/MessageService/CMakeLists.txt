#
#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
#

# !! MessageService/ Files built in ../MessageLogger/CMakeLists.txt !!

cet_make_exec( ELdestinationTester
  SOURCE ELdestinationTester.cc
  LIBRARIES
  MF_MessageLogger
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  )

add_subdirectory(plugins)

install_headers()
install_source()
