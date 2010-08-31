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
       
    ADD_LIBRARY (ExtensionsD SHARED 
        ${SRC_FILES}
        )
  
    ADD_LIBRARY (Extensions   
        ${SRC_FILES}
        )

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

