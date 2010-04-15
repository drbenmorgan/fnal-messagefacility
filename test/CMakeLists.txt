#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesD
#

INCLUDE_DIRECTORIES (
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

CONFIGURE_FILE (
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageFacilityTest.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/MessageFacility.cfg
    COPYONLY
    )

ADD_EXECUTABLE (MFTest
    MFTest.cc
    )

TARGET_LINK_LIBRARIES (MFTest 
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
    boost_thread
    boost_filesystem
    dcpssacpp
    )

ADD_EXECUTABLE (mfsend
    mfsend.cc
    CheckStatus.cpp
    bld/testDcps.cpp
    bld/testDcps_impl.cpp
    bld/test.cpp
    bld/testSplDcps.cpp
    )

TARGET_LINK_LIBRARIES (mfsend 
    dcpssacpp
    )

ADD_EXECUTABLE (mfrecv
    mfrecv.cc
    CheckStatus.cpp
    bld/testDcps.cpp
    bld/testDcps_impl.cpp
    bld/test.cpp
    bld/testSplDcps.cpp
    )

TARGET_LINK_LIBRARIES (mfrecv
    boost_thread
    dcpssacpp
    )


INSTALL (FILES
    mfexample.cc
    MessageFacility.cfg
    DESTINATION example
    )

INSTALL (FILES
    CMakeLists.txt
    MFTest.cc
    DESTINATION src/test
    )
