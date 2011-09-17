// (c) by Stefan Roettger

#ifndef VIEWER_H
#define VIEWER_H

#include <mini/viewerbase.h>

class Camera;

class Viewer : public viewerbase
{
public:
   Viewer(Camera *camera);
   ~Viewer();

   void render_geometry();

protected:
   void setup_matrix();
   void render_terrain_geometry();
   void render_ecef_geometry();

   Camera *m_camera;
};

#endif
