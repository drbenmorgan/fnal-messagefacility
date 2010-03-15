#include <QApplication>

#include "mvdlg.h"

int main( int argc, char ** argv )
{
  QApplication app(argc, argv);
  msgViewerDlg *dialog = new msgViewerDlg();

  dialog->show();
  return app.exec();

  return 0;
}
