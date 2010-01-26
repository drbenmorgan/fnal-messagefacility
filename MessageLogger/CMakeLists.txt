INCLUDE (src/CMakeLists.txt)

# add all source files in the src/ diretory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

# add include directories
INCLUDE_DIRECTORIES (${MessageFacility_SOURCE_DIR})

ADD_LIBRARY (MessageLogger SHARED ${srcFiles})

