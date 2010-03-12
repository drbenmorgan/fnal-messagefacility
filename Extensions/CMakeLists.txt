#
#  Extensions/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Extensions and ExtensionsD
#

INCLUDE (src/CMakeLists.txt)

SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

SET (BIN_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    )

# add idl files in the src/ directory
FOREACH (IDLFILE ${IDLFILES})
    SET (idlFiles ${idlFiles} ${SRC_DIR}/${IDLFILE})
ENDFOREACH (IDLFILE)

# add include directories
INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    ${BIN_DIR}
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

ADD_CUSTOM_TARGET (DDSIDL ALL
    COMMAND $ENV{OSPL_HOME}/bin/idlpp -S -l cpp -d ${BIN_DIR}/DDSdest_bld -I ${SRC_DIR} ${idlFiles}
    )

ADD_LIBRARY (ExtensionsD SHARED 
    ${SRC_FILES}
    ${DDS_FILES}
    )
  
ADD_LIBRARY (Extensions   
    ${SRC_FILES}
    ${DDS_FILES}
    )

SET_TARGET_PROPERTIES (ExtensionsD
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

SET_TARGET_PROPERTIES (Extensions
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

ADD_SUBDIRECTORY (bin)

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/Extensions/interface/*.h
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/Extensions/interface
    )

INSTALL (TARGETS Extensions ExtensionsD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

