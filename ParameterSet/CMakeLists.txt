INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

# add include directories
INCLUDE_DIRECTORIES (${MessageFacility_SOURCE_DIR})

ADD_LIBRARY (ParameterSetD SHARED ${srcFiles})  
ADD_LIBRARY (ParameterSet ${srcFiles})  

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/ParameterSet/interface/*.h
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/ParameterSet/interface
    )

INSTALL (TARGETS ParameterSet ParameterSetD 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

