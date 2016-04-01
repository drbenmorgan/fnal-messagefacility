#
#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
#

# !! MessageService/ Files built in ../MessageLogger/CMakeLists.txt !!

add_executable(ELdestinationTester ELdestinationTester.h ELdestinationTester.cc)
target_link_libraries(ELdestinationTester
  MF_MessageLogger
  Boost::filesystem
  Boost::program_options
  )


#cet_make_exec( ELdestinationTester
#  SOURCE ELdestinationTester.cc
#  LIBRARIES
#  MF_MessageLogger
#  ${Boost_FILESYSTEM_LIBRARY}
#  ${Boost_PROGRAM_OPTIONS_LIBRARY}
#  )

#add_subdirectory(plugins)

#install_headers()
#install_source()
