#
#  MessageService/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for library MessageService and MessageServiceD
#
SET (srcFiles
       ELadministrator.cc
       ELcollected.cc
       ELdestControl.cc
       ELdestination.cc
       ELerrorList.cc
       ELfwkJobReport.cc
       ELlimitsTable.cc
       ELoutput.cc
       ELrecv.cc
       ELstatistics.cc
       ELtsErrorLog.cc
       ErrorLog.cc
       HardwiredDefaults.cc
       MainThreadMLscribe.cc
       MessageLogger.cc
       MessageLoggerDefaults.cc
       MessageLoggerScribe.cc
       MessageServicePresence.cc
       MsgContext.cc
       Presence.cc
       SingleThreadMSPresence.cc
       ThreadQueue.cc
    )

link_libraries( ${FHICLCPP} dl )
ADD_LIBRARY (MessageServiceD SHARED ${srcFiles})
ADD_LIBRARY (MessageService ${srcFiles})

SET_TARGET_PROPERTIES ( MessageServiceD
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

SET_TARGET_PROPERTIES ( MessageService
    PROPERTIES OUTPUT_NAME MF_MessageService
    )

# install files
FILE (GLOB includeFiles *.h *.icc )

INSTALL (FILES ${includeFiles}
    DESTINATION ${product}/${version}/include/messagefacility/MessageService
    )

INSTALL (FILES CMakeLists.txt
               ${includeFiles}
	       ${srcFiles}
    DESTINATION ${product}/${version}/source/messagefacility/MessageService
    )

INSTALL (TARGETS MessageService MessageServiceD
    RUNTIME DESTINATION ${flavorqual_dir}/bin
    LIBRARY DESTINATION ${flavorqual_dir}/lib
    ARCHIVE DESTINATION ${flavorqual_dir}/lib
    ) 

