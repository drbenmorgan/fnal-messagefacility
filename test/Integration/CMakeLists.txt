# ======================================================================
#
# Testing script
#
# ======================================================================

CONFIGURE_FILE (
  ${CMAKE_CURRENT_SOURCE_DIR}/MessageFacilityTest.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/MessageFacility.cfg
  COPYONLY
  )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

# cout test
cet_test( messagefacility_destination_cout_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_cout.fcl
  )

# cerr test
cet_test( messagefacility_destination_cerr_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_cerr.fcl
  )

# cerr_withStatistics test
cet_test( messagefacility_destination_cerr_wStatistics_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_cerr_wStatistics.fcl
  )

# file test
cet_test( messagefacility_destination_file_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_file.fcl
  )          

# syslog test
cet_test( messagefacility_destination_syslog_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_syslog.fcl
  )          

# statistics test
cet_test( messagefacility_destination_statistics_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_statistics.fcl
  )
