#ifndef RENDERER_H
#define RENDERER_H

#include <QTime>
#include <QtOpenGL/qgl.h>

#include <mini/miniearth.h>
#include <mini/miniterrain.h>

#include <mini/viewerbase.h>
#include "viewerconst.h"

typedef enum tagCameraTransitionMode
{
    TRANSITION_NONE,
    TRANSITION_RESET_HEADING,
    TRANSITION_RESET_MAP,
    TRANSITION_FOCUS_ON_TARGET
} CameraTransitionMode;

struct Camera
{
    minicoord   pos;
    double      heading;
    double      pitch;
    double      fovy;
    double      nearplane;
    double      farplane;
    int         viewportwidth;
    int         viewportheight;

    // global coordinate
    miniv3d     forward;
    miniv3d     side;
    miniv3d     up;

    // camera ray hit ground
    minicoord   posGroundHit;
    double      distToGroundHit;

    // opengl coordinate (world coordinate)
    minicoord   posGL;
    miniv3d     forwardGL;
    miniv3d     sideGL;
    miniv3d     upGL;

    // libmini layers
    minilayer*  refLayer;
    minilayer*  nearestLayer;

    bool        doupdate;
    bool        dooverride;
    bool        updated;
};

class Renderer
{
public:
    Renderer(QGLWidget* window);
    ~Renderer();

    void    setMapURL(const char* url);

    void    initCamera(float fovy, float nearplane, float farplane);
    void    setCamera(float latitude, float longitude, float altitude, float heading, float pitch);

    void    init();
    void    resize(int width, int height);
    void    draw();

    void    rotateCamera(float dx, float dy);
    void    moveCamera(float dx, float dy);

    void    moveCameraForward(float delta);
    void    moveCursor(const QPoint& pos);

    void    resetMapOrientation();
    bool    processResetMapOrientation(int deltaT);
    void    focusOnTarget();
    bool    processFocusOnTarget(int deltaT);
    void    resetMap();
    bool    processResetMap(int deltaT);

    void    timerEvent(int timerId);

protected:
    void    initParameters();
    void    initVISbathymap();

    void    resizeViewport();
    void    initView();
    void    initTransition();

    void    resizeTextures(int width, int height);
    void    initFBO();
    void    bindFBO();
    void    unbindFBO();
    void    attachTexture(int textureId, int depthId = -1);
    void    loadTextureFromResource(const char* respath, GLuint& texId);

    void    setupMatrix();

    void    renderLandscape(bool force=false);
    void    renderTerrain(bool force=false);
    void    renderOverlay();
    void    renderComposition();
    void    renderHUD();

    void    updateCamera();

    void    startTransition(CameraTransitionMode mode);
    void    stopTransition();

private:
    void    drawFullscreenTexQuad();
    void    drawText(float x, float y, QString& str, QColor color = QColor(255, 255, 255), bool bIsDoublePrint = true);

protected:
    QGLWidget*  window;

    bool        m_bIsInited;

    Camera      m_Camera;

    char*       m_strURL;

    viewerbase* viewer;
    viewerbase::VIEWER_PARAMS* m_pViewerParams;   // the viewing parameters
    miniearth::MINIEARTH_PARAMS* m_pEarthParams;   // the earth parameters
    miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams;  // the terrain parameters

    float    m_fMoveCameraForward;

    bool     m_bCameraPanning;
    float    m_fMoveCameraX;
    float    m_fMoveCameraY;

    bool     m_bCameraRotating;

    // camera transition animation
    int      m_MapPagingTimerId;
    bool     m_bInCameraTransition;
    int      m_MapTransitionTimerId;
    QTime    m_Timer;

    CameraTransitionMode  m_CameraTransitionMode;

    // reset map orientation
    int      m_HeadingRotateDirection;

    // focus on target
    bool     m_bSetupFocusingOnTarget;
    minicoord m_TargetCameraPos;
    minicoord m_TransitingCameraPos;

    // reset map
    bool    m_bSetupResetMap;

    QPoint      m_CursorScreenPos;
    minicoord   m_CursorGlobalPos;
    bool        m_CursorValid;

    // offscreen rendering
    GLuint     m_FBOId;
    GLuint     m_DepthBufferId;
    GLuint     m_TerrainTextureId;
    GLuint     m_OverlayTextureId;

    // texture ids
    GLuint     m_CrosshairTextureId;

    unsigned char VIEWER_BATHYMAP[VIEWER_BATHYWIDTH*4*2];
};

#endif
