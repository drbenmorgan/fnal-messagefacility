# ======================================================================
#
# Testing script
#
# ======================================================================

# ----------------------------------------------------------------------
# Identify tests:

set ( TEST_LIST
      Exception_t
      ExceptionDerived_t
      Simple_t
    )

set ( ALSO_BUILD )

set ( ALSO_RUN )

link_libraries( MF_Utilities )

# ----------------------------------------------------------------------
# Build most tests:

foreach ( test ${TEST_LIST} ${ALSO_BUILD} )
  add_executable ( ${test} ${test}.cpp )
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
