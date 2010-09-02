#
#  CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for project MessageFacility
#


CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (MessageFacility)

#ADD_DEFINITIONS (-g) 

# define the boost envrionment so we don't get system libraries
set ( BOOST_ROOT       $ENV{BOOST_DIR} )
set ( BOOST_INCLUDEDIR $ENV{BOOST_INC} )
set ( BOOST_LIBRARYDIR $ENV{BOOST_LIB} )

find_package (Boost 1.42 REQUIRED 
                  COMPONENTS
                  thread
                  program_options
             )

# Project include directory
INCLUDE_DIRECTORIES(
    ${MessageFacility_SOURCE_DIR}
    ${MessageFacility_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    $ENV{FHICLCPP_DIR}/include
    )

# Project library directory
LINK_DIRECTORIES(
    ${Boost_LIBRARY_DIRS}
    $ENV{FHICLCPP_DIR}/lib
    )

# Install directory
SET (FLAVOR ${CMAKE_HOST_SYSTEM_NAME}.${CMAKE_HOST_SYSTEM_PROCESSOR})

IF (CMAKE_CROSSCOMPILING)
    SET (FLAVOR ${CMAKE_SYSTEM_NAME}.${CMAKE_SYSTEM_PROCESSOR})
ENDIF (CMAKE_CROSSCOMPILING)

SET (CMAKE_INSTALL_PREFIX ${MessageFacility_SOURCE_DIR}/install/${FLAVOR})

INSTALL (FILES
    CMakeLists.txt
    CMakeLists.general
    CMakeLists.nova
    DESTINATION src
    )

ADD_SUBDIRECTORY (MessageLogger)
ADD_SUBDIRECTORY (MessageService)
ADD_SUBDIRECTORY (Utilities)
ADD_SUBDIRECTORY (Extensions)

ADD_SUBDIRECTORY (test)
ADD_SUBDIRECTORY (xcompile)
