

#include <QtGui>

#include "mvdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
{
  setupUi(this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect(btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );

  connect(btnSwitchChannel, SIGNAL( clicked() ), this, SLOT(switchChannel()) );

  connect(vsSeverity, SIGNAL( valueChanged(int) ), 
          this, SLOT(changeSeverity(int)) );

  connect(&lthread, SIGNAL(newMessage(const QString &)),
          this, SLOT(printMessage(const QString &)) );
  connect(&lthread, SIGNAL(sysMessage(const QString &)),
          this, SLOT(printSysMessage(const QString &)) );

  label_Partition->setText("Partition 0");

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
  //txtMessages->append("<hr width=95%><br>\n");
}

void msgViewerDlg::printSysMessage(const QString & s)
{
  QString msg = "<b><font color=\"#000080\">SYSTEM:" 
      + s + "</font></b><br>";
  txtMessages->append(msg);
}

void msgViewerDlg::exit()
{
  close();
}

void msgViewerDlg::changeSeverity(int sev)
{
  lthread.changeSeverity(sev);
}

void msgViewerDlg::switchChannel()
{
  bool ok;
  int partition = QInputDialog::getInteger(this, 
    "Partition", 
    "Please enter a partition number:",
    lthread.getPartition(),
    -1, 9, 1, &ok);

  if(ok)
  {
    if( lthread.switchPartition(partition) )
    {
      QString partStr = "Partition " + QString::number(partition);
      label_Partition->setText(partStr);
    }
  }

}

void msgViewerDlg::closeEvent(QCloseEvent *event)
{
  printSysMessage("Closing DDS connection.");
  lthread.stopListener();
  event->accept();
}
