# ======================================================================
#
# Testing script
#
# ======================================================================
include(BasicPlugin)

if(ALT_CMAKE)
  add_executable(ELdestinationTester ELdestinationTester.cc)
  target_link_libraries(ELdestinationTester PRIVATE
    MF_MessageLogger
    cetlib::cetlib
    fhiclcpp::fhiclcpp
    Boost::program_options
    Boost::filesystem
    Boost::system
    )
  # Though it's a test executable, does seem to be installed so
  # that clients can test new destinations?
  install(TARGETS ELdestinationTester
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
else()
cet_make_exec(ELdestinationTester
  SOURCE
    ELdestinationTester.cc
  LIBRARIES
    MF_MessageLogger
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)
endif()

cet_test(Presence_init_t
  LIBRARIES
  MF_MessageLogger
  fhiclcpp
)

cet_test(WildcardDebug_t
  LIBRARIES
    MF_MessageLogger
    fhiclcpp
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
  DATAFILES
    fcl/WildcardDebug_t.fcl
  TEST_ARGS
    -c WildcardDebug_t.fcl
  REF
    ${CMAKE_CURRENT_SOURCE_DIR}/WildcardDebug_t-ref.txt
)

if(ALT_CMAKE)
  # much easier to do...
  add_executable(mfexample mfexample.cc)
  target_link_libraries(mfexample PRIVATE MF_MessageLogger fhiclcpp::fhiclcpp)
  # Need to clarify installation - doesn't do much,
  # and fails anyway on Mac:
  # libc++abi.dylib: terminating with uncaught exception of type fhicl::detail::validationException: Any parameters prefaced with '#' are optional.
  # Unsupported parameters:
  #
  # + statistics                     [ -:1 ]
  # + statistics[0]                  [ -:1 ]
  #
  #
  # Abort trap: 6
else()
cet_make_exec(mfexample
  LIBRARIES
  MF_MessageLogger
  fhiclcpp
  pthread
  )
endif()

basic_plugin(Issue17457TestDestination mfPlugin
  MF_MessageLogger fhiclcpp ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
  NO_INSTALL
  )

foreach (N 01 02) # 03 04 05)
  set(test_stem Issue17457_${N})
  cet_test(${test_stem}_t HANDBUILT
    TEST_EXEC ELdestinationTester
    TEST_ARGS -c ${test_stem}.fcl
    DATAFILES fcl/${test_stem}.fcl
    OUTPUT_FILTERS DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/Issue17457-filter
    REF ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.out
    ${CMAKE_CURRENT_SOURCE_DIR}/${test_stem}-ref.err
    )
endforeach()
