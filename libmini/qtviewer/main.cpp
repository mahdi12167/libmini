// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_core.h>
#include <mini/mini_generic.h>

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>
#include <QThread>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal)
{
   ministring msg;

   if (fatal==MINI_ERROR_NONFATAL) msg = "warning";
   else if (fatal==MINI_ERROR_MEM) msg = "insufficient memory";
   else if (fatal==MINI_ERROR_IO) msg = "io error";
   else msg = "fatal error";
   msg += ministring(" in <") + file + "> at line " + line + "!";

   fprintf(stderr, "%s\n", msg.c_str());

   MINILOG(msg);

   bool isGuiThread = (QThread::currentThread() == QCoreApplication::instance()->thread());

   if (fatal!=MINI_ERROR_NONFATAL)
      if (isGuiThread)
         QMessageBox::warning(0, "error", msg.c_str(), QMessageBox::Ok);
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
   minilog::logfile("qtviewer.log");
   MINILOG("program start");

   MainWindow window;

   window.show();
   centerWidgetOnScreen(&window);

   return(app.exec());
}
