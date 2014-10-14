# ======================================================================
#
# Testing script
#
# ======================================================================

include( BasicPlugin )

basic_plugin( ELpluginTest plugin
              MF_MessageLogger
              NO_INSTALL )

CONFIGURE_FILE (
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageFacilityTest.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/MessageFacility.cfg
    COPYONLY
    )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

cet_test( MFTest 
          LIBRARIES
          MF_MessageLogger )

cet_test( MFTimingService 
          LIBRARIES
          MF_MessageLogger )

