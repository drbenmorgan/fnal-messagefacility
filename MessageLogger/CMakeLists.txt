#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerD
#

INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

ADD_LIBRARY (MessageLoggerD SHARED ${srcFiles})
ADD_LIBRARY (MessageLogger ${srcFiles})

SET_TARGET_PROPERTIES (MessageLoggerD
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )

SET_TARGET_PROPERTIES (MessageLogger
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )

# install files
FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/*.h
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/*.icc
    )

LIST (APPEND srcFiles
    src/CMakeLists.txt
    )

FILE (GLOB cmakeFiles
    ${MessageFacility_SOURCE_DIR}/MessageLogger/CMakeLists.txt
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/MessageLogger/interface
    )

INSTALL (FILES
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/MessageLogger.h
    DESTINATION include
    )

INSTALL (FILES
    ${MessageFacility_SOURCE_DIR}/MessageLogger/interface/MessageLogger.h
    DESTINATION include/MessageFacility
    )

INSTALL (FILES ${includeFiles}
    DESTINATION src/MessageLogger/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION src/MessageLogger/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION src/MessageLogger
    )

INSTALL (TARGETS MessageLogger MessageLoggerD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

