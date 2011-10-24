// (c) by Stefan Roettger

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
   : Viewer(window)
{}

Renderer::~Renderer()
{}

mininode *Renderer::build_ecef_geometry()
{
   mininode *group=new mininode;

   // define ecef z-axis:

   mininode *axis=new mininode;
   mininoderef pole=mininoderef(new mininode_geometry_tube(10000,500000));

   axis->append(new mininode_coord(minicoord(miniv3d(0,-90*3600,0),minicoord::MINICOORD_LLH)))->
      append(new mininode_color(miniv3d(0,0,0.5)))->
      append(pole);

   axis->append(new mininode_coord(minicoord(miniv3d(0,90*3600,0),minicoord::MINICOORD_LLH)))->
      append(new mininode_color(miniv3d(0,0,1.0)))->
      append(pole);

   group->append(axis);

   // define equator:

   minidyna<miniv3d> pos;
   static const int eqlines=100;

   for (int i=0; i<eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600,0.0,0.0),minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);
      pos.append(c.vec);
      }

   group->append(new mininode_color(miniv3d(0.5,0.5,0.5)))->
      append(new mininode_geometry_band(pos,pos,5000));

   return(group);
}