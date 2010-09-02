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

TARGET_LINK_LIBRARIES ( MessageServiceD
    fhicl-cpp
    )

TARGET_LINK_LIBRARIES ( MessageService
    fhicl-cpp
    )

SET_TARGET_PROPERTIES ( MessageServiceD
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

SET_TARGET_PROPERTIES ( MessageService
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

# install files
FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/MessageService/interface/*.h
    ${MessageFacility_SOURCE_DIR}/MessageService/interface/*.icc
    )

LIST (APPEND srcFiles
    src/CMakeLists.txt
    )

FILE (GLOB cmakeFiles
    ${MessageFacility_SOURCE_DIR}/MessageService/CMakeLists.txt
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/MessageService/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION src/MessageService/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION src/MessageService/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION src/MessageService
    )

INSTALL (TARGETS MessageService MessageServiceD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

