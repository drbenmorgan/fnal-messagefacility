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

SET (DDS_UTILITY_FILES
    ${MessageFacility_SOURCE_DIR}/Extensions/src/CheckStatus.cpp
    )    

SET (DDS_IDL_FILES
    MessageFacility.idl
    )
