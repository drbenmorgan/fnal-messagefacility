#ifndef MessageFacility_MessageService_ELDDSdest_h
#define MessageFacility_MessageService_ELDDSdest_h


// ----------------------------------------------------------------------
//
// ELDDSdest	is a subclass of ELdestination representing the remote
//		destination.
//
// 02/03/10 ql	Created file.
//
// ----------------------------------------------------------------------

#include "MessageService/interface/ELdestination.h"

#include "MessageLogger/interface/ELstring.h"
#include "MessageLogger/interface/ELextendedID.h"

#include "ParameterSet/interface/ParameterSet.h"

#include "ccpp_dds_dcps.h"
#include "DDSdest_bld/ccpp_MessageFacility.h"

#include "boost/shared_ptr.hpp"

using namespace DDS;
using namespace MessageFacility;

namespace mf {       


// ----------------------------------------------------------------------
// prerequisite classes:
// ----------------------------------------------------------------------

class ErrorObj;

namespace service {       

class ELdestControl;


// ----------------------------------------------------------------------
// ELDDSdest:
// ----------------------------------------------------------------------

class ELDDSdest : public ELdestination  {

  friend class ELdestControl;

public:

  // ---  Birth/death:
  //
  ELDDSdest(ParameterSet const & pset_);
  //ELDDSdest( Transporter & tp, bool emitAtStart = false );	// 6/11/07 mf
  //ELDDSdest( const ELstring & fileName, bool emitAtStart = false );
  virtual ~ELDDSdest();

  // ---  Methods invoked by the ELadministrator:
  //
public:
  virtual
  ELDDSdest *
  clone() const;
  // Used by attach() to put the destination on the ELadministrators list
                //-| There is a note in Design Notes about semantics
                //-| of copying a destination onto the list:  ofstream
                //-| ownership is passed to the new copy.

  virtual bool log( const mf::ErrorObj & msg );

  // ---  Methods invoked through the ELdestControl handle:
  //
protected:
    // trivial clearSummary(), wipe(), zero() from base class
    // trivial three summary(..) from base class

  // ---  Data affected by methods of specific ELdestControl handle:
  //
protected:
    // ELDDSdest uses the generic ELdestControl handle

  // ---  Internal Methods -- Users should not invoke these:
  //
protected:
  virtual void emit( const ELstring & s, bool nl=false );

  virtual void suppressTime();        virtual void includeTime();
  virtual void suppressModule();      virtual void includeModule();
  virtual void suppressSubroutine();  virtual void includeSubroutine();
  virtual void suppressText();        virtual void includeText();
  virtual void suppressContext();     virtual void includeContext();
  virtual void suppressSerial();      virtual void includeSerial();
  virtual void useFullContext();      virtual void useContext();
  virtual void separateTime();        virtual void attachTime();
  virtual void separateEpilogue();    virtual void attachEpilogue();

  virtual void summarization ( const ELstring & fullTitle
                             , const ELstring & sumLines );
			     
  //virtual void changeFile (std::ostream & os);
  //virtual void changeFile (const ELstring & filename);
  virtual void flush(); 				       


protected:
  // --- member data:
  ParameterSet       pset;
  int                charsOnLine;
  mf::ELextendedID   xid;

  bool wantTimestamp
  ,    wantModule
  ,    wantSubroutine
  ,    wantText
  ,    wantSomeContext
  ,    wantSerial
  ,    wantFullContext
  ,    wantTimeSeparate
  ,    wantEpilogueSeparate
  ,    preambleMode
  ;

  // --- Verboten method:
  //
  ELDDSdest & operator=( const ELDDSdest & orig );

protected:
  // make the copy c'tor protected to prevent explicitly copy operation
  ELDDSdest( const ELDDSdest & orig );

private:
  // ---  OpenSplice DDS related fields

  // Generic DDS entities
  DomainParticipantFactory_var    dpf;
  DomainParticipant_var           participant;
  Topic_var                       MFMessageTopic;
  Publisher_var                   MFPublisher;
  DataWriter_ptr                  parentWriter;

  // QosPolicy holders
  TopicQos                        reliable_topic_qos;
  TopicQos                        setting_topic_qos;
  PublisherQos                    pub_qos;
  DataWriterQos                   dw_qos;

  // DDS Identifiers
  DomainId_t                      domain;
  InstanceHandle_t                userHandle;
  ReturnCode_t                    status;

  // Type-specific DDS entities
  MFMessageTypeSupport_var        MFMessageTS;
  MFMessageDataWriter_var         talker;

  // Message pointer
  boost::shared_ptr<MFMessage>    DDSmsg;

  // others
  bool                            bMsgRegistered;
  bool                            bConnected;
  const char                    * partitionName;
  char                          * MFMessageTypeName;

  // DDS connection initialzation code
  bool createDDSConnection();
  void destroyDDSConnection();

private:
  // ---  ELdestinationFactory register
  //static DerivedRegister<ELDDSdest> reg;
};  // ELDDSdest


// ----------------------------------------------------------------------


}        // end of namespace service
}        // end of namespace mf


#endif // Extensions_MessageService_ELDDSdest_h
