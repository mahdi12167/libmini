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
   m += ministring(" in <") + file + "> at line " + line + "!";
   if (msg!=NULL) m += ministring(" description=\"") + msg + "\"";

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

void usage(const char *prog)
{
   QString name(prog);
   std::cout << "usage:" << std::endl;
   std::cout << " " << name.mid(name.lastIndexOf("/")+1).toStdString() << " {options} {url}" << std::endl;
   std::cout << "where options are:" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "where url is:" << std::endl;
   std::cout << " a file name or url to a loadable object" << std::endl;
   std::cout << "where object is:" << std::endl;
   std::cout << " a terrain tileset (directory)" << std::endl;
   std::cout << " a geotiff image (.tif)" << std::endl;
   std::cout << " a gps path (.csv .gpx)" << std::endl;
   exit(0);
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

   // get argument list
   QStringList args = QCoreApplication::arguments();

   // scan for arguments and options
   QStringList arg,opt;
   for (int i=1; i<args.size(); i++)
      if (args[i].startsWith("--")) opt.push_back(args[i].mid(2));
      else if (args[i].startsWith("-")) opt.push_back(args[i].mid(1));
      else arg.push_back(args[i]);

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="help") usage(argv[0]);
      else usage(argv[0]);

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
