# ======================================================================
#
# Testing script
#
# ======================================================================

include( BasicPlugin )

basic_plugin( ELpluginTest mfplugin
  MF_MessageLogger
  NO_INSTALL )

CONFIGURE_FILE (
  ${CMAKE_CURRENT_SOURCE_DIR}/MessageFacilityTest.cfg
  ${CMAKE_CURRENT_BINARY_DIR}/MessageFacility.cfg
  COPYONLY
  )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

cet_test( messagefacility_destination_cout_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_cout.fcl
  )

cet_test( messagefacility_destination_cerr_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_cerr.fcl
  )

cet_test( messagefacility_destination_file_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_file.fcl
  )          

cet_test( messagefacility_destination_plugin_t HANDBUILT
  TEST_EXEC ELdestinationTester
  TEST_ARGS messageFacility_ELplugin.fcl
  )          
