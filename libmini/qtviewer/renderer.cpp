// (c) by Stefan Roettger, licensed under GPL 2+

#undef TEST

#include "nodes.h"
#include "renderer.h"

Renderer::Renderer(ViewerWindow* window)
   : Viewer(window)
{}

Renderer::~Renderer()
{}

mininode_group *Renderer::build_ecef_geometry()
{
   mininode_group *group = new mininode_group;

   // define ecef z-axis:

   mininode *axis = new mininode_group;
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

#ifdef TEST

   // house:

   // define local coordinate system at Lat/Lon=49/11
   mininode *house = new mininode_coord(minicoord(miniv3d(11*3600, 49*3600, 0), minicoord::MINICOORD_LLH));

   // scale up by a factor of 10000
   mininode *scale = house->append_child(new mininode_scale(10000));

   // white cube with base size 10x10 and height 5 meters
   // origin of the cube is its barycenter
   // translate up 2.5 meters
   scale->append_child(new mininode_color(miniv3d(1.0, 1.0, 1.0)))->
      append_child(new mininode_texgen_translate(miniv3d(0.5,0.5,0.5)))->
      append_child(new mininode_texgen_rotate(2,miniv3d(0,1,0)))->
      append_child(new mininode_texgen_rotate(90,miniv3d(1,0,0)))->
      append_child(new mininode_texgen_scale(0.7))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_volume("data/textures/Wood.db"))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 2.5)))->
      append_child(new mininode_geometry_cube(10.0, 10.0, 5.0));

   // grey prism as roof with base size 10x10
   // origin of the prism is the center of its base plane
   // translate up 5 meters
   scale->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_image("data/textures/wood.jpg"))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_prism(10.0, 10.0, 2.5));

   group->append_child(house);

   // tetrahedron /w procedural 2D texture:

   minicoord c0(miniv4d(-159*3600, 21*3600, 0), minicoord::MINICOORD_LLH);
   minicoord c1(miniv4d(-158*3600, 22*3600, 0), minicoord::MINICOORD_LLH);
   minicoord c2(miniv4d(-159*3600, 23*3600, 0), minicoord::MINICOORD_LLH);
   minicoord c3(miniv4d(-160*3600, 22*3600, 0), minicoord::MINICOORD_LLH);

   minicurve curve;

   curve.append_point(c3,-5);
   curve.append_point(c0,0);
   curve.append_point(c1,5);
   curve.append_point(c2,10);
   curve.append_point(c3,15);
   curve.append_point(c0,20);
   curve.append_point(c1,25);

   curve.set_time_repeat(0,20);

   group->append_child(new mininode_coord_animation(curve))->
      append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_image("data/textures/Checker.db"))->
      append_child(new mininode_scale(10000))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_tet(10));

   // tetrahedron /w procedural 3D texture:

   group->append_child(new mininode_coord(minicoord(miniv4d(-159*3600, 24*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_volume("data/textures/Marble.db"))->
      append_child(new mininode_scale(10000))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_tet(10));

   // semi-transparent spheres:

   mininode_ref sphere = new mininode_geometry_sphere(10.0);
   mininode *spheres = group->append_child(new mininode_deferred_semitransparent())->
                       append_child(new mininode_coord(minicoord(miniv4d(-180*3600, 0, 0), minicoord::MINICOORD_LLH)))->
                       append_child(new mininode_scale(10000));

   spheres->append_child(new mininode_translate(miniv3d(0.0, 0.0, 10.0)))->
      append_child(new mininode_color(miniv4d(0.5, 1.0, 0.5, 0.5)))->
      append_child(sphere);

   spheres->append_child(new mininode_translate(miniv3d(-9.0, 0.0, 10.0)))->
      append_child(new mininode_color(miniv4d(1.0, 0.5, 0.5, 0.5)))->
      append_child(sphere);

   spheres->append_child(new mininode_translate(miniv3d(9.0, 0.0, 10.0)))->
      append_child(new mininode_color(miniv4d(0.5, 0.5, 1.0, 0.5)))->
      append_child(sphere);

   // semi-transparent grid_extent:

   grid_extent ext;
   ext.set(minicoord(miniv4d(-180*3600, 0, 0), minicoord::MINICOORD_LLH), 10*3600,10*3600);

   group->append_child(new mininode_deferred_semitransparent())->
      append_child(new mininode_color(miniv4d(0.5, 0.5, 1.0, 0.5)))->
      append_child(new node_grid_extent(ext));

#endif

   return(group);
}
