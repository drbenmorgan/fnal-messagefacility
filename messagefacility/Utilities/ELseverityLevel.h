#ifndef messagefacility_Auxiliaries_ELseverityLevel_h
#define messagefacility_Auxiliaries_ELseverityLevel_h


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
// 30-Jun-1998 mf       Created file.
// 26-Aug-1998 WEB      Made ELseverityLevel object less weighty.
// 16-Jun-1999 mf       Added constructor from string.
// 23-Jun-1999 mf       Additional ELsev_noValueAssigned to allow constructor
//                      from string to give ELunspecified when not found, while
//                      still allowing finding zero severity.
// 23-Jun-1999 mf       Corrections for subtleties in initialization of
//                      global symbols:
//                              Added ELsevLevGlobals array
//                              Changed extern consts of SLseverityLevels into
//                                const ELseverityLevel & 's
//                              Inserted class ELinitializeGlobalSeverityObjects
//                                in place of the
//                                initializeGlobalSeverityObjects() function.
//                              Changed globalSeverityObjectsGuarantor to an
//                                ELinitializeGlobalSeverityObjects instance.
// 30-Jun-1999 mf       Modifications to eliminate problems with order of
//                      globals initializations:
//                              translate(), getInputStr(), getVarName()
// 12-Jun-2000 web      Final fix to global static initialization problem
// 14-Jun-2000 web      Declare classes before granting friendship.
// 27-Jun-2000 web      Fix order-of-static-destruction problem
//
// ----------------------------------------------------------------------


#include <string>

namespace mf {


  // ----------------------------------------------------------------------
  // Forward declaration:
  // ----------------------------------------------------------------------

  class ELseverityLevel;


  // ----------------------------------------------------------------------
  // Synonym for type of ELseverityLevel-generating function:
  // ----------------------------------------------------------------------

  typedef  ELseverityLevel ELslGen();


  // ----------------------------------------------------------------------
  // ELslProxy class template:
  // ----------------------------------------------------------------------

  template<ELslGen ELgen>
  struct ELslProxy  {

    // --- conversion:
    //
    operator ELseverityLevel() const;

    // --- forwarding:
    //
    int         getLevel()    const;
    std::string getSymbol()   const;
    std::string getName()     const;
    std::string getInputStr() const;
    std::string getVarName()  const;

  };  // ELslProxy<ELslGen>

  // ----------------------------------------------------------------------


  // ----------------------------------------------------------------------
  // ELseverityLevel:
  // ----------------------------------------------------------------------

  class ELseverityLevel  {
  public:

    // ---  One ELseverityLevel is globally instantiated (see below)
    // ---  for each of the following levels:
    //
    enum ELsev_ {
      ELsev_noValueAssigned = 0  // default returned by map when not found
        , ELsev_zeroSeverity         // threshold use only
        , ELsev_incidental           // flash this on a screen
        , ELsev_success              // report reaching a milestone
        , ELsev_info                 // information
        , ELsev_warning              // warning
        , ELsev_warning2             // more serious warning
        , ELsev_error                // error detected
        , ELsev_error2               // more serious error
        , ELsev_next                 // advise to skip to next event
        , ELsev_unspecified          // severity was not specified
        , ELsev_severe               // future results are suspect
        , ELsev_severe2              // more severe
        , ELsev_abort                // suggest aborting
        , ELsev_fatal                // strongly suggest aborting!
        , ELsev_highestSeverity      // threshold use only
        // -----
        , nLevels                    // how many levels?
        };  // ELsev_

    constexpr ELseverityLevel(ELsev_ lev = ELsev_unspecified);
    ELseverityLevel (std::string const& str);
    // str may match getSymbol, getName, getInputStr,
    // or getVarName -- see accessors

    // -----  Comparator:
    //
    int cmp(ELseverityLevel e) const;

    // -----  Accessors:
    //
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
  inline ELseverityLevel constexpr ELincidentalGen() {return ELseverityLevel::ELsev_incidental;}
  inline ELseverityLevel constexpr ELsuccessGen() {return ELseverityLevel::ELsev_success;}
  inline ELseverityLevel constexpr ELinfoGen() {return ELseverityLevel::ELsev_info;}
  inline ELseverityLevel constexpr ELwarningGen() {return ELseverityLevel::ELsev_warning;}
  inline ELseverityLevel constexpr ELwarning2Gen() {return ELseverityLevel::ELsev_warning2;}
  inline ELseverityLevel constexpr ELerrorGen() {return ELseverityLevel::ELsev_error;}
  inline ELseverityLevel constexpr ELerror2Gen() {return ELseverityLevel::ELsev_error2;}
  inline ELseverityLevel constexpr ELnextEventGen() {return ELseverityLevel::ELsev_next;}
  inline ELseverityLevel constexpr ELunspecifiedGen() {return ELseverityLevel::ELsev_unspecified;}
  inline ELseverityLevel constexpr ELsevereGen() {return ELseverityLevel::ELsev_severe;}
  inline ELseverityLevel constexpr ELsevere2Gen() {return ELseverityLevel::ELsev_severe2;}
  inline ELseverityLevel constexpr ELabortGen() {return ELseverityLevel::ELsev_abort;}
  inline ELseverityLevel constexpr ELfatalGen() {return ELseverityLevel::ELsev_fatal;}
  inline ELseverityLevel constexpr ELhighestSeverityGen() {return ELseverityLevel::ELsev_highestSeverity;}

  ELslProxy< ELzeroSeverityGen    > constexpr ELzeroSeverity {};
  ELslProxy< ELincidentalGen      > constexpr ELincidental {};
  ELslProxy< ELsuccessGen         > constexpr ELsuccess {};
  ELslProxy< ELinfoGen            > constexpr ELinfo {};
  ELslProxy< ELwarningGen         > constexpr ELwarning {};
  ELslProxy< ELwarning2Gen        > constexpr ELwarning2 {};
  ELslProxy< ELerrorGen           > constexpr ELerror {};
  ELslProxy< ELerror2Gen          > constexpr ELerror2 {};
  ELslProxy< ELnextEventGen       > constexpr ELnextEvent {};
  ELslProxy< ELunspecifiedGen     > constexpr ELunspecified {};
  ELslProxy< ELsevereGen          > constexpr ELsevere {};
  ELslProxy< ELsevere2Gen         > constexpr ELsevere2 {};
  ELslProxy< ELabortGen           > constexpr ELabort {};
  ELslProxy< ELfatalGen           > constexpr ELfatal {};
  ELslProxy< ELhighestSeverityGen > constexpr ELhighestSeverity {};


  // ----------------------------------------------------------------------
  // Comparators:
  // ----------------------------------------------------------------------

  extern bool operator== (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator!= (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<  (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator<= (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>  (ELseverityLevel e1, ELseverityLevel e2);
  extern bool operator>= (ELseverityLevel e1, ELseverityLevel e2);

} // end of namespace mf


// ----------------------------------------------------------------------

#define ELSEVERITYLEVEL_ICC
#include "messagefacility/Utilities/ELseverityLevel.icc"
#undef  ELSEVERITYLEVEL_ICC

// ----------------------------------------------------------------------

#endif /* messagefacility_Auxiliaries_ELseverityLevel_h */

// Local variables:
// mode: c++
// End:
