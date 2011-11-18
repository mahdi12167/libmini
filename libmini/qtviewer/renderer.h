// (c) by Stefan Roettger

#ifndef RENDERER_H
#define RENDERER_H

#include "viewer.h"

//! qt viewer class for viewing tilesets plus ecef geometry
class Renderer: public Viewer
{
public:
   Renderer(QGLWidget* window);
   ~Renderer();

protected:
   virtual mininode *build_ecef_geometry();
};

#endif
