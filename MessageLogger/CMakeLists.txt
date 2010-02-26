INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

# add include directories
INCLUDE_DIRECTORIES (${MessageFacility_SOURCE_DIR})

ADD_LIBRARY (MessageLoggerD SHARED ${srcFiles})
ADD_LIBRARY (MessageLogger ${srcFiles})

SET_TARGET_PROPERTIES (MessageLoggerD
    PROPERTIES OUTPUT_NAME MessageLogger
    )

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/*.h
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/*.icc
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/MessageLogger/interface
    )

INSTALL (FILES
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/MessageLogger.h
    DESTINATION include
    )

INSTALL (TARGETS MessageLogger MessageLoggerD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

