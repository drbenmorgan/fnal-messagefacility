#
#  Extension/bin/CMakeLists.txt
#  -------------------------------------------------------------------
#
#  CMake build file for MFDDSserver and msgviewer
#


SET (SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

SET (BIN_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    )


# Include Qt4 module
FIND_PACKAGE (Qt4 4.5.2 REQUIRED)
INCLUDE (${QT_USE_FILE})

# add include directories
INCLUDE_DIRECTORIES (
    ${MessageFacility_SOURCE_DIR}
    ${SRC_DIR}
    ${BIN_DIR}/..
    $ENV{OSPL_HOME}/include
    $ENV{OSPL_HOME}/include/sys
    $ENV{OSPL_HOME}/include/dcps/C++/SACPP
    )

LINK_DIRECTORIES (
    $ENV{OSPL_HOME}/lib
    )

FILE (GLOB serverSrcFiles
    MFDDSserver.cc
    ${SRC_DIR}/../src/CheckStatus.cpp
    ${BIN_DIR}/../DDSdest_bld/*.cpp
    )

ADD_EXECUTABLE (MFDDSserver
    ${serverSrcFiles}
    )

TARGET_LINK_LIBRARIES (MFDDSserver
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
    boost_thread
    boost_filesystem
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    )

INSTALL (TARGETS MFDDSserver
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

IF (QT_WRAP_CPP)
  IF (QT_WRAP_UI)

    QT_WRAP_UI (msgviewer
        msgviewer_ui_headers
        msgviewer_ui_sources
        msgviewerdlgui.ui
        )

    QT_WRAP_CPP (msgviewer
        msgviewer_moc_files
        msgviewerdlg.h
        )

  ENDIF (QT_WRAP_UI) 
ENDIF (QT_WRAP_CPP)

ADD_EXECUTABLE (msgviewer
    msgviewer.cc
    msgviewerdlg.cc
    ${msgviewer_ui_headers}
    ${msgviewer_moc_files}
    )

TARGET_LINK_LIBRARIES (msgviewer
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
    boost_thread
    boost_filesystem
    ddsdatabase
    dcpsgapi
    dcpssacpp
    ddsos
    ${QT_LIBRARIES}
    )

INSTALL (TARGETS msgviewer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

