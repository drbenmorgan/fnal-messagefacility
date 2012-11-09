#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#

SET (srcFiles
    DebugMacros.cc
    exception.cc
    FormatTime.cc
    SingleConsumerQ.cc
    tinyxml.cc
    tinyxmlerror.cc
    tinyxmlparser.cc
    UnixSignalHandlers.cc
    )

FILE (GLOB includeFiles *.h )

cet_make_library( LIBRARY_NAME MF_Utilities
        	  SOURCE ${srcFiles}
        	  LIBRARIES ${CETLIB} ${Boost_THREAD_LIBRARY} -lpthread
        	  WITH_STATIC_LIBRARY )

# install files
install_headers()
install_source( EXTRAS CMakeLists.txt )
