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
    fhiclcpp
    dl
    )

TARGET_LINK_LIBRARIES ( MessageService
    fhiclcpp
    dl
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
    DESTINATION ${product}/${version}/include/MessageService/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION ${product}/${version}/source/MessageService/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION ${product}/${version}/source/MessageService/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION ${product}/${version}/source/MessageService
    )

INSTALL (TARGETS MessageService MessageServiceD
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 

