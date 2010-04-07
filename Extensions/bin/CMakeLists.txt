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

SET (MF_PACKAGES
    MessageServiceD
    MessageLoggerD
    ParameterSetD
    UtilitiesD
    ExtensionsD
    )

SET (BOOST_PACKAGES
    boost_program_options
    boost_thread
    boost_system
    boost_filesystem
    )

SET (DDS_PACKAGES
    dcpssacpp
    )

ADD_EXECUTABLE (msgserver
    msgserver.cc
    )

TARGET_LINK_LIBRARIES (msgserver
    ${MF_PACKAGES}
    ${BOOST_PACKAGES}
    ${DDS_PACKAGES}
    )

INSTALL (TARGETS msgserver
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    ) 

ADD_EXECUTABLE (msglogger
    msglogger.cc
    )

TARGET_LINK_LIBRARIES (msglogger
    ${MF_PACKAGES}
    ${BOOST_PACKAGES}
    ${DDS_PACKAGES}
    )

INSTALL (TARGETS msglogger
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

# Include Qt4 module
IF (NOT CMAKE_CROSSCOMPILING)
    FIND_PACKAGE (Qt4 4.5.2 REQUIRED)
    INCLUDE (${QT_USE_FILE})

    IF (QT_WRAP_CPP)
      IF (QT_WRAP_UI)

        QT_WRAP_UI (msgviewer
            msgviewer_ui_headers
            msgviewer_ui_sources
            msgviewerdlgui.ui
            )

        QT_WRAP_CPP (msgviewer
            msgviewer_moc_files
            mvdlg.h
            mvlistener.h
            )

      ENDIF (QT_WRAP_UI) 
    ENDIF (QT_WRAP_CPP)

    ADD_EXECUTABLE (msgviewer
        msgviewer.cc
        mvdlg.cc
        mvlistener.cc
        ${msgviewer_ui_headers}
        ${msgviewer_moc_files}
        )

    TARGET_LINK_LIBRARIES (msgviewer
        ${MF_PACKAGES}
        ${BOOST_PACKAGES}
        ${DDS_PACKAGES}
        ${QT_LIBRARIES}
        )

    INSTALL (TARGETS msgviewer
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        ) 
ENDIF (NOT CMAKE_CROSSCOMPILING)


FILE (GLOB binFiles
    CMakeLists.txt
    *.cc
    *.h
    *.ui
    )
    
INSTALL (FILES ${binFiles}
    DESTINATION src/Extensions/bin
    )
