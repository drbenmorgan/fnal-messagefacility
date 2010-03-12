#include <QtGui>

#include "msgviewerdlg.h"

msgViewerDlg::msgViewerDlg(QDialog * parent)
{
  setupUi(this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()));
}

void msgViewerDlg::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}
