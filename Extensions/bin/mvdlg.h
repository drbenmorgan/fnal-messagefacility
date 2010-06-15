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
  void onNewMsg(mf::MessageFacilityMsg const & mfmsg);
  void onNewSysMsg(mf::QtDDSReceiver::SysMsgCode, std::string const & msg);

  void pause();
  void exit();
  void changeSeverity(int sev);
  void printMessage(QString const & s);
  void printSysMessage(QString const & s);
  void switchChannel();

protected:
  void closeEvent(QCloseEvent *event);

private:

  const int BUFFER_SIZE;

  // circular buffer head pointer
  int idx;

  // circular message buffer
  std::vector<mf::MessageFacilityMsg> mfmessages;

  // map from a hostname to a list of message indices
  std::map<std::string, std::list<int> > hostmap;

  // map from an appliation name to a list of message indices
  std::map<std::string, std::list<int> > appmap;

  // DDSReceiver for Qt
  mf::QtDDSReceiver qtdds;

};

#endif
