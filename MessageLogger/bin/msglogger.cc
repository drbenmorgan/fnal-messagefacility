//
// msgLogger.cc
// ------------------------------------------------------------------
// Command line appication to send a message through MessageFacility.
// 
// ql   03/01/2010
//

#include "MessageLogger/interface/MessageLogger.h"

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

using namespace boost;
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;


std::string trim_copy(std::string const src)
{
  std::string::size_type len = src.length();
  std::string::size_type i    = 0;
  std::string::size_type j    = len-1;

  while( (i < len) && (src[i] == ' ') ) ++i;
  while( (j > 0  ) && (src[j] == ' ') ) --j;

  return src.substr(i,j-i+1);
}

void parseDestinations (string const & s, vector<string> & dests)
{
  dests.clear();

  const std::string::size_type npos = s.length();
        std::string::size_type i    = 0;
  while ( i < npos ) {    
    std::string::size_type j = s.find('|',i); 
    std::string dest = trim_copy(s.substr(i,j-i));  
    dests.push_back (dest);
    i = j;
    while ( (i < npos) && (s[i] == '|') ) ++i; 
    // the above handles cases of || and also | at end of string
  } 
}

int main(int ac, char* av[])
{
  string         severity;
  string         message;
  string         cat;
  string         dest;
  vector<string> vcat;
  vector<string> vdest;

  vector<string> vcat_def;
  vector<string> vdest_def;

  vcat_def.push_back("cat");
  vdest_def.push_back("terminal");

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "display help message")
      ("severity,s", po::value<string>(&severity)->default_value("info"), 
        "severity of the message (error, warning, info, debug)")
      ("category,c", po::value< vector<string> >(&vcat)->default_value(vcat_def, ""),
        "message id / categories")
      ("destination,d", po::value< vector<string> >(&vdest)->default_value(vdest_def, ""),
        "logging destination(s) of the message (stdout, file, server)")
      ("message,m", po::value<string>(&message), "message body");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
      cout << "Usage: msglogger [options]\n";
      cout << desc;
      return 0;
    }

    // "--message" is a must
    if(!vm.count("message"))
    {
      cout << "Message body is missing!\n";
      cout << "Use \"msglogger --help\" for help messages\n";
      return 1;
    }
  } 
  catch(std::exception & e)
  {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << "Exception of unknown type!\n";
    return 1;
  }
    

  // checking severity...
  transform(severity.begin(), severity.end(), severity.begin(), ::toupper);
  if( (severity!="ERROR") && (severity!="WARNING")
      && (severity!="INFO") && (severity!="DEBUG") )
  {
    cerr << "Unknown severity level!\n";
    return 1;
  }

  // checking categories..
  vector<string>::iterator it = vcat.begin();
  while(it!=vcat.end())
  {
    cat += *it + "|";
    ++it;
  }

  // checking destiantions...
  it = vdest.begin();
  while(it!=vdest.end())
  {
    transform((*it).begin(), (*it).end(), (*it).begin(), ::toupper);
    dest += *it + "|";
    ++it;
  }

  // parsing destinations...
  parseDestinations(dest, vdest);
  int flag = 0x00;
  it = vdest.begin();
  while(it!=vdest.end())
  {
    if( (*it) == "STDOUT" )      flag = flag | 0x01;
    else if( (*it) == "FILE" )   flag = flag | 0x02;
    else if( (*it) == "SERVER")  flag = flag | 0x04;
    ++it;
  }

  // preparing parameterset for detinations...
  mf::ParameterSet pset;
  switch(flag)
  {
    case 0x01:
      pset = mf::MessageFacilityService::instance().logConsole;
      break;
    case 0x02:
      pset = mf::MessageFacilityService::instance().logFile;
      break;
    case 0x03:
      pset = mf::MessageFacilityService::instance().logCF;
      break;
    case 0x04:
      pset = mf::MessageFacilityService::instance().logServer;
      break;
    case 0x05:
      pset = mf::MessageFacilityService::instance().logCS;
      break;
    case 0x06:
      pset = mf::MessageFacilityService::instance().logFS;
      break;
    case 0x07:
      pset = mf::MessageFacilityService::instance().logCFS;
      break;
    default:
      pset = mf::MessageFacilityService::instance().logConsole;
  }

  // start up message facility service
  boost::shared_ptr<mf::Presence> MFPresence;
  mf::StartMessageFacility("SingleThreadMSPresence", MFPresence, pset);
  mf::SetModuleName("msgLogger");
  
  // logging message...
  if( severity == "ERROR" )
    LogError  (cat) << message;
  else if(severity == "WARNING" )
    LogWarning(cat) << message;
  else if(severity == "INFO"    )
    LogInfo   (cat) << message;
  else if(severity == "DEBUG"   )
    LogDebug  (cat) << message;
  

  return 0;
}
