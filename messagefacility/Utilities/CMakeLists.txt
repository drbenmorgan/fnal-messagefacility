#
#  Utilities/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library Utilities and UtilitiesS
#

SET (srcFiles
    DebugMacros.cc
    exception.cc
    FormatTime.cc
    SingleConsumerQ.cc
    tinyxml.cc
    tinyxmlerror.cc
    tinyxmlparser.cc
    UnixSignalHandlers.cc
    )

LINK_LIBRARIES ( ${CETLIB}
                 ${Boost_THREAD_LIBRARY}
                 )

ADD_LIBRARY (Utilities SHARED ${srcFiles})
ADD_LIBRARY (UtilitiesS STATIC ${srcFiles})

SET_TARGET_PROPERTIES (Utilities
    PROPERTIES OUTPUT_NAME MF_Utilities
    )
SET_TARGET_PROPERTIES (UtilitiesS
    PROPERTIES OUTPUT_NAME MF_Utilities
    )
SET_TARGET_PROPERTIES(Utilities  PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(UtilitiesS PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# install files
FILE (GLOB includeFiles *.h )

INSTALL (FILES ${includeFiles}
    DESTINATION ${product}/${version}/include/messagefacility/Utilities
    )

INSTALL (FILES CMakeLists.txt
               ${includeFiles}
               ${srcFiles}
    DESTINATION ${product}/${version}/source/messagefacility/Utilities
    )

INSTALL (TARGETS Utilities UtilitiesS
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    )

