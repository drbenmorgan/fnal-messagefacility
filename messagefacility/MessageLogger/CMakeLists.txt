#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerS
#


FILE (GLOB MLsrcFiles *.cc)
FILE (GLOB MSsrcFiles ../MessageService/*.cc)

LIST ( REMOVE_ITEM MSsrcFiles ../MessageService/ELdestinationTester.cc )

cet_make_library( LIBRARY_NAME MF_MessageLogger
                  SOURCE  ${MLsrcFiles} ${MSsrcFiles}
        	  LIBRARIES 
                  MF_Utilities 
                  fhiclcpp
                  ${Boost_THREAD_LIBRARY} )

# install files
install_headers()
install_source()
