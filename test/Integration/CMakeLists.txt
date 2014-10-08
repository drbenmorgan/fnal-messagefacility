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

link_libraries( MF_MessageLogger )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

cet_test( MFTest )
cet_test( MFTimingService )

