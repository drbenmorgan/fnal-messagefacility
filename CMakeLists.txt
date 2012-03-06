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
#        -Dqualifier:STRING=$CETPKG_QUAL
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package

cmake_minimum_required (VERSION 2.8) # use cmake 2.8 or later

project(messagefacility)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()
set(CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)

if ( NOT qualifier )
    message (FATAL_ERROR "ERROR: please call cmake with -Dqualifier:STRING=qualifier")
endif ( NOT qualifier )
cet_cmake_env( messagefacility v1_10_08 ${qualifier} )

cet_set_compiler_flags(DIAGS VIGILANT WERROR EXTRA_FLAGS -pedantic)
if ("${qualifier}" STREQUAL "e1")
  cet_add_compiler_flags(CXX -std=c++0x;-Wno-deprecated-declarations)
endif()
cet_report_compiler_flags()

cet_check_gcc()

# these are minimum required versions, not the actual product versions
find_ups_product( cetbuildtools v1_15_00 )
find_ups_product( cpp0x v1_03_02 )
find_ups_product( cetlib v1_03_02 )
find_ups_product( fhiclcpp v2_16_02  )
find_ups_boost( v1_48_0 filesystem
                        thread
                        program_options )

# Use cmake directive include_directories to define include search path

# We want to be able to #include "MyProduct/MyHeader.h" in source code,
# generated code, and user code.
# We achieve this by putting all headers into a MyProduct subdirectory
# instead of an include subdirectory.
# PROJECT_SOURCE_DIR is a cmake variable which points to the top of the
# source code tree.
include_directories ("${PROJECT_SOURCE_DIR}")

# Generated code will be created in the build directory,
# so we also add PROJECT_BINARY_DIR to the include search path.
# This step may be omitted if there are no generated headers.
include_directories ("${PROJECT_BINARY_DIR}")

# let cmake define variables for external libraries and executables

# define CETLIB
find_library( CETLIB NAMES cetlib PATHS $ENV{CETLIB_LIB} )

# define FHICLCPP
find_library( FHICLCPP NAMES fhiclcpp PATHS $ENV{FHICLCPP_LIB} )

# Building  all libraries and binaries in a single lib and bin directory
# is useful for a complex product like art.
# You may not want to do this for smaller products.

#build all libraries in a single directory
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#build all executables in a single directory
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)
add_subdirectory(test)
add_subdirectory(xcompile)
add_subdirectory(ups)

# This step enables building the package tarball
# packaging utility
include(UseCPack)
