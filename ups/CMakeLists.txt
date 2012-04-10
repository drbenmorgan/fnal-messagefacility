
# create package configuration and version files

process_ups_files()

configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility-config.cmake.in
                 ${PROJECT_BINARY_DIR}/ups/messagefacility-config.cmake @ONLY )
configure_file ( ${PROJECT_SOURCE_DIR}/ups/messagefacility-config-version.cmake.in
                 ${PROJECT_BINARY_DIR}/ups/messagefacility-config-version.cmake @ONLY )

install (FILES ${PROJECT_BINARY_DIR}/ups/messagefacility-config.cmake
               ${PROJECT_BINARY_DIR}/ups/messagefacility-config-version.cmake
         DESTINATION ${flavorqual_dir}/lib/messagefacility-${version} )

