INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    )

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

