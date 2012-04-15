// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef VIEWER_H
#define VIEWER_H

#include <QtOpenGL/qgl.h>

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>
#include <mini/mini_sfx.h>

#include "viewerwindow.h"
#include "viewerconst.h"

#include "camera.h"
#include "cameraconst.h"

//! qt viewer class for viewing tilesets
class Viewer: public miniview
{
public:
   Viewer(ViewerWindow* window);
   ~Viewer();

   void       init();
   bool       isInited() {return(m_bIsInited);}

   Camera*    getCamera();
   void       resizeWindow();

   minilayer* loadMap(ministring url);
   void       removeMap(minilayer *layer);
   void       clearMaps();

   void       draw();

   void       toggleStereo(bool on);
   void       toggleWireFrame(bool on);

   void       checkFog(bool on);
   void       setFogDensity(double density);
   void       checkContours(bool on);
   void       checkSeaLevel(bool on);
   void       setSeaLevel(double level);
   void       checkLight(bool on);
   void       setLight(double hour);
   void       checkExagger(bool on);
   void       setExagger(double scale);

protected:
   void       initParameters();
   void       initBathyMap();

   void       resizeViewport();
   void       renderHUD();

private:
   void       loadTextureFromResource(const char* respath, GLuint& texId);
   void       drawText(float x, float y, QString& str, QColor color = QColor(255, 255, 255), bool bIsDoublePrint = true);

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
   unsigned char m_BathyMap[VIEWER_BATHYWIDTH*4*2]; // bathy color map

   virtual mininode_group *build_ecef_geometry()=0;

   virtual void check_ecef_geometry(miniv3d &center, double &radius);
   virtual void render_ecef_geometry(double t);
   virtual double shoot_ecef_geometry(const miniv3d &o,const miniv3d &d,double mindist=0.0);

private:
   GLuint    m_CrosshairTextureId;

   double    m_StereoBase;
   double    m_FogDensity;
   double    m_SeaLevel;
   double    m_DayHourDelta;
   bool      m_ExaggerOn;
   double    m_ExaggerScale;
};

#endif
