#
#  Extensions/src/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file gathering source files for library Extensions and
#  ExtensionsD 
#

SET (SRCFILES 
    DDSReceiver.cc
    ELDDSdest.cc
    ELarchive.cc
    )

SET (QTSRCFILES
    QtDDSReceiver.cc
    )

SET (DDSUTILITYFILES
    CheckStatus.cpp
    )    

SET (DDSIDLFILES
    MessageFacility.idl
    )
