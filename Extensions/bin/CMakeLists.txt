
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

ADD_EXECUTABLE (MFDDSserver
    MFDDSserver.cc
    ../src/CheckStatus.cpp
    ../src/DDSdest_bld/MessageFacility.cpp
    ../src/DDSdest_bld/MessageFacilityDcps.cpp
    ../src/DDSdest_bld/MessageFacilityDcps_impl.cpp
    ../src/DDSdest_bld/MessageFacilitySplDcps.cpp
    )

TARGET_LINK_LIBRARIES (MFDDSserver
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )
