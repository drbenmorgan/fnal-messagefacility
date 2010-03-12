#include <QApplication>

#include "msgwidget.h"

int main( int argc, char ** argv )
{
  QApplication app(argc, argv);
  msgWidget *dialog = new msgWidget();

  dialog->show();
  return app.exec();
}
