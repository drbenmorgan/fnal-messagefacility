# ======================================================================
#
# Testing script
#
# ======================================================================

# Get all FHiCL files
FILE (GLOB testConfigFiles fhicl-files/*.fcl )

foreach ( filename ${testConfigFiles} )

  # trim directory prefixes and .fcl suffix
  string( REGEX REPLACE "(.*/fhicl-files/)(.*)(.fcl)" "\\2" testname ${filename} )

  cet_test( messagefacility_${testname}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${filename}
    DATAFILES
    ${filename}
    )

endforeach()
