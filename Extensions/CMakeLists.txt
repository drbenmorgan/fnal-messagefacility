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

SET (DDS_INCLUDE_DIR
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

SET (DDS_LIBRARY_DIR
    $ENV{OSPL_HOME}/lib
    )

# Include directories
INCLUDE_DIRECTORIES (
    ${DDS_INCLUDE_DIR}
    )

# Library directories
LINK_DIRECTORIES (
    ${DDS_LIBRARY_DIR}
    )

# Add DDS IDL files in the src/ directory
FOREACH (IDLFILE ${DDS_IDL_FILES})
    GET_FILENAME_COMPONENT (idlName ${IDLFILE} NAME_WE)
    LIST (APPEND DDS_GENERATED_FILES
        ${BIN_DIR}/DDSdest_bld/${idlName}.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}Dcps.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}Dcps_impl.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}SplDcps.cpp)
    LIST (APPEND idlFiles ${SRC_DIR}/${IDLFILE})
ENDFOREACH (IDLFILE)

# Generate DDS IDL pre-compiling output files
ADD_CUSTOM_COMMAND (OUTPUT ${DDS_GENERATED_FILES}
    COMMAND $ENV{OSPL_HOME}/bin/idlpp 
    ARGS -S -l cpp -d ${BIN_DIR}/DDSdest_bld -I ${SRC_DIR} ${idlFiles}
    MAIN_DEPENDENCY ${idlFiles}
    )

ADD_CUSTOM_TARGET (generatedIDL
    DEPENDS ${DDS_GENERATED_FILES}
    )

ADD_LIBRARY (ExtensionsD SHARED 
    ${SRC_FILES}
    ${DDS_UTILITY_FILES}
    ${DDS_GENERATED_FILES}
    )
  
ADD_LIBRARY (Extensions   
    ${SRC_FILES}
    ${DDS_UTILITY_FILES}
    ${DDS_GENERATED_FILES}
    )

SET_TARGET_PROPERTIES (ExtensionsD
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

SET_TARGET_PROPERTIES (Extensions
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

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

ADD_SUBDIRECTORY (bin)
