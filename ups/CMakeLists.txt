
execute_process( COMMAND date
                 OUTPUT_VARIABLE datime
                 OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process( COMMAND  ups flavor
                 OUTPUT_VARIABLE flavor
                 OUTPUT_STRIP_TRAILING_WHITESPACE )

configure_file ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility-config.cmake.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility-config.cmake @ONLY )
configure_file ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility-config-version.cmake.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility-config-version.cmake @ONLY )

CONFIGURE_FILE ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility.table.in
                 ${MessageFacility_BINARY_DIR}/ups/messagefacility.table @ONLY )
CONFIGURE_FILE ( ${MessageFacility_SOURCE_DIR}/ups/messagefacility.version.in
                 ${MessageFacility_BINARY_DIR}/ups/${version}.version @ONLY )

install (FILES ${MessageFacility_BINARY_DIR}/ups/messagefacility-config.cmake
               ${MessageFacility_BINARY_DIR}/ups/messagefacility-config-version.cmake
         DESTINATION ${flavorqual_dir}/lib/messagefacility-${version} )

INSTALL (FILES ${MessageFacility_BINARY_DIR}/ups/messagefacility.table
         DESTINATION ${version}/ups)
INSTALL (FILES ${MessageFacility_BINARY_DIR}/ups/${version}.version
         DESTINATION ${CMAKE_INSTALL_PREFIX} )

