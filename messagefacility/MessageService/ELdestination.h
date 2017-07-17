#ifndef messagefacility_MessageService_ELdestination_h
#define messagefacility_MessageService_ELdestination_h

// ----------------------------------------------------------------------
//
// ELdestination   is a virtual class defining the interface to a
//                 destination.  The ELadministrator owns
//                 a list of ELdestination* as well as the objects those
//                 list elements point to.
//
// ----------------------------------------------------------------------

#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/TableFragment.h"
#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELset.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include "messagefacility/MessageService/MsgStatistics.h"

namespace mf {
  namespace service {
    class ELdestination;
  }
}

namespace cet {
  template <> struct PluginTypeDeducer<mf::service::ELdestination> {
    static std::string const value;
  };
}

namespace mf {
  namespace service {

    class ELdestination {
    public:

      struct Config {
        fhicl::Atom<std::string> dest_type{fhicl::Name{"type"}};
        fhicl::Table<MsgFormatSettings::Config> format{fhicl::Name{"format"}};
        // Assembling the following comment is, shall we say, icky.
        // It's bad; really bad: worse than ending a sentence with a
        // preposition; worse than parallel octaves in counterpoint
        // music.  But alas, it cannot be helped:
        //
        //  - The fhicl-cpp types system would need to be expanded to
        //    provide some mechanism that says "I want to have a table
        //    whose members all have the form of X.  We don't have
        //    that.
        //
        //  - Since C++ does not provide reflection facilities into
        //    struct members and so forth, the 'RegisterIfTableMember'
        //    base class is intentionally instantiated for every
        //    parameter.  This means that even if I wanted to create a
        //    temporary Table<mf::Category::Config> to automatically
        //    generate the allowed configuration, that temporary table
        //    would be registered as belonging to whatever
        //    encapsulating table is currently on the
        //    TableMemberRegistry stack.  This is bad.
        fhicl::OptionalDelegatedParameter categories{fhicl::Name{"categories"},
            fhicl::Comment{
R"(The 'categories' parameter (if provided) is a FHiCL table that
configures the behavior of logging to this destination for the specified
category.  For example, if the following appears in C++ source code:

  mf::LogInfo{"Tracking"} << my_track.diagnostics();

the category is 'Tracking', and its behavior can be specified via:

  categories: {
    Tracking: {
      limit: -1  # default
      reportEvery: -1 # default
      timespan: -1 # default
    }
  }

Within the 'categories' table, it is permitted to specify a 'default'
category, which becomes the configuration for all message categories
that are not explicitly listed.

Note the categories listed only customize the behavior of messages
that are logged specifically to this destination. Messages that are
routed to other destinations are not be affected.

Category parameters
===================

)"+Category::Config::limit_comment()
  +"\n\n"+Category::Config::reportEvery_comment()
  +"\n\n"+Category::Config::timespan_comment()}
        };
        fhicl::Atom<std::string> threshold {
          fhicl::Name{"threshold"},
          fhicl::Comment{"The 'threshold' parameter specifies the lowest severity level of\n"
                         "messages that will be logged to the destination"},
          "INFO"
        };
        fhicl::Atom<bool> noTimeStamps{fhicl::Name{"noTimeStamps"}, false};
        fhicl::Atom<bool> noLineBreaks{fhicl::Name{"noLineBreaks"}, false};
        fhicl::Atom<unsigned long long> lineLength{fhicl::Name{"lineLength"},
                                                   fhicl::Comment{"The following parameter is allowed only if 'noLineBreaks' has been set to 'false'."},
                                                   [this]{ return !noLineBreaks(); }, 80ull};
        fhicl::Atom<bool> useMilliseconds{fhicl::Name{"useMilliseconds"}, false};
        fhicl::Atom<bool> outputStatistics{fhicl::Name{"outputStatistics"}, false};
        fhicl::TableFragment<MsgStatistics::Config> msgStatistics;
      };

      ELdestination(Config const& pset);

      // Suppress copy operations
      ELdestination(ELdestination const& orig) = delete;
      ELdestination& operator= (ELdestination const& orig) = delete;

      virtual ~ELdestination() noexcept = default;


      virtual void finish();
      virtual void log(mf::ErrorObj& msg);
      virtual void noTerminationSummary(){}
      virtual void summarization(std::string const& title,
                                 std::string const& sumLines);
      virtual void summary();
      virtual void wipe();

      void setThreshold(ELseverityLevel sv);
      void userWantsStats() { enableStats = true; }
      bool resetStats() const { return stats.reset; }

    protected:

      void emitToken(std::ostream& os, std::string const& s, bool nl = false);

      bool passLogMsgThreshold  (mf::ErrorObj const& msg);
      bool passLogStatsThreshold(mf::ErrorObj const& msg) const;

      virtual void fillPrefix(std::ostringstream& oss, mf::ErrorObj const& msg);
      virtual void fillUsrMsg(std::ostringstream& oss, mf::ErrorObj const& msg);
      virtual void fillSuffix(std::ostringstream& oss, mf::ErrorObj const& msg);

      virtual void routePayload(std::ostringstream const& oss,
                                mf::ErrorObj const& msg);

      virtual void filterModule(std::string const& moduleName);
      virtual void excludeModule(std::string const& moduleName);
      virtual void ignoreModule(std::string const& moduleName);
      virtual void respondToModule(std::string const& moduleName);
      virtual bool thisShouldBeIgnored(std::string const& s) const;

      virtual void summary(std::ostream& os, std::string const& title = {});
      virtual void summary(std::string& s, std::string const& title = {});

      virtual void flush();

    protected:

      // This block of protected data is icky.  Should find a way to
      // make it private.
      MsgStatistics stats;
      MsgFormatSettings format;
      ELseverityLevel threshold;
      std::size_t lineLength_;

    private:

      void configure(fhicl::OptionalDelegatedParameter const&);

      std::string indent {std::string(6,' ')};
      std::size_t charsOnLine {};
      ELset_string respondToThese {};
      bool ignoreMostModules {false};
      bool respondToMostModules {false};
      ELset_string ignoreThese {};

      bool enableStats;

    }; // ELdestination

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELdestination_h */

// Local variables:
// mode: c++
// End:
