#include <QtGui>
#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   if (!QGLFormat::hasOpenGL())
   {
      QMessageBox::warning(0, "NO OPENGL",
                           "Open GL is not available. Program abort.",
                           QMessageBox::Ok);

      return(1);
   }

   MainWindow w;
   w.show();

   return a.exec();
}
