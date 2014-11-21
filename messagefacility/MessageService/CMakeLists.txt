#
#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
#

# !! MessageService/ Files built in ../MessageLogger/CMakeLists.txt !!

cet_make_exec( ELdestinationTester
               SOURCE ELdestinationTester.cc
               LIBRARIES MF_MessageLogger )

add_subdirectory(plugins)

install_headers()
install_source()
