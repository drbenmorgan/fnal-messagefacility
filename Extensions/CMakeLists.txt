#
#  Extensions/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Extensions
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
       
ADD_LIBRARY (Extensions MODULE
    ${SRC_FILES}
    )

SET_TARGET_PROPERTIES (Extensions 
    PROPERTIES OUTPUT_NAME MF_ExtModules
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
    DESTINATION ${version}/include/Extensions/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION ${version}/src/Extensions/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION ${version}/src/Extensions/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION ${version}/src/Extensions
    )

INSTALL (TARGETS Extensions
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 

