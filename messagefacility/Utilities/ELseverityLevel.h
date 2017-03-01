#ifndef messagefacility_Utilities_ELseverityLevel_h
#define messagefacility_Utilities_ELseverityLevel_h


// ----------------------------------------------------------------------
//
// ELseverityLevel.h - declare objects that encode a message's urgency
//
//      Both frameworker and user will often pass one of the
//      instantiated severity levels to logger methods.
//
//      The only other methods of ELseverityLevel a frameworker
//      might use is to check the relative level of two severities
//      using operator< or the like.
//
// ----------------------------------------------------------------------

#include <string>

namespace mf {

  class ELseverityLevel;

  using ELslGen = ELseverityLevel();

  // ----------------------------------------------------------------------
  // ELslProxy class template:
  // ----------------------------------------------------------------------

  template<ELslGen ELgen>
  struct ELslProxy {

    operator ELseverityLevel() const;

    int         getLevel()    const;
    std::string getSymbol()   const;
    std::string getName()     const;
    std::string getInputStr() const;
    std::string getVarName()  const;

  };  // ELslProxy<ELslGen>

  // ----------------------------------------------------------------------
  // ELseverityLevel:
  // ----------------------------------------------------------------------

  class ELseverityLevel  {
  public:

    // ---  One ELseverityLevel is globally instantiated (see below)
    // ---  for each of the following levels:

    enum ELsev_ {
      ELsev_noValueAssigned = 0  // default returned by map when not found
        , ELsev_zeroSeverity         // threshold use only
        , ELsev_success              // report reaching a milestone
        , ELsev_info                 // information
        , ELsev_warning              // warning
        , ELsev_error                // error detected
        , ELsev_unspecified          // severity was not specified
        , ELsev_severe               // future results are suspect
        , ELsev_highestSeverity      // threshold use only
        // -----
        , nLevels                    // how many levels?
        };  // ELsev_

    constexpr ELseverityLevel(ELsev_ lev = ELsev_unspecified);
    ELseverityLevel (std::string const& str);
    // str may match getSymbol, getName, getInputStr, or getVarName --
    // see accessors

    int cmp(ELseverityLevel e) const;

    int          getLevel()    const;
    std::string  getSymbol()   const;  // example: "-e"
    std::string  getName()     const;  // example: "Error"
    std::string  getInputStr() const;  // example: "ERROR"
    std::string  getVarName()  const;  // example: "ELerror"

    friend std::ostream& operator<< (std::ostream& os,
                                     ELseverityLevel const sev);

  private:
    int myLevel;
  };  // ELseverityLevel

  inline
  constexpr
  ELseverityLevel::ELseverityLevel(ELsev_ const level)
  : myLevel{level}
  {}

  // ----------------------------------------------------------------------
  // Declare the globally available severity objects,
  // one generator function and one proxy per non-default ELsev_:
  // ----------------------------------------------------------------------

  inline ELseverityLevel constexpr ELzeroSeverityGen() {return ELseverityLevel::ELsev_zeroSeverity;}
  inline ELseverityLevel constexpr ELsuccessGen() {return ELseverityLevel::ELsev_success;}
  inline ELseverityLevel constexpr ELdebugGen() {return ELseverityLevel::ELsev_success;}
  inline ELseverityLevel constexpr ELinfoGen() {return ELseverityLevel::ELsev_info;}
  inline ELseverityLevel constexpr ELwarningGen() {return ELseverityLevel::ELsev_warning;}
  inline ELseverityLevel constexpr ELerrorGen() {return ELseverityLevel::ELsev_error;}
  inline ELseverityLevel constexpr ELunspecifiedGen() {return ELseverityLevel::ELsev_unspecified;}
  inline ELseverityLevel constexpr ELsevereGen() {return ELseverityLevel::ELsev_severe;}
  inline ELseverityLevel constexpr ELhighestSeverityGen() {return ELseverityLevel::ELsev_highestSeverity;}

  ELslProxy<ELzeroSeverityGen> constexpr ELzeroSeverity {};
  ELslProxy<ELdebugGen> constexpr ELdebug {};
  ELslProxy<ELsuccessGen> constexpr ELsuccess {};
  ELslProxy<ELinfoGen> constexpr ELinfo {};
  ELslProxy<ELwarningGen> constexpr ELwarning {};
  ELslProxy<ELerrorGen> constexpr ELerror {};
  ELslProxy<ELunspecifiedGen> constexpr ELunspecified {};
  ELslProxy<ELsevereGen> constexpr ELsevere {};
  ELslProxy<ELhighestSeverityGen> constexpr ELhighestSeverity {};


  // ----------------------------------------------------------------------
  // Comparators:
  // ----------------------------------------------------------------------

  extern bool operator==(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator!=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator< (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<=(ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator> (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>=(ELseverityLevel e1, ELseverityLevel e2);

} // end of namespace mf


// ----------------------------------------------------------------------

#define ELSEVERITYLEVEL_ICC
#include "messagefacility/Utilities/ELseverityLevel.icc"
#undef  ELSEVERITYLEVEL_ICC

// ----------------------------------------------------------------------

#endif /* messagefacility_Utilities_ELseverityLevel_h */

// Local variables:
// mode: c++
// End:
