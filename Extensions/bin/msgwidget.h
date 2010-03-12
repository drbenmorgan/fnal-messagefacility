#ifndef MSGWIDGET_H
#define MSGWIDGET_H

#include "Extensions/bin/msgviewerdlg.h"

class msgWidget : public QWidget, private Ui::MsgViewerDlg
{
  Q_OBJECT

public:
  msgWidget( QWidget *parent = 0 );

public slots:
  void pause();
};

#endif
