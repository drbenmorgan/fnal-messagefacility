#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesD
#

INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

ADD_LIBRARY (UtilitiesD SHARED ${srcFiles})
ADD_LIBRARY (Utilities ${srcFiles})

SET_TARGET_PROPERTIES (UtilitiesD
    PROPERTIES OUTPUT_NAME MF_Utilities
    )

SET_TARGET_PROPERTIES (Utilities
    PROPERTIES OUTPUT_NAME MF_Utilities
    )

# install files
FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/Utilities/interface/*.h
    )

LIST (APPEND srcFiles
    src/CMakeLists.txt
    )

FILE (GLOB cmakeFiles
    ${MessageFacility_SOURCE_DIR}/Utilities/CMakeLists.txt
    )

INSTALL (FILES ${includeFiles}
    DESTINATION ${version}/include/Utilities/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION ${version}/src/Utilities/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION ${version}/src/Utilities/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION ${version}/src/Utilities
    )

INSTALL (TARGETS Utilities UtilitiesD
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 

