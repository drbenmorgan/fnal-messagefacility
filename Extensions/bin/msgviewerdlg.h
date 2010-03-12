#ifndef MSGVIEWERDLG_H
#define MSGVIEWERDLG_H

#include "Extensions/bin/msgviewerdlgui.h"

class msgViewerDlg : public QDialog, private Ui::MsgViewerDlg
{
  Q_OBJECT

public:
  msgViewerDlg( QDialog *parent = 0 );

public slots:
  void pause();
};

#endif
