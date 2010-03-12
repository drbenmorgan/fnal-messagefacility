#
#  Extensions/src/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file gathering source files for library Extensions and
#  ExtensionsD 
#

FILE (GLOB SRC_FILES 
    ${MessageFacility_SOURCE_DIR}/Extensions/src/*.cc
    )

FILE (GLOB DDS_FILES 
    ${MessageFacility_SOURCE_DIR}/Extensions/src/*.cpp
    ${MessageFacility_BINARY_DIR}/Extensions/DDSdest_bld/*.cpp
    )

SET (IDLFILES
    MessageFacility.idl
    )
