// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/mininodes.h>

#include "renderer.h"

Renderer::Renderer(ViewerWindow* window)
   : Viewer(window)
{}

Renderer::~Renderer()
{}

mininode_group *Renderer::build_ecef_geometry()
{
   mininode_group *group = new mininode_group();

   // define ecef z-axis:

   mininode *axis = new mininode_group();
   mininode_ref pole(new mininode_geometry_tube(10000, 500000));

   axis->append_child(new mininode_coord(minicoord(miniv3d(0, -90*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0, 0.0, 0.5)))->
      append_child(pole);

   axis->append_child(new mininode_coord(minicoord(miniv3d(0, 90*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0, 0.0, 1.0)))->
      append_child(pole);

   group->append_child(axis);

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
