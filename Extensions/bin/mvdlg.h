#ifndef MSGVIEWERDLG_H
#define MSGVIEWERDLG_H

#include "Extensions/bin/msgviewerdlgui.h"
#include "Extensions/interface/QtDDSReceiver.h"

#include <string>
#include <vector>
#include <map>
#include <list>


class msgViewerDlg : public QDialog, private Ui::MsgViewerDlg
{
  Q_OBJECT

public:
  msgViewerDlg( QDialog *parent = 0 );


public slots:

  void pause();
  void exit();
  void changeSeverity(int sev);
  void switchChannel();

protected:
  void closeEvent(QCloseEvent *event);

private slots:

  void onNewMsg(mf::MessageFacilityMsg const & mfmsg);
  void onNewSysMsg(mf::QtDDSReceiver::SysMsgCode, std::string const & msg);

  void setFilter();
  void resetFilter();

  void renderMode();

  void clearHostSelection();
  void clearAppSelection();
  void clearCatSelection();

private:

  std::list<int> findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2
		, std::list<int> const & l3 );

  std::list<int> findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2 );

  // Display a single message
  void displayMsg(mf::MessageFacilityMsg const & mfmsg);
  // Display a list of messages by the list of message indices
  void displayMsg(std::list<int> const & l);
  // Display all messages stored in the buffer
  void displayMsg();

  std::string generateMsgStr(mf::MessageFacilityMsg const & mfmsg);

  // Pop the first element from the list pointed by the key. Returns true
  // if the list becomes empty after the deletion
  bool deleteFromMap(std::map<std::string, std::list<int> > &, std::string const &);

  // Update the map. Returns true if provided key is a new key in the map
  bool updateMap(std::map<std::string, std::list<int> > &, std::string const &);

  // Update the list. Returns true if there's a change in the selection
  // before and after the update. e.g., the selected entry has been deleted
  // during the process of updateMap(), therefore it returns a false.
  bool updateList(QListWidget *, std::map<std::string, std::list<int> > &);


private:

  // # of received messages
  int nMsgs;

  const int msgsPerPage;
  int nDisplayMsgs;
  int currentPage;

  bool simpleRender;

  // filter strings for hosts, applications, and categories
  // the value "##DEADBEAF##" indicates no filter condition has
  // been set
  std::string hostFilter;
  std::string appFilter;
  std::string catFilter;

  // buffer size
  const int BUFFER_SIZE;

  // circular buffer head pointer
  int idx;

  // circular message buffer
  std::vector<mf::MessageFacilityMsg> mfmessages;

  // map from a hostname to a list of message indices
  std::map<std::string, std::list<int> > hostmap;

  // map from an application name to a list of message indices
  std::map<std::string, std::list<int> > appmap;

  // map from a category name to a list of message indices
  std::map<std::string, std::list<int> > catmap;

  // DDSReceiver for Qt
  mf::QtDDSReceiver qtdds;

};

#endif
