#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerS
#

FILE (GLOB MLsrcFiles *.cc)
FILE (GLOB MSsrcFiles ../MessageService/*.cc)

cet_make_library( LIBRARY_NAME MF_MessageLogger
        	  SOURCE  ${MLsrcFiles} ${MSsrcFiles}
        	  LIBRARIES 
                  MF_Utilities 
                  ${FHICLCPP} 
                  ${Boost_THREAD_LIBRARY} 
        	  WITH_STATIC_LIBRARY )

# install files
install_headers( EXTRAS classes.h )
install_source()
