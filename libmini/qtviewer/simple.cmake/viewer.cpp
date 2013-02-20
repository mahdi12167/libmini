// (c) by Stefan Roettger, licensed under GPL 2+

#include "viewer.h"

MyQtViewer::MyQtViewer()
{
   QAction *quitAction;

   quitAction = new QAction(tr("Q&uit"), this);
   quitAction->setShortcuts(QKeySequence::Quit);
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

MyQtViewer::~MyQtViewer()
{}

mininode_group *MyQtViewer::build_ecef_geometry()
{
   mininode_group *group = new mininode_group();

   // define equator:

   minidyna<miniv3d> pos;
   static const int eqlines = 500;

   for (int i=0; i<=eqlines; i++)
   {
      minicoord c(miniv3d((double)i/eqlines*360*3600, 0.0, 0.0), minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);
      pos.append(c.vec);
   }

   group->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_geometry_band(pos, pos, 20000));

   return(group);
}
