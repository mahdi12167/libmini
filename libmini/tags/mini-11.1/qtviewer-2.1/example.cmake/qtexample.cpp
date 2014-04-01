// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_core.h>
#include <mini/mini_generic.h>

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal,const char *msg)
{
   ministring m;

   if (fatal==MINI_ERROR_NONFATAL) m = "warning";
   else if (fatal==MINI_ERROR_MEM) m = "insufficient memory";
   else if (fatal==MINI_ERROR_IO) m = "io error";
   else m = "fatal error";
   m += ministring(" in <") + file + "> at line " + line + "!";
   if (msg!=NULL) m += ministring(" description=\"") + msg + "\"";

   fprintf(stderr, "%s\n", m.c_str());

   MINILOG(m);

   if (fatal!=MINI_ERROR_NONFATAL)
      QMessageBox::warning(0, "error", m.c_str(), QMessageBox::Ok);
}

void centerWidgetOnScreen(QWidget *widget)
{
   QRect desktop = QApplication::desktop()->availableGeometry(widget);
   widget->move(widget->pos() + desktop.center() - widget->frameGeometry().center());
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

   miniseed();

   minilog::on();
   minilog::logfile("qtexample.log");
   MINILOG("program start");

   MainWindow window;

   window.show();
   centerWidgetOnScreen(&window);

   return(app.exec());
}
