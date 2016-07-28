# ======================================================================
#
# Testing
#
# ======================================================================

cet_enable_asserts()
include(CetTest)

# - General Test environment:
cet_test_env("FHICL_FILE_PATH=.")

# - Dynamic loader path
set(SYSTEM_LD_LIBRARY_PATH LD_LIBRARY_PATH)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(SYSTEM_LD_LIBRARY_PATH DYLD_LIBRARY_PATH)
endif()
cet_test_env("${SYSTEM_LD_LIBRARY_PATH}=$<TARGET_FILE_DIR:MF_Utilities>:$ENV{${SYSTEM_LD_LIBRARY_PATH}}")

add_subdirectory(Utilities)
add_subdirectory(Integration)
add_subdirectory(MessageService)
