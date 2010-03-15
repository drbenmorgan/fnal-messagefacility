#
#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageService and MessageServiceD
#

INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

ADD_LIBRARY (MessageServiceD SHARED ${srcFiles})
ADD_LIBRARY (MessageService ${srcFiles})

SET_TARGET_PROPERTIES (MessageServiceD
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

SET_TARGET_PROPERTIES (MessageService
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/MessageService/interface/*.h
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/MessageService/interface
    )

INSTALL (TARGETS MessageService MessageServiceD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

