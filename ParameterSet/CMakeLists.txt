#
#  ParameterSet/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library ParameterSet and ParameterSetD
#

INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

ADD_LIBRARY (ParameterSetD SHARED ${srcFiles})  
ADD_LIBRARY (ParameterSet ${srcFiles})  

SET_TARGET_PROPERTIES (ParameterSetD
    PROPERTIES OUTPUT_NAME MF_ParameterSet
    )

SET_TARGET_PROPERTIES (ParameterSet
    PROPERTIES OUTPUT_NAME MF_ParameterSet
    )

#install files
FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/ParameterSet/interface/*.h
    )

LIST (APPEND srcFiles
    src/CMakeLists.txt
    )

FILE (GLOB cmakeFiles
    ${MessageFacility_SOURCE_DIR}/ParameterSet/CMakeLists.txt
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/ParameterSet/interface
    )

INSTALL (FILES ${includeFiles}
    DESTINATION src/ParameterSet/interface
    )

INSTALL (FILES ${srcFiles}
    DESTINATION src/ParameterSet/src
    )

INSTALL (FILES ${cmakeFiles}
    DESTINATION src/ParameterSet
    )

INSTALL (TARGETS ParameterSet ParameterSetD 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

