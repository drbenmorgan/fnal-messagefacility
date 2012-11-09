#
#  Extensions/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Extensions
#

SET ( srcFiles
    Extensions.cc
    )

cet_make_library( LIBRARY_NAME MF_ExtModules
        	  SOURCE ${srcFiles}
        	  WITH_STATIC_LIBRARY )


# install files
install_headers()
install_source( EXTRAS CMakeLists.txt )
