# ======================================================================
#
# Testing script
#
# ======================================================================

# Get all FHiCL files
file(GLOB testConfigFiles fhicl-files/*.fcl )
list(REMOVE_ITEM testConfigFiles ${CMAKE_CURRENT_SOURCE_DIR}/fhicl-files/statistics_duplicateDest.fcl)

foreach(filename ${testConfigFiles})

  # trim directory prefixes and .fcl suffix
  string(REGEX REPLACE "(.*/fhicl-files/)(.*)(.fcl)" "\\2" testname ${filename})

  cet_test(messagefacility_${testname}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${filename}
    DATAFILES
    ${filename}
    )

endforeach()

cet_test(messagefacility_statistics_duplicateDest_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS -c statistics_duplicateDest.fcl
  DATAFILES
  fhicl-files/statistics_duplicateDest.fcl
  TEST_PROPERTIES WILL_FAIL TRUE
  )
