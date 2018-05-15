# Generate mf_binreloc private interface
# Just mangles symbols which we need for libs in case several linked libs
# use their own binreloc. It is private, so will not be installed
string(RANDOM LENGTH 16 MANGLE_BINRELOC)
set(MANGLE_BINRELOC "mf_reloc_binreloc${MANGLE_BINRELOC}")
configure_file(mf_binreloc.h.in mf_binreloc.h @ONLY)

include(CetMake)

cet_make_library(
  LIBRARY_NAME
    MF_MessageLogger
  SOURCE
    MessageLogger/MessageLogger.cc
    MessageLogger/MFConfig.cc
    MessageService/ELdestination.cc
    MessageService/ELostreamOutput.cc
    MessageService/ELstatistics.cc
    Utilities/ELextendedID.cc
    Utilities/ELseverityLevel.cc
    Utilities/ErrorObj.cc
    Utilities/exception.cc
    mf_binreloc.c
  LIBRARIES
    cetlib
    cetlib_except
    fhiclcpp
    hep_concurrency
    ${TBB})

# Enable binreloc via private compiler definition
target_compile_definitions(MF_MessageLogger PRIVATE ENABLE_BINRELOC)

add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
add_subdirectory(plugins)
# Testing
add_subdirectory(test)
