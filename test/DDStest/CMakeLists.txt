
INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}/test
    ${CMAKE_CURRENT_BINARY_DIR}
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

SET (IDLFILE
    ${SRC_DIR}/Chat.idl
    )

ADD_CUSTOM_TARGET (ChatIDL ALL
    COMMAND $ENV{OSPL_HOME}/bin/idlpp -S -l cpp -I ${SRC_DIR} ${IDLFILE}
    )

ADD_EXECUTABLE (DDStest
    CreateTopics.cc
    )

TARGET_LINK_LIBRARIES (DDStest
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )
