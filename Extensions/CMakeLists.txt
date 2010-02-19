INCLUDE (src/CMakeLists.txt)

SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

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

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

#ADD_CUSTOM_TARGET (ELDDSdestIDL ALL
#    COMMAND $ENV{OSPL_HOME}/bin/idlpp -S -l cpp -d ${SRC_DIR}/DDSdest_bld -I ${SRC_DIR} ${idlFiles}
#    )

ADD_LIBRARY (ExtensionsD SHARED ${FILES})  
ADD_LIBRARY (Extensions ${FILES})  

ADD_SUBDIRECTORY (bin)

FILE (GLOB includeFiles
    ${MessageFacility_SOURCE_DIR}/Extensions/interface/*.h
    )

INSTALL (FILES ${includeFiles}
    DESTINATION include/Extensions/interface
    )

INSTALL (TARGETS Extensions ExtensionsD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

