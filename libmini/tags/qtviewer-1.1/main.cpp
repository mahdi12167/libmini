// (c) by Stefan Roettger

#include <mini/mini.h>

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal)
{
   QString msg;

   if (fatal==MINI_ERROR_NONFATAL) msg = "warning";
   else if (fatal==MINI_ERROR_MEM) msg = "insufficient memory";
   else if (fatal==MINI_ERROR_IO) msg = "io error";
   else msg = "fatal error";
   msg += " in <" + QString(file) + "> at line " + QString::number(line) + "!";

   QMessageBox::warning(0, "error", msg, QMessageBox::Ok);
}

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

   setminierrorhandler(errormsg);

   MainWindow window;

   window.resize(window.sizeHint());
   window.show();

   return(app.exec());
}
