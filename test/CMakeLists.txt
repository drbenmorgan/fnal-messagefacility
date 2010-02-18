INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

FIND_PACKAGE (Boost COMPONENTS thread filesystem REQUIRED)

ADD_EXECUTABLE (MFTest
    MFTest.cc
    )

ADD_DEFINITIONS ("-g") 

TARGET_LINK_LIBRARIES (MFTest 
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
#   ${Boost_LIBRARIES}
    boost_thread
    boost_filesystem
    pthread
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
)

ADD_SUBDIRECTORY (DDStest)
