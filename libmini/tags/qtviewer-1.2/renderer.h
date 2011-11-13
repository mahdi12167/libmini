// (c) by Stefan Roettger

#ifndef RENDERER_H
#define RENDERER_H

#include "viewer.h"

class Renderer: public Viewer
{
public:
   Renderer(QGLWidget* window);
   ~Renderer();

protected:
   virtual mininode *build_ecef_geometry();
};

#endif
