#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#


CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (MessageFacility)

# Project include directory
INCLUDE_DIRECTORIES(
    ${MessageFacility_SOURCE_DIR}
    ${MessageFacility_BINARY_DIR}
    $ENV{BOOST_DIR}/include
    )

# Project library directory
LINK_DIRECTORIES(
    $ENV{BOOST_DIR}/lib
    )

# Install directory
SET (CMAKE_INSTALL_PREFIX ${MessageFacility_SOURCE_DIR}/build)

#ADD_DEFINITIONS (-g) 

ADD_SUBDIRECTORY (MessageLogger)
ADD_SUBDIRECTORY (MessageService)
ADD_SUBDIRECTORY (ParameterSet)
ADD_SUBDIRECTORY (Utilities)
ADD_SUBDIRECTORY (Extensions)

ADD_SUBDIRECTORY (test)
