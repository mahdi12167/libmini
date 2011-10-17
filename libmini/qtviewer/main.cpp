// (c) by Stefan Roettger

#include <mini/mini.h>

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include <QtGui/QMessageBox>

#include "mainwindow.h"

void errormsg(const char *file,int line,int fatal)
{
   if (fatal==MINI_ERROR_NONFATAL) fprintf(stderr,"warning");
   else if (fatal==MINI_ERROR_MEM) fprintf(stderr,"insufficient memory");
   else if (fatal==MINI_ERROR_IO) fprintf(stderr,"io error");
   else fprintf(stderr,"fatal error");
   fprintf(stderr," in <%s> at line %d!\n",file,line);
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
