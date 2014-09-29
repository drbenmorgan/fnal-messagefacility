# ======================================================================
#
# Testing script
#
# ======================================================================

# ----------------------------------------------------------------------
# Identify tests:

set ( TEST_LIST_CC
      MLexampleModule_1
      MemoryTestClient_A
      MemoryTestClient_B
      MessageLoggerClient
      PSetTestClient_A
      ProblemTestClient_t1
      UnitTestClient_A
      UnitTestClient_B
      UnitTestClient_C
      UnitTestClient_D
      UnitTestClient_E
      UnitTestClient_F
      UnitTestClient_G
      UnitTestClient_H
      UnitTestClient_I
      UnitTestClient_J
      UnitTestClient_K
      UnitTestClient_L
      UnitTestClient_M
      UnitTestClient_N
      UnitTestClient_O
      UnitTestClient_P
      UnitTestClient_Q
      UnitTestClient_R
      UnitTestClient_S
      UnitTestClient_SLumi
      UnitTestClient_T
      UnitTestClient_V
      makeSignals
    )

set ( TEST_LIST_CPP
     makeJobReport
     standAloneTest
     testLoggerInCmsRun
     trivial_main
     u1
     unitTestsGroup_1
     unitTestsGroup_2
     unitTestsGroup_3
     unitTestsGroup_4
     unitTestsGroup_5
     unitTestsGroup_6
     unitTestsLimits
     unitTestsMultithread_1
     unitTestsMultithread_2
     unitTestsMultithread_3
     unitTestsStandAlone
     unitTestsStatistics
    )

set ( TEST_LIST_CPPNEW
     forKicks
    )

set ( ALSO_RUN )

link_libraries( MF_MessageLogger )

# ----------------------------------------------------------------------
# Build most tests:

foreach ( test ${TEST_LIST_CPP} ${ALSO_BUILD_CPP} )
#  add_executable ( ${test} ${test}.cpp )
endforeach ( test ${TEST_LIST_CPP} ${ALSO_BUILD_CPP} )

foreach ( test ${TEST_LIST_CC} ${ALSO_BUILD_CC} )
#  add_executable ( ${test} ${test}.cc )
endforeach ( test ${TEST_LIST_CC} ${ALSO_BUILD_CC} )

foreach ( test ${TEST_LIST_CPPNEW} ${ALSO_BUILD_CPPNEW} )
  add_executable ( ${test} ${test}.cpp )
endforeach ( test ${TEST_LIST_CPPNEW} ${ALSO_BUILD_CPPNEW} )

# ----------------------------------------------------------------------
# Build tests with special requirements:

#add_executable( sha1_test_performance sha1_test_performance.cc sha1.cpp )
#target_link_libraries( sha1_test_performance cetlib )

# ----------------------------------------------------------------------
# Run tests:

foreach ( test ${TEST_LIST_CPPNEW} ${ALSO_RUN} )
  add_test ( ${test} ${EXECUTABLE_OUTPUT_PATH}/${test} )
endforeach ( test ${TEST_LIST_CPPNEW} )

# Non-standard tests:

# ======================================================================
