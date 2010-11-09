# ======================================================================
#
# Testing script
#
# ======================================================================

message (CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR})
message (CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR})

CONFIGURE_FILE (
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageFacilityTest.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/MessageFacility.cfg
    COPYONLY
    )

# ----------------------------------------------------------------------
# Identify tests:

# FIXME MFTest and mfexample need working new-style configurations.
set ( TEST_LIST
#      MFTest
#      mfexample
    )

set ( ALSO_BUILD )

set ( ALSO_RUN )

link_libraries( MessageService Utilities )

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

foreach ( test ${TEST_LIST} ${ALSO_RUN} )
  add_test ( ${test} ${EXECUTABLE_OUTPUT_PATH}/${test} )
endforeach ( test ${TEST_LIST} )

# Non-standard tests:

# ======================================================================
