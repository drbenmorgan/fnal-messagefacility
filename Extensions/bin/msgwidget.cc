#include <QtGui>

#include "msgwidget.h"

msgWidget::msgWidget(QWidget * parent)
{
  setupUi((QDialog*)this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()));
}

void msgWidget::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}
