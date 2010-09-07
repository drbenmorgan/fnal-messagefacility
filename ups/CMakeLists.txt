CONFIGURE_FILE ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility-config.cmake.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility-config.cmake @ONLY )
CONFIGURE_FILE ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility-config-version.cmake.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility-config-version.cmake @ONLY )

CONFIGURE_FILE ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility.table.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility.table @ONLY )

install (FILES ${MessageFacility_BINARY_DIR}/ups/messagefacility-config.cmake
               ${MessageFacility_BINARY_DIR}/ups/messagefacility-config-version.cmake
         DESTINATION lib/messagefacility-${version} )

INSTALL (FILES
    ${MessageFacility_BINARY_DIR}/ups/messagefacility.table
    DESTINATION ups
    )

