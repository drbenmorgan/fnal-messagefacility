#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerD
#

FILE (GLOB srcFiles *.cc )

ADD_LIBRARY (MessageLoggerD SHARED ${srcFiles})
ADD_LIBRARY (MessageLogger ${srcFiles})

SET_TARGET_PROPERTIES (MessageLoggerD
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )

SET_TARGET_PROPERTIES (MessageLogger
    PROPERTIES OUTPUT_NAME MF_MessageLogger
    )

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

INSTALL (TARGETS MessageLogger MessageLoggerD
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 

