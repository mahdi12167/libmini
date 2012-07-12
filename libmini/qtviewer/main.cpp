// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mini_core.h>

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>
#include <QtGui/QDesktopWidget>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal)
{
   QString msg;

   if (fatal==MINI_ERROR_NONFATAL) msg = "warning";
   else if (fatal==MINI_ERROR_MEM) msg = "insufficient memory";
   else if (fatal==MINI_ERROR_IO) msg = "io error";
   else msg = "fatal error";
   msg += " in <" + QString(file) + "> at line " + QString::number(line) + "!\n";

   fprintf(stderr, "%s", msg.toStdString().c_str());

   if (fatal!=MINI_ERROR_NONFATAL)
      QMessageBox::warning(0, "error", msg, QMessageBox::Ok);
}

void centerWidgetOnScreen(QWidget *widget)
{
   QRect rect = QApplication::desktop()->availableGeometry();
   widget->move(rect.center() - widget->rect().center());
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

   minilog::on();
   MINILOG("program start");

   MainWindow window;

   window.resize(window.sizeHint());
   centerWidgetOnScreen(&window);
   window.show();

   return(app.exec());
}
