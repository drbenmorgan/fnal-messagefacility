# ======================================================================
#
# Testing script
#
# ======================================================================

link_libraries( MF_Utilities )

cet_test_env(CLEAR "FHICL_FILE_PATH=${CMAKE_CURRENT_SOURCE_DIR}")

cet_test( Exception_t )
cet_test( ExceptionDerived_t )
cet_test( Simple_t )

