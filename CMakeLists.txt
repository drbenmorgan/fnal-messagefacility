#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#
# Create a build directory and an install direcotory which are outside the source code directory
#  cd .../path/to/build/directory
#  source .../path/to/source/code/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)

cmake_minimum_required (VERSION 2.8) # use cmake 2.8 or later

project(messagefacility)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()
set(CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS VIGILANT
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  EXTRA_CXX_FLAGS -std=c++11
  )
cet_report_compiler_flags()

cet_check_gcc()

# these are minimum required versions, not the actual product versions
# find_ups_product defines variables for external libraries and executables
find_ups_product( cetbuildtools v3_02_00 )
find_ups_product( fhiclcpp v2_17_00  )

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
add_subdirectory(test)
add_subdirectory(xcompile)
add_subdirectory(ups)

# This step enables building the package tarball
# packaging utility
include(UseCPack)
