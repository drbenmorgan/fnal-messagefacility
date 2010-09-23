#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesD
#

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
    UtilitiesD
    boost_thread
    boost_filesystem
    fhiclcpp
    dl
    )

INSTALL (FILES
    mfexample.cc
    MessageFacility.cfg
    DESTINATION ${version}/example
    )

INSTALL (FILES
    CMakeLists.txt
    MFTest.cc
    DESTINATION ${version}/src/test
    )
