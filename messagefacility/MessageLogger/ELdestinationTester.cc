//#define NDEBUG
#define ML_DEBUG    // always enable debug

#include <iostream>
#include <cstdlib>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/ELdestinationTester.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/MessageDrop.h"

namespace {

  void anotherLogger [[ gnu::unused ]] ()
  {
    // Set module name
    mf::MessageDrop::instance()->setSinglet("anotherLogger");

    mf::LogWarning("warn1 | warn2") << "Followed by a WARNING message.";
    mf::LogDebug("debug")           << "The debug message in the other thread";

    return;
  }

  void runModule(std::string const& modulename)
  {
    mf::MessageDrop::instance()->setSinglet(modulename);

    // Post begin job
    mf::SetContext("postBeginJob");
    mf::LogAbsolute("TimeReport")
      << "TimeReport> Report activated\n"
      "TimeReport> Report columns headings for events: "
      "eventnum runnum timetaken\n"
      "TimeReport> Report columns headings for modules: "
      "eventnum runnum modulelabel modulename timetaken";

    // Post end job
    mf::SetContext("postEndJob");
    mf::LogAbsolute("TimeReport")
      << "TimeReport> Time report complete in "
      << 0.0402123 << " seconds\n"
      << " Time Summary: \n"
      << " Min: " << 303  << "\n"
      << " Max: " << 5555 << "\n"
      << " Avg: " << 4000 << "\n";

    // Post event processing
    mf::SetContext("postEventProcessing");
    mf::LogAbsolute("TimeEvent")
      << "TimeEvent> "
      << "run: 1   subRun: 2    event: 456 " << .0440404;

    // Post Module
    mf::SetContext("postModule");
    mf::LogAbsolute("TimeModule")
      << "TimeModule> "
      << "run: 1   subRun: 2    event: 456 "
      << "someString "
      << modulename << " "
      << 0.04404;

    mf::LogSystem("system") << "This would be a major problem, I guess.";
  }

}

namespace bpo = boost::program_options;
int main(int argc, char* argv[])
{
  std::ostringstream descstr;
  descstr << "\nELdestinationTester is a sample program that can be used\n"
          << "to test how messages are logged using different destinations\n"
          << "and configurations.  This can be particularly helpful for\n"
          << "users who wish to supply their own plugin destinations.\n\n"
          << "The configuration file should look like\n"
          << "\n"
          << "  message: {\n"
          << "    destinations: {\n"
          << "      dest1: {\n"
          << "        ...\n"
          << "      }\n"
          << "      ...\n"
          << "    }\n"
          << "  }\n\n";
  descstr << "Usage: "
          << boost::filesystem::path(argv[0]).filename().native()
          << " -c <config-file> \n\n"
          << "Allowed options";
  bpo::options_description desc {descstr.str()};
  desc.add_options()
    ("config,c", bpo::value<std::string>(), "Configuration file.")
    ("help,h", "produce help message");
  bpo::variables_map vm;

  try {
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);
  }
  catch (bpo::error const & e) {
    std::cerr << "Exception from command line processing in "
              << argv[0] << ": " << e.what() << "\n";
    return 2;
  }
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  if (!vm.count("config")) {
    std::cerr << "ERROR: No configuration specified ( -c <config_file> )" << std::endl;
    return 3;
  }
  std::string const config_string = vm["config"].as<std::string>();

  cet::filepath_lookup_nonabsolute filepath {"FHICL_FILE_PATH"};

  fhicl::ParameterSet main_pset;
  try {
    // create an intermediate table from the input string
    make_ParameterSet(config_string, filepath, main_pset);
  }
  catch (cet::exception const& e) {
    std::cerr << "ERROR: Failed to create a parameter set from an input configuration string with exception "
              << e.what()
              << ".\n";
    std::cerr << "       Input configuration string follows:\n"
              << "------------------------------------"
              << "------------------------------------"
              << "\n";
    std::cerr << config_string << "\n";
    std::cerr << "------------------------------------"
              << "------------------------------------"
              << "\n";
    return 7003;
  }

  // Start MessageFacility Service
  try {
    mf::StartMessageFacility(main_pset.get<fhicl::ParameterSet>("message"));
  }
  catch (mf::Exception const& e) {
    std::cerr << e.what() << std::endl;
    return 4;
  }
  catch (...) {
    std::cerr << "Caught unknown exception from mf::StartMessageFacility" << std::endl;
    return 5;
  }

  // Set module name for the main thread
  mf::SetApplicationName("MessageFacility");
  mf::MessageDrop::instance()->setSinglet("MFTest");
  mf::SetContext("pre-event");

  // Start up another logger in a separate thread
  //boost::thread loggerThread(anotherLogger);

  // Memory Check output
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.043 0.628906";
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.25 0.199219";
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 5.2 RSS 357.453 0.1875";

  mf::LogInfo linfo("info");
  linfo << " vint contains: ";

  std::vector<int> vint { 1, 2, 5, 89, 3 };

  auto i = std::begin(vint);
  auto const e = std::end(vint);
  while (i != e) {
    linfo << *i;
    if (++i != e) {
      linfo << ", ";
    }
  }

  // Issue messages with different severity levels
  mf::LogError("err1|err2") << "This is an ERROR message.";
  mf::LogWarning("warning") << "Followed by a WARNING message.";

  // Switch context
  mf::SetContext("pro-event");

  // Log Debugs
  for(int i = 0; i != 5; ++i) {
    mf::LogError("catError")     << "Error information.";
    mf::LogWarning("catWarning") << "Warning information.";
    mf::LogInfo("catInfo")       << "Info information.";
    LOG_DEBUG("debug")           << "DEBUG information.";
  }

  // Test move operations
  {
    // normal macro
    auto logPROBLEM = LOG_PROBLEM("problem");
    logPROBLEM << "clever way to ensure persistence of temporary object";

    // macro w/ternary operator
    auto log = LOG_DEBUG("debug2") << "first line.\n";
    log << "second line.";
  }

  runModule("module1");
  runModule("module5");
  mf::LogStatistics();

}
