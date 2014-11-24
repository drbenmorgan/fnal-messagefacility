# ======================================================================
#
# Testing script
#
# ======================================================================

# Get all FHiCL files
FILE (GLOB testConfigFiles fhicl-files/*.fcl )

# Exclude the following *.fcl files
# LIST ( REMOVE_ITEM testConfigFiles )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

foreach ( filename ${testConfigFiles} )

  # trim directory prefixes and .fcl suffix
  string( REGEX REPLACE "(.*/fhicl-files/)(.*)(.fcl)" "\\2" testname ${filename} )

  cet_test( messagefacility_${testname}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS ${filename}
    )

endforeach()
