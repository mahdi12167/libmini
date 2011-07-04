#include <QtGui>
#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include "mainwindow.h"

#define width 1024
#define height 768

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   if (!QGLFormat::hasOpenGL())
   {
      QMessageBox::warning(0, "NO OPENGL", "Open GL is not available. Program abort.", QMessageBox::Ok);
      return 1;
   }

   // there are five parameters:
   // <binary name> <baseurl> <baseid> <basepath1> <basepath2>
   if (argc < 5)
   {
      QMessageBox::warning(0, "INVALID PARAMETERS", "Map data path is not specified. Program abort.", QMessageBox::Ok);
      return 1;
   }

   MainWindow w;
   w.show();
   w.resize(width, height);

   return a.exec();
}
