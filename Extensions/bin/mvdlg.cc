

#include <QtGui>

#include "boost/bind.hpp"
#include "mvdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
: BUFFER_SIZE ( 10000 )
, idx         ( 0 )
, mfmessages  ( std::vector<mf::MessageFacilityMsg>(BUFFER_SIZE) )
, hostmap     ( )
, appmap      ( )
//, dds         ( 0
//		      , boost::bind(&msgViewerDlg::onNewMsg, this, _1)
//              , boost::bind(&msgViewerDlg::onNewSysMsg, this, _1, _2) )
{
  setupUi(this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect(btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );

  connect(btnSwitchChannel, SIGNAL( clicked() ), this, SLOT(switchChannel()) );

  //connect(vsSeverity, SIGNAL( valueChanged(int) ),
  //        this, SLOT(changeSeverity(int)) );

  connect(&lthread, SIGNAL(newMessage(const QString &)),
          this, SLOT(printMessage(const QString &)) );
  connect(&lthread, SIGNAL(sysMessage(const QString &)),
          this, SLOT(printSysMessage(const QString &)) );

  label_Partition->setText("Partition 0");

  // Start lisenter thread
  //lthread.startListener();

  if(true) std::cout<<mfmessages.size()<<" empty\n\n";

}

void msgViewerDlg::onNewMsg(mf::MessageFacilityMsg const & mfmsg) {
	//txtMessages->append(QString(mfmsg.message().c_str()));
	sleep(5);
	txtMessages->append("hahaha");
	std::cout << "\nexit\n";
}

void msgViewerDlg::onNewSysMsg(mf::DDSReceiver::SysMsgCode, std::string const & msg) {
	txtMessages->append(QString(msg.c_str()));
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
  //lthread.changeSeverity(sev);
}

void msgViewerDlg::switchChannel()
{
#if 0
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
#endif

}

void msgViewerDlg::closeEvent(QCloseEvent *event)
{
	//dds.stop();
	event->accept();
#if 0
  printSysMessage("Closing DDS connection.");
  lthread.stopListener();
  event->accept();
#endif
}
