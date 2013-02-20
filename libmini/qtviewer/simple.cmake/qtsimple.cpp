// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui/QApplication>
#include <QtOpenGL/qgl.h>

#include "qtsimple.h"

class MyQtViewer: public QtSimpleViewer
{
public:

   MyQtViewer()
   {
      QAction *quitAction;

      quitAction = new QAction(tr("Q&uit"), this);
      quitAction->setShortcuts(QKeySequence::Quit);
      connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
   }

   ~MyQtViewer()
   {}

protected:

   virtual mininode_group *build_ecef_geometry()
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

};

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   if (!QGLFormat::hasOpenGL()) return(1);

   MyQtViewer viewer;
   viewer.show();

   return(app.exec());
}
