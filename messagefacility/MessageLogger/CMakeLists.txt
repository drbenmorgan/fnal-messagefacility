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
       ../MessageService/ELsyslog.cc
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

cet_make_library( LIBRARY_NAME MF_MessageLogger
        	  SOURCE  ${srcFiles} ${MSsrcFiles}
        	  LIBRARIES MF_Utilities ${FHICLCPP} ${Boost_THREAD_LIBRARY} dl
        	  WITH_STATIC_LIBRARY )

# install files
install_headers( EXTRAS classes.h )
install_source( EXTRAS CMakeLists.txt )
