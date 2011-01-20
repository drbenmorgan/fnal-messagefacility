
execute_process( COMMAND date 
                 OUTPUT_VARIABLE datime 
                 OUTPUT_STRIP_TRAILING_WHITESPACE )

# create package configuration and version files
configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility-config.cmake.in
                 ${PROJECT_BINARY_DIR}/ups/messagefacility-config.cmake @ONLY )
configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility-config-version.cmake.in
                 ${PROJECT_BINARY_DIR}/ups/messagefacility-config-version.cmake @ONLY )
configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility.table.in
                 ${PROJECT_BINARY_DIR}/ups/messagefacility.table @ONLY )
configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility.version.in
                 ${PROJECT_BINARY_DIR}/ups/${UPSFLAVOR}_${qualifier} @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/ups/messagefacility-config.cmake
               ${PROJECT_BINARY_DIR}/ups/messagefacility-config-version.cmake
	 DESTINATION ${flavorqual_dir}/lib/messagefacility-${version} )
install (FILES ${PROJECT_BINARY_DIR}/ups/messagefacility.table
	 DESTINATION ${product}/${version}/ups )
INSTALL (FILES ${PROJECT_BINARY_DIR}/ups/${UPSFLAVOR}_${qualifier}
         DESTINATION ${product}/${version}.version )

