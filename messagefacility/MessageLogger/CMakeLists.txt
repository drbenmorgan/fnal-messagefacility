#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerS
#

FILE (GLOB srcFiles *.cc )

ADD_LIBRARY (MessageLogger SHARED ${srcFiles})
ADD_LIBRARY (MessageLoggerS STATIC ${srcFiles})

SET_TARGET_PROPERTIES (MessageLogger
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )
SET_TARGET_PROPERTIES (MessageLoggerS
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )
SET_TARGET_PROPERTIES(MessageLogger  PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(MessageLoggerS PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# install files
FILE (GLOB includeFiles *.h *.icc )

INSTALL (FILES ${includeFiles}
    DESTINATION ${product}/${version}/include/messagefacility/MessageLogger/
    )

INSTALL (FILES CMakeLists.txt
               ${includeFiles}
	       ${srcFiles}
    DESTINATION ${product}/${version}/source/messagefacility/MessageLogger/
    )

INSTALL (TARGETS MessageLogger MessageLoggerS
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 
