#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#

cet_make( LIBRARY_NAME MF_Utilities
          LIBRARIES ${CETLIB}
          ${Boost_FILESYSTEM_LIBRARY}
          ${Boost_REGEX_LIBRARY}
          ${Boost_SYSTEM_LIBRARY}
          ${Boost_THREAD_LIBRARY}
          -lpthread
          WITH_STATIC_LIBRARY )

# install files
install_headers()
install_source()
