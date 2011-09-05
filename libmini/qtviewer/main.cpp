#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   if (!QGLFormat::hasOpenGL())
   {
      QMessageBox::warning(0, "NO OPENGL",
                           "Open GL is not available. Program abort.",
                           QMessageBox::Ok);

      return(1);
   }

   MainWindow window;

   window.resize(window.sizeHint());
   window.show();

   return(app.exec());
}
