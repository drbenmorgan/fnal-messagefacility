
#include "mvdlg.h"

#include <QtGui>

#include <sstream>

msgViewerDlg::msgViewerDlg(int part, QDialog * parent)
: BUFFER_SIZE ( 1000 )  // size of the circular buffer for received messages
, idx         ( 0 )      // index of the head position in the circular buffer
, mfmessages  ( std::vector<mf::MessageFacilityMsg>(BUFFER_SIZE) )
, nMsgs       ( 0 )
, nDisplayMsgs( 0 )
, hostFilter  ( "##DEADBEAF##" )
, appFilter   ( "##DEADBEAF##" )
, catFilter   ( "##DEADBEAF##" )
, sevThresh   ( mf::QtDDSReceiver::DEBUG )
, hostmap     ( )
, appmap      ( )
, catmap      ( )
, qtdds       ( part )      // partition 0
, msgsPerPage ( 5 )
, currentPage ( 0 )
, simpleRender( true )
, timer       ( this )
{
  setupUi(this);

  connect( btnPause, SIGNAL( clicked() ), this, SLOT(pause()) );
  connect( btnExit,  SIGNAL( clicked() ), this, SLOT(exit())  );
  connect( btnSwitchChannel, SIGNAL( clicked() ), this, SLOT(switchChannel()) );

  connect( btnRMode, SIGNAL( clicked() ), this, SLOT(renderMode()) );

  connect( btnFilter, SIGNAL( clicked() ), this, SLOT( setFilter() ) );
  connect( btnReset,  SIGNAL( clicked() ), this, SLOT( resetFilter() ) );

  connect( btnClearHost, SIGNAL( clicked() ), this, SLOT( clearHostSelection() ) );
  connect( btnClearApp,  SIGNAL( clicked() ), this, SLOT( clearAppSelection() ) );
  connect( btnClearCat,  SIGNAL( clicked() ), this, SLOT( clearCatSelection() ) );

  connect( vsSeverity
         , SIGNAL( valueChanged(int) )
         , this
         , SLOT(changeSeverity(int)) );

  connect( &qtdds
         , SIGNAL(newMessage(mf::MessageFacilityMsg const & ))
         , this
         , SLOT(onNewMsg(mf::MessageFacilityMsg const & )) );

  connect( &qtdds
         , SIGNAL(newSysMessage(mf::QtDDSReceiver::SysMsgCode, QString const & ))
         , this
         , SLOT(onNewSysMsg(mf::QtDDSReceiver::SysMsgCode, QString const & )) );

  connect( &timer, SIGNAL(timeout()), this, SLOT(trimDisplayMsgs()));

  QString partStr = "Partition " + QString::number(qtdds.getPartition());
  btnSwitchChannel->setText(partStr);

  if(simpleRender)	  btnRMode -> setChecked(true);
  else 				  btnRMode -> setChecked(false);

  btnRMode ->setEnabled(false);

  // start the purge timer
  //timer.start(10000);
}

void msgViewerDlg::onNewMsg(mf::MessageFacilityMsg const & mfmsg) {

	bool updateApp = false;
	bool updateCat = false;
	bool updateHost = false;

	// Check if the buffer is full
	if(!mfmessages[idx].empty()) {
		std::string host = mfmessages[idx].hostname();
		host = host.substr(0, host.find_first_of('.'));

		updateApp = deleteFromMap(appmap, mfmessages[idx].application());
		updateCat = deleteFromMap(catmap, mfmessages[idx].category());
		updateHost = deleteFromMap(hostmap, host);
	}

	// push the message into buffer
	mfmessages[idx] = mfmsg;

	std::string host = mfmsg.hostname();
	std::string cat  = mfmsg.category();
	std::string app  = mfmsg.application();

	host = host.substr(0, host.find_first_of('.'));

	bool resetFilter = false;

	if(updateMap(appmap, app) || updateApp) {
		updateList(lwApplication, appmap);
	}

	if(updateMap(catmap, cat) || updateCat) {
		updateList(lwCategory, catmap);
	}

	if(updateMap(hostmap, host) || updateHost) {
		updateList(lwHost, hostmap);
	}

	bool hostAny = ( hostFilter == "##DEADBEAF##" );
	bool appAny  = ( appFilter  == "##DEADBEAF##" );
	bool catAny  = ( catFilter  == "##DEADBEAF##" );

	bool hostMatch = ( hostFilter == host );
	bool appMatch  = ( appFilter  == app  );
	bool catMatch  = ( catFilter  == cat  );

	// Check to display the message
	if( (hostAny || hostMatch) && (appAny || appMatch) && (catAny || catMatch) ) {
#if 0
		if( currentPage == (nDisplayMsgs + msgsPerPage-1) / msgsPerPage) {
			// Means its in the latest page
			if( nDisplayMsgs % msgsPerPage == 0 ) {
				// Move to next page
				txtMessages->clear();
				++currentPage;
			}
			// display the message
			displayMsg(mfmsg);
		}
		// no matter what, evolves the nDisplayMsgs
		++nDisplayMsgs;
#endif

		displayMsg(mfmsg);
	}

	// finally revolve the idx forward by one
	idx = (++idx) % BUFFER_SIZE;
}

void msgViewerDlg::onNewSysMsg(mf::QtDDSReceiver::SysMsgCode syscode, QString const & msg) {

	if(syscode == mf::QtDDSReceiver::NEW_MESSAGE) {
		++nMsgs;
		lcdMsgs->display( nMsgs );
	}
	else {
		QString qtmsg = "SYSTEM: " + msg + "\n";
		txtMessages->setTextColor(QColor(0, 0, 128));
		txtMessages->append(qtmsg);
	}
}

// display a single message
void msgViewerDlg::displayMsg(mf::MessageFacilityMsg const & mfmsg) {

	QString msg = generateMsgStr(mfmsg);

	if(!msg.isEmpty()) {
		setMsgColor(mf::QtDDSReceiver::getSeverityCode(mfmsg.severity()));
		txtMessages->append( msg );
		++nDisplayMsgs;
		trimDisplayMsgs();
	}
}

// display a list of message
void msgViewerDlg::displayMsg(std::list<int> const & l) {

	std::string str;
	std::list<int>::const_iterator it;

	//if(l.size()>msgsPerPage) it = l.end()-msgsPerPage;
	//else					 it = l.begin();

	it = l.begin();

	while(it!=l.end()) {
		QString msg = generateMsgStr(mfmessages[*it]);
		if(!msg.isEmpty()) {
			setMsgColor(mf::QtDDSReceiver::getSeverityCode(mfmessages[*it].severity()));
			txtMessages->append( msg );
			++nDisplayMsgs;
		}

		++it;
	}

	trimDisplayMsgs();

	//nDisplayMsgs = l.size();
	//currentPage = (nDisplayMsgs + msgsPerPage-1) / msgsPerPage;
}

// display all messages in buffer
void msgViewerDlg::displayMsg() {

	int i = idx;

	do {
		if(!mfmessages[i].empty()) {
			QString msg = generateMsgStr(mfmessages[i]);
			if(!msg.isEmpty()) {
				setMsgColor(mf::QtDDSReceiver::getSeverityCode(mfmessages[i].severity()));
				txtMessages->append( msg );
				++nDisplayMsgs;
			}
		}

		i = (++i) % BUFFER_SIZE;
	} while (i != idx);

	trimDisplayMsgs();
}

void msgViewerDlg::trimDisplayMsgs() {

	if(nDisplayMsgs > BUFFER_SIZE) {

		//int nBlocks = simpleRender ? 9 : 1;
		int nBlocks = 8 * (nDisplayMsgs - BUFFER_SIZE);

		// remember current cursor
		QScrollBar * sb = txtMessages->verticalScrollBar();
		int v = sb->value();

		// trim the leading message
		txtMessages->moveCursor( QTextCursor::Start, QTextCursor::MoveAnchor );
		for(int i=0; i<nBlocks; ++i)
			txtMessages->moveCursor( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
		txtMessages->textCursor().removeSelectedText();

		// Restore cursor
		sb->setValue(v);

		nDisplayMsgs = BUFFER_SIZE;
	}
}

void msgViewerDlg::setMsgColor(mf::QtDDSReceiver::SeverityCode sev) {

	QColor qc;

	if(sev==mf::QtDDSReceiver::ERROR)         qc.setRgb(255, 0, 0);
    else if(sev==mf::QtDDSReceiver::WARNING)  qc.setRgb(224, 128, 0);
    else if(sev==mf::QtDDSReceiver::INFO)     qc.setRgb(0, 128, 0);
    else                                      qc.setRgb(80, 80, 80);

	txtMessages->setTextColor(qc);
}


QString msgViewerDlg::generateMsgStr(mf::MessageFacilityMsg const & mfmsg) {

    mf::QtDDSReceiver::SeverityCode sevid =
    		mf::QtDDSReceiver::getSeverityCode(mfmsg.severity());

    if(sevid < sevThresh)
    	return QString();

    std::ostringstream ss;

    if(simpleRender) {
		ss << mfmsg.severity() << " / " << mfmsg.category() << "\n"
		   << mfmsg.timestr()  << "\n"
		   << mfmsg.hostname() << " (" << mfmsg.hostaddr() << ")" << "\n"
		   << mfmsg.process()  << " (" << mfmsg.pid()      << ")" << "\n"
		   << mfmsg.file()     << " (" << mfmsg.line() << ")" << "\n"
		   << mfmsg.application() << " / "
		   << mfmsg.module()      << " / "
		   << mfmsg.context()     << "\n"
		   << mfmsg.message()     << "\n";
    }
    else {
		ss << "<font ";

		if(sevid==mf::QtDDSReceiver::ERROR)         ss << "color='#FF0000'>";
		else if(sevid==mf::QtDDSReceiver::WARNING)  ss << "color='#E08000'>";
		else if(sevid==mf::QtDDSReceiver::INFO)     ss << "color='#008000'>";
		else                                      ss << "color='#505050'>";

		ss << "<b>" << mfmsg.severity() << " / " << mfmsg.category() << "</b><br>";
		ss << mfmsg.timestr()  << "<br>";
		ss << mfmsg.hostname() << " (" << mfmsg.hostaddr() << ")" << "<br>";
		ss << mfmsg.process()  << " (" << mfmsg.pid()      << ")" << "<br>";
		ss << mfmsg.file()     << " (" << mfmsg.line() << ")" << "<br>";
		ss << mfmsg.application() << " / "
		   << mfmsg.module()      << " / "
		   << mfmsg.context()     << "<br>";
		ss << mfmsg.message()     << "<br>";
		ss << "</font><br>";
    }

#if 0
    ss << "<font face=\"New Courier\" ";

    if(sevid==mf::DDSReceiver::ERROR)         ss << "color='#FF0000'>";
    else if(sevid==mf::DDSReceiver::WARNING)  ss << "color='#E08000'>";
    else if(sevid==mf::DDSReceiver::INFO)     ss << "color='#008000'>";
    else                                      ss << "color='#505050'>";

    ss << "<table border=0 width=95%>";

    ss << "<tr><td width=18%>severity:</td><td><b>"
       << mfmsg.severity()
       << "</b></td></tr>";

    ss << "<tr><td>category:</td><td><b>"
       << mfmsg.category()
       << "</b></td></tr>";

    ss << "<tr><td>date:</td><td>"
       << mfmsg.timestr()
       << "</td></tr>";

    ss << "<tr><td>host:</td><td>"
       << mfmsg.hostname() << " (" << mfmsg.hostaddr() << ")"
       << "</td></tr>";

    ss << "<tr><td>process:</td><td>"
       << mfmsg.process()  << " (" << mfmsg.pid()      << ")"
       << "<td></tr>";

    ss << "<tr><td>context:</td><td>"
       << mfmsg.application() << " / "
       << mfmsg.module()      << " / "
       << mfmsg.context()
       << "</td></tr>";

    ss << "<tr><td>file:</td><td>"
       << mfmsg.file()  << " (" << mfmsg.line() << ")"
       << "</td></tr>";

    ss << "<tr><td>message:</td><td><i>"
       << mfmsg.message()
       << "</i></td></tr>";

    ss << "</table></font><br>";
#endif


    return QString(ss.str().c_str());
}

bool msgViewerDlg::updateMap(std::map<std::string, std::list<int> > & map, std::string const & key) {

	std::map<std::string, std::list<int> >::iterator it = map.find(key);

	if(it != map.end()) {
		// the key already exists
		it->second.push_back(idx);
		return false;
	} else {
		// its a new key
		std::list<int> idxlist;
		idxlist.push_back(idx);
		map.insert( std::make_pair( key, idxlist ) );
		return true;
	}
}

bool msgViewerDlg::deleteFromMap(std::map<std::string, std::list<int> > & map, std::string const & key) {
	std::map<std::string, std::list<int> >::iterator it = map.find(key);
	if(it != map.end()) {
		it->second.pop_front();
		if(it->second.empty()) {
			map.erase(it);
			return true;
		}
	}
	return false;
}

bool msgViewerDlg::updateList(QListWidget * lw, std::map<std::string, std::list<int> > & map) {

	bool nonSelectedBefore = ( lw->currentRow() == -1 );
	bool nonSelectedAfter = true;

	QString item = nonSelectedBefore ? "" : lw->currentItem()->text();

	lw -> clear();
	int row = 0;
	std::map<std::string, std::list<int> >::const_iterator it = map.begin();

	while(it!=map.end()) {
		lw -> addItem(it->first.c_str());

		if(!nonSelectedBefore && nonSelectedAfter) {
			if(item == it->first.c_str()) {
				lw ->setCurrentRow(row);
				nonSelectedAfter = false;
			}
		}

		++row;
		++it;
	}

	if(!nonSelectedBefore && nonSelectedAfter)  return true;

	return false;
}

std::list<int> msgViewerDlg::findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2
		, std::list<int> const & l3 )
{
	bool e1 = l1.empty();
	bool e2 = l2.empty();
	bool e3 = l3.empty();

	if( !e1 &&  e2 &&  e3 )
		return l1;

	if(  e1 && !e2 &&  e3 )
		return l2;

	if(  e1 &&  e2 && !e3 )
		return l3;

	if( !e1 && !e2 &&  e3 )
		return findCommonInLists(l1, l2);

	if( !e1 &&  e2 && !e3 )
		return findCommonInLists(l1, l3);

	if(  e1 && !e2 && !e3 )
		return findCommonInLists(l2, l3);

	// start finding commons
	std::list<int>::const_iterator it1 = l1.begin();
	std::list<int>::const_iterator it2 = l2.begin();
	std::list<int>::const_iterator it3 = l3.begin();

	std::list<int> r;

	while( (it1!=l1.end()) && (it2 != l2.end()) && (it3!=l3.end())) {

		if( (*it1 == *it2) && (*it1 == *it3) ) {
			r.push_back(*it1);
			++it1;
			++it2;
			++it3;
		}
		else
			++ ( (*it1 < *it2)
					? ( (*it1 < *it3) ? it1 : it3 )
					: ( (*it2 < *it3) ? it2 : it3 ) );
	}

	return r;
}

std::list<int> msgViewerDlg::findCommonInLists(
		  std::list<int> const & l1
		, std::list<int> const & l2 )
{
	std::list<int>::const_iterator it1 = l1.begin();
	std::list<int>::const_iterator it2 = l2.begin();

	std::list<int> r;

	while( (it1!=l1.end()) && (it2 != l2.end()) )
	{
		if(*it1 == *it2)
		{
			r.push_back(*it1);
			++it1;
			++it2;
		}
		else
			++ ( (*it1 < *it2) ? it1 : it2 );
	}

	return r;
}

void msgViewerDlg::setFilter() {

	hostFilter = (lwHost->currentItem()) ? lwHost->currentItem()->text().toStdString() : "##DEADBEAF##";
	appFilter = (lwApplication->currentItem()) ? lwApplication->currentItem()->text().toStdString() : "##DEADBEAF##";
	catFilter = (lwCategory->currentItem()) ? lwCategory->currentItem()->text().toStdString() : "##DEADBEAF##";

	if(hostFilter == "##DEADBEAF##" && appFilter  == "##DEADBEAF##" && catFilter  == "##DEADBEAF##" ) {
		resetFilter();
		return;
	}

	std::list<int> result;
	std::list<int> empty;

	std::map<std::string, std::list<int> >::const_iterator it;

	result = findCommonInLists(
			  ( (it = hostmap.find(hostFilter)) == hostmap.end() ) ? empty : it->second
			, ( (it = appmap.find(appFilter)) == appmap.end() ) ? empty : it->second
			, ( (it = catmap.find(catFilter)) == catmap.end() ) ? empty : it->second );

	// Update the view
	txtMessages->clear();
	nDisplayMsgs = 0;
	displayMsg(result);
}

void msgViewerDlg::resetFilter() {

	hostFilter = "##DEADBEAF##";
	appFilter = "##DEADBEAF##";
	catFilter = "##DEADBEAF##";

	lwHost->setCurrentRow(-1);
	lwApplication->setCurrentRow(-1);
	lwCategory->setCurrentRow(-1);

	// Update the view
	txtMessages->clear();
	nDisplayMsgs = 0;
	displayMsg();
}

void msgViewerDlg::clearHostSelection() { lwHost->setCurrentRow(-1); }
void msgViewerDlg::clearAppSelection()  { lwApplication->setCurrentRow(-1); }
void msgViewerDlg::clearCatSelection()  { lwCategory->setCurrentRow(-1); }

void msgViewerDlg::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}

void msgViewerDlg::exit()
{
  close();
}

void msgViewerDlg::changeSeverity(int sev)
{
	sevThresh = mf::QtDDSReceiver::getSeverityCode(sev);
	setFilter();
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
    btnSwitchChannel->setText(partStr);
  }

}

void msgViewerDlg::renderMode()
{
	simpleRender = !simpleRender;

	if(simpleRender) {
		btnRMode -> setChecked(true);
		txtMessages->setPlainText(txtMessages->toPlainText());
	}
	else {
		btnRMode -> setChecked(false);
		setFilter();
	}
}

void msgViewerDlg::closeEvent(QCloseEvent *event)
{
	qtdds.stop();
	event->accept();
}
