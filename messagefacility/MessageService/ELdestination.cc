//======================================================================
//
// ELdestination
//
//======================================================================

#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/ELdestination.h"

#include <fstream>
#include <iostream>
#include <iomanip>

std::string const
cet::PluginTypeDeducer<mf::service::ELdestination>::
value = "ELdestination";

namespace mf {
  namespace service {

    namespace {
      std::string const noSummarizationMsg {"No summarization()"};
      std::string const noSummaryMsg {"No summary()"};
      std::string const hereMsg {"available via this destination"};
      std::string const noosMsg {"No ostream"};
      std::string const notELoutputMsg {"This destination is not an ELoutput"};
      std::string const preamble {"%MSG"};
    }

    //=============================================================================
    ELdestination::ELdestination()
      : ELdestination{fhicl::ParameterSet{}}
    {}

    ELdestination::ELdestination(fhicl::ParameterSet const& pset)
      : stats{pset}
      , format{pset.get<fhicl::ParameterSet>("format", {})}
      , enableStats{pset.get<bool>("outputStatistics", false)}
    {
      if (enableStats) {
        auto const& dest_type = pset.get<std::string>("type","file");
        ELdestConfig::checkType(dest_type, ELdestConfig::STATISTICS);
      }
    }

    //=============================================================================
    void ELdestination::emit(std::ostream& os,
                             std::string const& s,
                             bool const nl)
    {
      if (s.length() == 0)  {
        if (nl)  {
          os << '\n';
          charsOnLine = 0;
        }
        return;
      }

      char first = s[0];
      char second,
        last,
        last2;
      second = (s.length() < 2) ? '\0' : s[1];
      last = (s.length() < 2) ? '\0' : s[s.length()-1];
      last2 = (s.length() < 3) ? '\0' : s[s.length()-2];
      //checking -2 because the very last char is sometimes a ' ' inserted
      //by ErrorLog::operator<<

      if (format.preambleMode) {

        //Accounts for newline @ the beginning of the std::string
        if (first == '\n' || (charsOnLine + static_cast<int>(s.length())) > lineLength) {
          charsOnLine = 0;
          if (second != ' ') {
            os << ' ';
            charsOnLine++;
          }
          if (first == '\n') {
            os << s.substr(1);
          }
          else {
            os << s;
          }
        }
        else {
          os << s;
        }

        if (last == '\n' || last2 == '\n') {  // accounts for newline @ end
          os << indent;                       // of the std::string
          if (last != ' ')
            os << ' ';
          charsOnLine = indent.length() + 1;
        }

        if (nl) { os << '\n'; charsOnLine = 0;           }
        else    {             charsOnLine += s.length(); }
      }

      if (!format.preambleMode) {
        os << s;
      }

    }  // emit()

    //=============================================================================
    bool ELdestination::passLogStatsThreshold(mf::ErrorObj const& msg) const
    {
      // See if this message is to be counted.
      if (msg.xid().severity < threshold) return false;
      if (thisShouldBeIgnored(msg.xid().module)) return false;

      return true;
    }

    //=============================================================================
    bool ELdestination::passLogMsgThreshold(mf::ErrorObj const& msg)
    {
      auto const& xid = msg.xid();

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      if (xid.severity < threshold)  return false;
      if (xid.severity < ELsevere && thisShouldBeIgnored(xid.module)) return false;
      if (xid.severity < ELsevere && !stats.limits.add(xid)) return false;

      return true;
    }

    void ELdestination::fillPrefix(std::ostringstream& oss,
                                   mf::ErrorObj const& msg,
                                   ELcontextSupplier const& contextSupplier)
    {
      // No prefix for verbatim messages
      if (msg.is_verbatim()) return;

      // Output the prologue:
      //
      format.preambleMode = true;

      auto xid = msg.xid();      // Save the xid.

      charsOnLine = 0;
      emit(oss, preamble);
      emit(oss, xid.severity.getSymbol());
      emit(oss, " ");
      emit(oss, xid.id);
      emit(oss, msg.idOverflow());
      emit(oss, ": ");

      // Output serial number of message:
      //
      if (format.want(SERIAL)) {
        std::ostringstream s;
        s << msg.serial();
        emit(oss, "[serial #" + s.str() + "] ");
      }

      // Provide further identification:
      //
      bool needAspace = true;
      if (format.want(EPILOGUE_SEPARATE)) {
        if (xid.module.length()+xid.subroutine.length() > 0) {
          emit(oss,"\n");
          needAspace = false;
        }
        else if (format.want(TIMESTAMP) && !format.want(TIME_SEPARATE)) {
          emit(oss,"\n");
          needAspace = false;
        }
      }
      if (format.want(MODULE) && (xid.module.length() > 0)) {
        if (needAspace) {
          emit(oss," ");
          needAspace = false;
        }
        emit(oss, xid.module + " ");
      }
      if (format.want(SUBROUTINE) && (xid.subroutine.length() > 0) ) {
        if (needAspace) {
          emit(oss," ");
          needAspace = false;
        }
        emit(oss, xid.subroutine + "() ");
      }

      // Provide time stamp:
      //
      if (format.want(TIMESTAMP))  {
        if (format.want(TIME_SEPARATE))  {
          emit(oss, "\n");
          needAspace = false;
        }
        if (needAspace) {
          emit(oss," ");
          needAspace = false;
        }
        emit(oss, format.timestamp(msg.timestamp()) + " ");
      }

      // Provide the context information:
      //
      if (format.want(SOME_CONTEXT)) {
        if (needAspace) {
          emit(oss, " ");
          needAspace = false;
        }
        if (format.want(FULL_CONTEXT)) {
          emit(oss, contextSupplier.fullContext());
        } else {
          emit(oss, contextSupplier.context());
        }
      }

    }

    //=============================================================================
    void ELdestination::fillUsrMsg (std::ostringstream& oss,
                                    mf::ErrorObj const& msg)
    {
      // No user message if text is not wanted.
      if (!format.want(TEXT)) return;

      format.preambleMode = false;
      auto const usrMsgStart = std::next(msg.items().cbegin(), 4);
      auto it = msg.items().cbegin();

      // Determine if file and line should be included
      if  (!msg.is_verbatim()) {

        // The first four items are { " ", "<FILENAME>", ":", "<LINE>" }
        while (it != usrMsgStart) {
          if (!it->compare(" ") && !std::next(it)->compare("--")) {
            // Do not emit if " --:0" is the match
            std::advance(it,4);
          }
          else {
            // Emit if <FILENAME> and <LINE> are meaningful
            emit(oss, *it++);
          }
        }

        // Check for user-requested line breaks
        if (format.want(NO_LINE_BREAKS)) emit(oss, " ==> ");
        else emit(oss, "", true);
      }

      // For verbatim (and user-supplied) messages, just print the contents
      for ( ;  it != msg.items().cend(); ++it ) {
        emit(oss, *it);
      }

    }

    //=============================================================================
    void ELdestination::fillSuffix(std::ostringstream& oss,
                                   mf::ErrorObj const& msg)
    {
      if (!msg.is_verbatim() && !format.want(NO_LINE_BREAKS)) {
        emit(oss, "\n%MSG");
      }
      oss << '\n';
    }

    //=============================================================================
    void ELdestination::routePayload(std::ostringstream const&,
                                     mf::ErrorObj const&,
                                     ELcontextSupplier const&)
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    //=============================================================================
    void ELdestination::log(mf::ErrorObj& msgObj, ELcontextSupplier const& contextSupplier)
    {
      if (!passLogMsgThreshold(msgObj)) return;

      std::ostringstream payload;
      fillPrefix(payload, msgObj, contextSupplier);
      fillUsrMsg(payload, msgObj);
      fillSuffix(payload, msgObj);

      routePayload(payload, msgObj, contextSupplier);

      msgObj.setReactedTo(true);

      if (enableStats && passLogStatsThreshold(msgObj))
        stats.log(msgObj, contextSupplier);
    }

    // Each of the functions below must be overridden by any
    // destination for which they make sense.  In this base class,
    // where they are all no-ops, the methods which generate data to a
    // destination, stream or stream will warn at that place, and all
    // the no-op methods will issue an ELwarning2 at their own
    // destination.

    void ELdestination::wipe()
    {
      stats.limits.wipe();
    }

    void ELdestination::respondToModule(std::string const& moduleName)
    {
      if (moduleName=="*") {
        ignoreMostModules = false;
        respondToMostModules = true;
        ignoreThese.clear();
        respondToThese.clear();
      } else {
        respondToThese.insert(moduleName);
        ignoreThese.erase(moduleName);
      }
    }

    void ELdestination::ignoreModule(std::string const& moduleName)
    {
      if (moduleName=="*") {
        respondToMostModules = false;
        ignoreMostModules = true;
        respondToThese.clear();
        ignoreThese.clear();
      } else {
        ignoreThese.insert(moduleName);
        respondToThese.erase(moduleName);
      }
    }

    void ELdestination::filterModule(std::string const& moduleName)
    {
      ignoreModule("*");
      respondToModule(moduleName);
    }

    void ELdestination::excludeModule(std::string const& moduleName)
    {
      respondToModule("*");
      ignoreModule(moduleName);
    }

    void ELdestination::summary(ELcontextSupplier const& contextSupplier)
    {
      if (enableStats && stats.updatedStats && stats.printAtTermination)
        {
          std::ostringstream payload;
          payload << "\n=============================================\n\n"
                  << "MessageLogger Summary\n"
                  << stats.formSummary();
          routePayload(payload, mf::ErrorObj{ELzeroSeverity, noosMsg}, contextSupplier);
        }
    }

    void ELdestination::summary(std::ostream& os, std::string const& title)
    {
      os << preamble
         << ELwarning2.getSymbol() << " "
         << noSummaryMsg << " "
         << hereMsg << '\n'
         << title << '\n';
    }

    void ELdestination::summary(std::string& s, std::string const& title)
    {
      std::ostringstream ss;
      summary(ss, title);
      s = ss.str();
    }

    void ELdestination::summaryForJobReport(std::map<std::string, double>&)
    {}

    void ELdestination::finish()
    {}

    void ELdestination::setThreshold(ELseverityLevel const sv)
    {
      threshold = sv;
    }

    void ELdestination::setLimit(std::string const& s, int const n)
    {
      stats.limits.setLimit(s, n);
    }

    void ELdestination::setInterval(ELseverityLevel const sv, int const interval)
    {
      stats.limits.setInterval(sv, interval);
    }

    void ELdestination::setInterval(std::string const& s, int const interval)
    {
      stats.limits.setInterval(s, interval);
    }

    void ELdestination::setLimit(ELseverityLevel const sv, int const n)
    {
      stats.limits.setLimit(sv, n);
    }

    void ELdestination::setTimespan(std::string const& s, int const n)
    {
      stats.limits.setTimespan(s, n);
    }

    void ELdestination::setTimespan(ELseverityLevel const sv, int const n)
    {
      stats.limits.setTimespan(sv, n);
    }

    void ELdestination::formatSuppress(flag_enum const FLAG)
    {
      format.suppress(FLAG);
    }

    void ELdestination::formatInclude(flag_enum const FLAG)
    {
      format.include(FLAG);
    }

    void ELdestination::summarization(std::string const& title,
                                      std::string const& /*sumfines*/,
                                      ELcontextSupplier const& contextSupplier)
    {
      mf::ErrorObj msg {ELwarning2, noSummarizationMsg};
      msg << hereMsg << '\n' << title;
      log(msg, contextSupplier);
    }

    void ELdestination::changeFile(std::ostream&, ELcontextSupplier const& contextSupplier)
    {
      mf::ErrorObj msg {ELwarning2, noosMsg};
      msg << notELoutputMsg;
      log(msg, contextSupplier);
    }

    void ELdestination::changeFile(std::string const& filename,
                                   ELcontextSupplier const& contextSupplier)
    {
      mf::ErrorObj msg {ELwarning2, noosMsg};
      msg << notELoutputMsg << '\n' << "file requested is" << filename;
      log(msg, contextSupplier);
    }

    void ELdestination::flush(ELcontextSupplier const& contextSupplier)
    {
      mf::ErrorObj msg {ELwarning2, noosMsg};
      msg << "cannot flush()";
      log(msg, contextSupplier);
    }

    // ----------------------------------------------------------------------
    // Output format options:
    // ----------------------------------------------------------------------

    int ELdestination::setLineLength (int const len)
    {
      int const temp = lineLength;
      lineLength = len;
      return temp;
    }

    int ELdestination::getLineLength () const { return lineLength; }

    // ----------------------------------------------------------------------
    // Protected helper methods:
    // ----------------------------------------------------------------------

    bool ELdestination::thisShouldBeIgnored(std::string const& s) const
    {
      if (respondToMostModules) {
        return ignoreThese.find(s) != ignoreThese.end();
      } else if (ignoreMostModules) {
        return respondToThese.find(s) == respondToThese.end();
      } else {
        return false;
      }
    }

  } // end of namespace service
} // end of namespace mf
