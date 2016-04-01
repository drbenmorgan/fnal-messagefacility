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

cmake_minimum_required(VERSION 3.3)
project(messagefacility VERSION )

# cetbuildtools contains our cmake modules
# - Cetbuildtools, version2
find_package(cetbuildtools2 0.1.0 REQUIRED)
set(CMAKE_MODULE_PATH ${cetbuildtools2_MODULE_PATH})
include(CetInstallDirs)
include(CetCMakeSettings)
include(CetCompilerSettings)

# C++ Standard Config
set(CMAKE_CXX_EXTENSIONS OFF)
set(messagefacility_COMPILE_FEATURES
  cxx_auto_type
  cxx_generic_lambdas
  )

# these are minimum required versions, not the actual product versions
# find_ups_product defines variables for external libraries and executables
find_package(fhiclcpp 3.1.0 REQUIRED)
find_package(cetlib 1.17 REQUIRED)
find_package(SQLite 3.8.5.0 REQUIRED)
find_package(Boost 1.50.0 REQUIRED thread program_options filesystem)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
#add_subdirectory(perllib)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
#add_subdirectory(xcompile)
#add_subdirectory(ups)

# This step enables building the package tarball
# packaging utility
#include(UseCPack)
