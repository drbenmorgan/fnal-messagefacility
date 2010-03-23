#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesD
#

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
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
    pthread
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )

INSTALL (FILES
    ${MessageFacility_SOURCE_DIR}/test/mfexample.cc
    DESTINATION example
    )
