# - When user supplies "-DALT_CMAKE" use the non-CET/UPS system
if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()

include(CetMake)

add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
# Testing
add_subdirectory(test)

endif() # ALT_CMAKE
