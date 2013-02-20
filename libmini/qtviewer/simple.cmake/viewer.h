// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWER_H
#define VIEWER_H

#include "qtsimpleviewer.h"

class MyQtViewer: public QtSimpleViewer
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
