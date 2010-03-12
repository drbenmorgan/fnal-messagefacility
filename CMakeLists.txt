#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#


CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (MessageFacility)

SET (CMAKE_INSTALL_PREFIX ${MessageFacility_SOURCE_DIR}/build)

#ADD_DEFINITIONS (-g) 

INCLUDE_DIRECTORIES($ENV{BOOST_DIR}/include)
LINK_DIRECTORIES($ENV{BOOST_DIR}/lib)

ADD_SUBDIRECTORY (MessageLogger)
ADD_SUBDIRECTORY (MessageService)
ADD_SUBDIRECTORY (ParameterSet)
ADD_SUBDIRECTORY (Utilities)
ADD_SUBDIRECTORY (Extensions)

ADD_SUBDIRECTORY (test)
