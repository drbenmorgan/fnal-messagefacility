

#include <QtGui>

#include "mvdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
{
  setupUi(this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect(btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );

  connect(vsSeverity, SIGNAL( valueChanged(int) ), 
          this, SLOT(changeSeverity(int)) );

  connect(&lthread, SIGNAL(newMessage(const QString &)),
          this, SLOT(printMessage(const QString &)) );
  connect(&lthread, SIGNAL(sysMessage(const QString &)),
          this, SLOT(printSysMessage(const QString &)) );

  // Start lisenter thread
  lthread.startListener();

}

void msgViewerDlg::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}

void msgViewerDlg::printMessage(const QString & s)
{
  txtMessages->append(s);
}

void msgViewerDlg::printSysMessage(const QString & s)
{
  QString msg = "<b><font color=\"#000080\">SYSTEM:" + s + "</font></b><br></br>";
  txtMessages->append(msg);
}

void msgViewerDlg::exit()
{
  printSysMessage("Closing DDS connection.");
  lthread.stopListener();
  close();
}

void msgViewerDlg::changeSeverity(int sev)
{
  lthread.changeSeverity(sev);
}
