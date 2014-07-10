// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include "viewer.h"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   if (!QGLFormat::hasOpenGL()) return(1);

   MyQtViewer viewer;
   viewer.show();

   return(app.exec());
}
