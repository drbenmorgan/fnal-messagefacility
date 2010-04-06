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
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )

INSTALL (FILES
    mfexample.cc
    DESTINATION example
    )

INSTALL (FILES
    CMakeLists.txt
    MFTest.cc
    DESTINATION src/test
    )
