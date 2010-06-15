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

FOREACH (FILE ${SRCFILES})
    LIST (APPEND SRC_FILES ${SRC_DIR}/${FILE})
ENDFOREACH (FILE)

FOREACH (FILE ${QTSRCFILES})
    LIST (APPEND QT_SRC_FILES ${SRC_DIR}/${FILE})
ENDFOREACH (FILE)

FOREACH (FILE ${DDSUTILITYFILES})
    LIST (APPEND DDS_UTILITY_FILES ${SRC_DIR}/${FILE})
ENDFOREACH (FILE)

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
FOREACH (IDLFILE ${DDSIDLFILES})
    GET_FILENAME_COMPONENT (idlName ${IDLFILE} NAME_WE)
    LIST (APPEND DDS_GENERATED_FILES
        ${BIN_DIR}/DDSdest_bld/${idlName}.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}Dcps.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}Dcps_impl.cpp
        ${BIN_DIR}/DDSdest_bld/${idlName}SplDcps.cpp)
    LIST (APPEND DDS_IDL_FILES ${SRC_DIR}/${IDLFILE})
ENDFOREACH (IDLFILE)

# Generate DDS IDL pre-compiling output files
ADD_CUSTOM_COMMAND (OUTPUT ${DDS_GENERATED_FILES}
    COMMAND $ENV{OSPL_HOME}/bin/idlpp
    ARGS -S -l cpp -d ${BIN_DIR}/DDSdest_bld -I ${SRC_DIR} ${DDS_IDL_FILES}
    MAIN_DEPENDENCY ${idlFiles}
    )

ADD_CUSTOM_TARGET (generateIDL
    DEPENDS ${DDS_GENERATED_FILES}
    )

IF (NOT CMAKE_CROSSCOMPILING)

    FIND_PACKAGE (Qt4 4.5.2 REQUIRED)
    INCLUDE (${QT_USE_FILE})
    
    IF (QT_WRAP_CPP)

        QT_WRAP_CPP (qtreceiver
            qtreceiver_moc_files
            interface/QtDDSReceiver.h
            )

    ENDIF (QT_WRAP_CPP)
        
    ADD_LIBRARY (ExtensionsD SHARED 
        ${SRC_FILES}
        ${QT_SRC_FILES}
        ${qtreceiver_moc_files}
        ${DDS_UTILITY_FILES}
        ${DDS_GENERATED_FILES}
        )

    TARGET_LINK_LIBRARIES(ExtensionsD
        ${QT_LIBRARIES}
        )    
  
    ADD_LIBRARY (Extensions   
        ${SRC_FILES}
        ${QT_SRC_FILES}
        ${qtreceiver_moc_files}
        ${DDS_UTILITY_FILES}
        ${DDS_GENERATED_FILES}
        )

ELSE (NOT CMAKE_CROSSCOMPILING)

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

ENDIF (NOT CMAKE_CROSSCOMPILING)


SET_TARGET_PROPERTIES (ExtensionsD
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

SET_TARGET_PROPERTIES (Extensions
    PROPERTIES OUTPUT_NAME MF_Extensions
    )

# install files
FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/Extensions/interface/*.h
    )

SET (srcFiles
    src/CMakeLists.txt
    ${SRC_FILES}
    ${DDS_UTILITY_FILES}
    ${DDS_IDL_FILES}
    )

FILE (GLOB cmakeFiles
    ${MessageFacility_SOURCE_DIR}/Extensions/CMakeLists.txt
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/Extensions/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION src/Extensions/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION src/Extensions/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION src/Extensions
    )

INSTALL (TARGETS Extensions ExtensionsD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

ADD_SUBDIRECTORY (bin)
