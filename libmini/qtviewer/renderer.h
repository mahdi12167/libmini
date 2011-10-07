// (c) by Stefan Roettger

#ifndef RENDERER_H
#define RENDERER_H

#include <QtOpenGL/qgl.h>

#include <mini/miniview.h>
#include "viewerconst.h"

#include "camera.h"
#include "cameraconst.h"

class Renderer: public miniview
{
public:
   Renderer(QGLWidget* window);
   ~Renderer();

   void       init();
   bool       isInited() {return(m_bIsInited);}
   minilayer* loadMap(const char* url);
   void       clearMaps();

   void       resizeWindow();
   void       draw();

   Camera*    getCamera() {return(m_camera);}

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
   QGLWidget* m_window;

   bool m_bIsInited;

   // camera
   Camera* m_camera;

   // parameters
   miniscene::MINISCENE_PARAMS* m_pSceneParams; // the scene parameters
   miniearth::MINIEARTH_PARAMS* m_pEarthParams; // the earth parameters
   miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams; // the terrain parameters
   unsigned char m_BathyMap[VIEWER_BATHYWIDTH*4*2]; // bathy color map

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
