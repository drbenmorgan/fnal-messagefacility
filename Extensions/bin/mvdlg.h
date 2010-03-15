#ifndef MSGVIEWERDLG_H
#define MSGVIEWERDLG_H

#include "Extensions/bin/msgviewerdlgui.h"

#include "mvlistener.h"

#include <string>


class msgViewerDlg : public QDialog, private Ui::MsgViewerDlg
{
  Q_OBJECT

public:
  msgViewerDlg( QDialog *parent = 0 );

public slots:
  void pause();
  void exit();
  void changeSeverity(int sev);
  void printMessage(QString const & s);
  void printSysMessage(QString const & s);

private:
  ListenerThread lthread;

};

#endif
