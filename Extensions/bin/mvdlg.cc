

#include <QtGui>

#include "mvdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
: BUFFER_SIZE ( 10000 )
, idx         ( 0 )
, mfmessages  ( std::vector<mf::MessageFacilityMsg>(BUFFER_SIZE) )
, hostmap     ( )
, appmap      ( )
, qtdds       ( )
{
  setupUi(this);

  connect( btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect( btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );
  connect( btnSwitchChannel, SIGNAL( clicked() ), this, SLOT(switchChannel()) );

  connect( vsSeverity
         , SIGNAL( valueChanged(int) )
         , this
         , SLOT(changeSeverity(int)) );

  connect( &qtdds
         , SIGNAL(newMessage(mf::MessageFacilityMsg const & ))
         , this
         , SLOT(onNewMsg(mf::MessageFacilityMsg const & )) );

  connect( &qtdds
         , SIGNAL(newSysMessage(mf::DDSReceiver::SysMsgCode, std::string const & ))
         , this
         , SLOT(onNewSysMsg(mf::DDSReceiver::SysMsgCode, std::string const & )) );

  label_Partition->setText("Partition 0");


  //if(true) std::cout<<mfmessages.size()<<" empty\n\n";

}

void msgViewerDlg::onNewMsg(mf::MessageFacilityMsg const & mfmsg) {
	txtMessages->append(QString(mfmsg.message().c_str()));
}

void msgViewerDlg::onNewSysMsg(mf::DDSReceiver::SysMsgCode syscode, std::string const & msg) {
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
  qtdds.setSeverityThreshold(mf::QtDDSReceiver::getSeverityCode(sev));
}

void msgViewerDlg::switchChannel()
{
  bool ok;
  int partition = QInputDialog::getInteger(this, 
    "Partition", 
    "Please enter a partition number:",
    qtdds.getPartition(),
    -1, 9, 1, &ok);

  if(ok)
  {
    qtdds.switchPartition(partition);

    QString partStr = "Partition " + QString::number(qtdds.getPartition());
    label_Partition->setText(partStr);
  }

}

void msgViewerDlg::closeEvent(QCloseEvent *event)
{
	qtdds.stop();
	event->accept();
}
