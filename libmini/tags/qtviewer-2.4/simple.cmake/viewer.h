// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWER_H
#define VIEWER_H

#include <mini/qt_viewer.h>

class MyQtViewer: public QtViewer
{
   Q_OBJECT;

public:

   MyQtViewer();
   ~MyQtViewer();

protected:

   virtual mininode_group *build_ecef_geometry();

   virtual void timerEvent(QTimerEvent *);
};

#endif
