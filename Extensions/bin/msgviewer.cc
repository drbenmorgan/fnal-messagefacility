#include <QApplication>

#include "mvdlg.h"

int main( int argc, char ** argv )
{
  QApplication app(argc, argv);

  int p = 0;

  if(argc >1) {
	  for(int i = 1; i<argc; ++i) {
		  if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--partition")) {
			  if(i < argc-1) {
				  sscanf(argv[i+1], "%d", &p);
				  break;
			  }
		  }
	  }
  }

  msgViewerDlg *dialog = new msgViewerDlg(p);

  dialog->show();
  return app.exec();

  return 0;
}
