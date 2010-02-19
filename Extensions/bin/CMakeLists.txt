
SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

# add include directories
INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

FILE (GLOB serverSrcFiles
    MFDDSserver.cc
    ${MessageFacility_SOURCE_DIR}/Extensions/src/CheckStatus.cpp
    ${MessageFacility_SOURCE_DIR}/Extensions/src/DDSdest_bld/*.cpp
    )

ADD_EXECUTABLE (MFDDSserver
    ${serverSrcFiles}
    )

TARGET_LINK_LIBRARIES (MFDDSserver
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )

INSTALL (TARGETS MFDDSserver
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 
