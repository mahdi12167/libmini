#ifndef RENDERER_H
#define RENDERER_H

#include <QTime>
#include <QtOpenGL/qgl.h>

#include <mini/miniearth.h>
#include <mini/miniterrain.h>
#include <mini/minicam.h>

#include <mini/viewerbase.h>
#include "viewerconst.h"

#include "viewerwindow.h"

class Renderer
{
public:
   Renderer(QGLWidget* window);
   ~Renderer();

   void     setMapURL(const char* url);
   void     loadMapURL(const char* url);

   void     init();
   void     resizeWindow(int width, int height);
   void     draw();

   void     setCamera(float latitude, float longitude, float altitude, float heading, float pitch);
   minicam* getCamera() {return(camera);}

   void     rotateCamera(float dx, float dy);

   void     moveCameraForward(float delta);
   void     moveCameraSideward(float delta);
   void     moveCursor(const QPoint& pos);

   void     focusOnTarget();
   void     timerEvent(int timerId);

   void     modifierKey(modifierKeys modifier, bool pressed);
   void     toggleWireframe();

protected:
   void     initParameters();
   void     initVISbathymap();

   void     resizeViewport();

   void     initView();
   void     initTransition();

   void     setupMatrix();
   void     renderTerrain();
   void     renderHUD();

   void     startIdling();
   void     stopIdling();

   void     startTransition(minicoord target);
   void     stopTransition();

   void     processTransition(double dt);

   miniv3d  unprojectMouse();
   miniv3d  targetVector();
   miniv3d  cursorVector();

private:
   void     loadTextureFromResource(const char* respath, GLuint& texId);
   void     drawText(float x, float y, QString& str, QColor color = QColor(255, 255, 255), bool bIsDoublePrint = true);

protected:
   QGLWidget* window;

   int viewportwidth;
   int viewportheight;

   bool m_bIsInited;

   // tileset url
   char* m_strURL;

   // viewer
   viewerbase* viewer;
   viewerbase::VIEWER_PARAMS* m_pViewerParams;   // the viewing parameters
   miniearth::MINIEARTH_PARAMS* m_pEarthParams;   // the earth parameters
   miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams;  // the terrain parameters
   unsigned char m_BathyMap[VIEWER_BATHYWIDTH*4*2]; // bathy color map

   // camera
   minicam*  camera;

   // camera idling timer
   int       m_IdlingTimerId;
   QTime     m_IdlingTimer;

   // camera transition animation
   minicoord m_TargetCameraPos;
   bool      m_bInCameraTransition;
   int       m_TransitionTimerId;
   QTime     m_TransitionTimer;

   // cursor position
   QPoint    m_CursorScreenPos;
   bool      m_CursorValid;

   // modifier keys
   bool      m_Shift, m_Control, m_Meta;

   // texture ids
   GLuint m_CrosshairTextureId;
};

#endif
