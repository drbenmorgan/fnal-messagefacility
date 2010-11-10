#
#  Extensions/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Extensions
#

SET ( srcFiles
    Extensions.cc
    )

ADD_LIBRARY (Extensions SHARED ${srcFiles})
ADD_LIBRARY (ExtensionsS STATIC ${srcFiles})

SET_TARGET_PROPERTIES (Extensions
    PROPERTIES OUTPUT_NAME MF_ExtModules
    )
SET_TARGET_PROPERTIES (ExtensionsS
    PROPERTIES OUTPUT_NAME MF_ExtModules
    )
SET_TARGET_PROPERTIES(Extensions  PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(ExtensionsS PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# install files
FILE (GLOB includeFiles *.h )

INSTALL (FILES ${includeFiles}
    DESTINATION ${product}/${version}/include/messagefacility/Extensions
    )

INSTALL (FILES CMakeLists.txt
               ${srcFiles}
	       ${includeFiles}
    DESTINATION ${product}/${version}/source/messagefacility/Extensions
    )

INSTALL (TARGETS Extensions
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 
