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

# ----------------------------------------------------------------------
# Identify tests:

set ( TEST_LIST
      MFTest
      MFTimingService
#      mfexample
    )

set ( ALSO_BUILD )

set ( ALSO_RUN )

link_libraries( MF_MessageLogger )

# ----------------------------------------------------------------------
# Build most tests:

foreach ( test ${TEST_LIST} ${ALSO_BUILD} )
  add_executable ( ${test} ${test}.cc )
endforeach ( test ${TEST_LIST} ${ALSO_BUILD} )

# ----------------------------------------------------------------------
# Build tests with special requirements:

#add_executable( sha1_test_performance sha1_test_performance.cc sha1.cpp )
#target_link_libraries( sha1_test_performance cetlib )

# ----------------------------------------------------------------------
# Run tests:

# FIXME MFTest and mfexample need working new-style configurations.
foreach ( test ${TEST_LIST} ${ALSO_RUN} )
  add_test ( ${test} ${EXECUTABLE_OUTPUT_PATH}/${test} )
endforeach ( test ${TEST_LIST} )

# Non-standard tests:

# ======================================================================
