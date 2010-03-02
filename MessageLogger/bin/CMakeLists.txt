INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

ADD_EXECUTABLE (msglogger
    msglogger.cc
    )

TARGET_LINK_LIBRARIES (msglogger
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
    boost_thread
    boost_filesystem
    boost_program_options
    pthread
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )

INSTALL (TARGETS msglogger
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
