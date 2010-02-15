INCLUDE (src/CMakeLists.txt)

SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

# add all source files in the src/ directory
FOREACH (FILE ${FILES})
    SET (srcFiles ${srcFiles} src/${FILE})
ENDFOREACH (FILE)

# add idl files in the src/ directory
FOREACH (IDLFILE ${IDLFILES})
    SET (idlFiles ${idlFiles} ${SRC_DIR}/${IDLFILE})
ENDFOREACH (IDLFILE)

# add include directories
INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

MESSAGE (${CMAKE_CURRENT_SOURCE_DIR})

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

ADD_CUSTOM_TARGET (ELDDSdestIDL ALL
    COMMAND $ENV{OSPL_HOME}/bin/idlpp -S -l cpp -d ${SRC_DIR}/DDSdest_bld -I ${SRC_DIR} ${idlFiles}
    )

ADD_LIBRARY (ExtensionsD SHARED ${srcFiles})  
ADD_LIBRARY (Extensions ${srcFiles})  

ADD_SUBDIRECTORY (bin)
