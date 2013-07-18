// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWER_H
#define VIEWER_H

#include <QtOpenGL/qgl.h>

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>
#include <mini/mini_sfx.h>

#include "viewerwindow.h"

//! qt viewer class for viewing tilesets
class Viewer: public miniview
{
public:
   Viewer(ViewerWindow* window);
   ~Viewer();

   void       init();
   bool       isInited() {return(m_bIsInited);}

   void       resizeWindow();

   mininode_root* getRoot();
   mininode_cam* getCamera();

   minilayer* loadMap(ministring url);
   void       removeMap(minilayer *layer);
   void       clearMaps();

   void       draw();

   mininode_geometry *pick(const minicoord &o, const miniv3d &d, double mindist=0.0);

protected:
   void       resizeViewport();

protected:
   // parent window reference
   ViewerWindow* m_window;

   // scene graph root node
   mininode_rootref m_root;

   // initialization flag
   bool m_bIsInited;

   // parameter pointers
   miniscene::MINISCENE_PARAMS* m_pSceneParams; // the scene parameters
   miniearth::MINIEARTH_PARAMS* m_pEarthParams; // the earth parameters
   miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams; // the terrain parameters

   virtual mininode_group *build_ecef_geometry()=0;

   virtual void check_ecef_geometry(miniv3d &center, double &radius);
   virtual void render_ecef_geometry(double t);
   virtual double shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double mindist=0.0);

private:

   void initParameters();
};

#endif
