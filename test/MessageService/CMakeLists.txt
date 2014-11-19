# ======================================================================
#
# Testing script
#
# ======================================================================

set ( TEST_LIST
 destination_cerr
 destination_cerr_wStatistics
 destination_cout
 destination_file
 destination_file_categories
 destination_file_duplicateDest
 destination_file_filenameOptions
 destination_file_lineLength
 destination_file_noLineBreaks
 destination_file_noTimeStamps
 destination_file_placeholder
 destination_file_threshold
 destination_file_useMilliseconds
 destination_file_wStatistics1
 destination_file_wStatistics2
 destination_file_wStatistics3
 destination_syslog
 fwkJobReports
 statistics
 )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

foreach ( test ${TEST_LIST} )
  cet_test( messagefacility_${test}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS fhicl-files/${test}.fcl
    )
endforeach()
