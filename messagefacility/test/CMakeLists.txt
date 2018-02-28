# ======================================================================
#
# Testing
#
# ======================================================================

cet_enable_asserts()
include(CetTest)

# In ALT_CMAKE mode, we do a couple of things
if(ALT_CMAKE)
  # Create interface targets for imported targets. In ALT_CMAKE,
  # targets are namespaced, e.g. "cetlib::cetlib" so that CMake
  # can guarantee their availability (plain "cetlib" can result in a
  # "-lcetlib" flag)
  foreach(_ac_cetpkg cetlib_except cetlib fhiclcpp)
    add_library(${_ac_cetpkg} INTERFACE)
    target_link_libraries(${_ac_cetpkg} INTERFACE ${_ac_cetpkg}::${_ac_cetpkg})
  endforeach()

  # Add output path to CETD_LIBRARY_PATH
  cet_test_env("CETD_LIBRARY_PATH=$<TARGET_FILE_DIR:MF_MessageLogger>:$ENV{CETD_LIBRARY_PATH}")
  # Scripts will run programs, so add to PATH
  # NB: many tests use:
  #
  #   HANDBUILT TEST_EXEC ELdestinationTester
  #
  # This (via cet_test_exec) relies on finding EldestinationTester in the PATH
  # so genexs are a better nd more robust solution:
  #
  #   HANDBUILT TEST_EXEC $<TARGET_FILE:ELdestinationTester>
  #
  # which *guarantees* the local build to be used.
  cet_test_env("PATH=$<TARGET_FILE_DIR:ELdestinationTester>:$ENV{PATH}")
endif()


cet_test_env("FHICL_FILE_PATH=.")

add_subdirectory(Utilities)
add_subdirectory(Integration)
add_subdirectory(MessageService)

