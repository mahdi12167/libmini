// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_core.h>
#include <mini/mini_generic.h>

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDesktopWidget>
#else
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>
#endif

#include <QtOpenGL/qgl.h>
#include <QThread>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal,const char *msg)
{
   ministring m;

   if (fatal==MINI_ERROR_NONFATAL) m = "warning";
   else if (fatal==MINI_ERROR_MEM) m = "insufficient memory";
   else if (fatal==MINI_ERROR_IO) m = "io error";
   else m = "fatal error";
   if (msg!=NULL) m += ministring(" description=\"") + msg + "\"";
   m += ministring(" in <") + file + "> at line " + line + "!";

   fprintf(stderr, "%s\n", m.c_str());

   MINILOG(m);

   bool isGuiThread = (QThread::currentThread() == QCoreApplication::instance()->thread());

   if (fatal!=MINI_ERROR_NONFATAL)
      if (isGuiThread)
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

   setlocale(LC_NUMERIC, "C");

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
