// (c) by Stefan Roettger

#ifndef RENDERER_H
#define RENDERER_H

#include "viewer.h"

class ViewerWindow;

//! qt viewer class for viewing tilesets plus ecef geometry
class Renderer: public Viewer
{
public:
   Renderer(ViewerWindow* window);
   ~Renderer();

protected:
   virtual mininode_group *build_ecef_geometry();
};

#endif
