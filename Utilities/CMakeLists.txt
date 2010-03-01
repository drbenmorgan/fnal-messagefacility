INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

# add include directories
INCLUDE_DIRECTORIES (${MessageFacility_SOURCE_DIR})

ADD_LIBRARY (UtilitiesD SHARED ${srcFiles})
ADD_LIBRARY (Utilities ${srcFiles})

SET_TARGET_PROPERTIES (UtilitiesD
    PROPERTIES OUTPUT_NAME MF_Utilities
    )

SET_TARGET_PROPERTIES (Utilities
    PROPERTIES OUTPUT_NAME MF_Utilities
    )

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/Utilities/interface/*.h
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/Utilities/interface
    )

INSTALL (TARGETS Utilities UtilitiesD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

