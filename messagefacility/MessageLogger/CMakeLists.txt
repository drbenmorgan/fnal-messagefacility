#
#  MessageLogger/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageLogger and MessageLoggerS
#

FILE (GLOB srcFiles *.cc )

SET (MSsrcFiles
       ../MessageService/ELadministrator.cc
       ../MessageService/ELcollected.cc
       ../MessageService/ELdestControl.cc
       ../MessageService/ELdestination.cc
       ../MessageService/ELerrorList.cc
       ../MessageService/ELfwkJobReport.cc
       ../MessageService/ELlimitsTable.cc
       ../MessageService/ELoutput.cc
       ../MessageService/ELrecv.cc
       ../MessageService/ELstatistics.cc
       ../MessageService/ELtsErrorLog.cc
       ../MessageService/ErrorLog.cc
       ../MessageService/HardwiredDefaults.cc
       ../MessageService/MainThreadMLscribe.cc
       ../MessageService/MessageLogger.cc
       ../MessageService/MessageLoggerDefaults.cc
       ../MessageService/MessageLoggerScribe.cc
       ../MessageService/MessageServicePresence.cc
       ../MessageService/MsgContext.cc
       ../MessageService/Presence.cc
       ../MessageService/SingleThreadMSPresence.cc
       ../MessageService/ThreadQueue.cc
    )

LINK_LIBRARIES (
 Utilities
 ${FHICLCPP}
 ${Boost_THREAD_LIBRARY}
 dl
 )

ADD_LIBRARY (MessageLogger SHARED ${srcFiles} ${MSsrcFiles} )
ADD_LIBRARY (MessageLoggerS STATIC ${srcFiles} ${MSsrcFiles} )

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

FILE (GLOB MSincludeFiles ../MessageService/*.h ../MessageService/*.icc )

INSTALL (FILES ${MSincludeFiles}
    DESTINATION ${product}/${version}/include/messagefacility/MessageService
    )

INSTALL (FILES ../MessageService/CMakeLists.txt
               ${MSincludeFiles}
                     ${MSsrcFiles}
    DESTINATION ${product}/${version}/source/messagefacility/MessageService
    )



INSTALL (TARGETS MessageLogger MessageLoggerS
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    )
