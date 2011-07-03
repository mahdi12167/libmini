#ifndef RENDERER_H
#define RENDERER_H

#include <QTime>
#include <QtOpenGL/qgl.h>
#include <mini/viewerbase.h>
#include <mini/miniearth.h>
#include <mini/miniterrain.h>

#define MAX_BASE_URL_LEN 255

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
    double      fov;
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

    miniv3d     frustumPointsGL[8];
    miniv4d     frustumPlanesGL[6];

    // libmini layers
    minilayer*  refLayer;
    minilayer*  nearestLayer;

    bool        doupdate;
    bool        dooverride;
    bool        updated;
};

class Ray
{
public:
  Ray() {}

  Ray(miniv3d o, miniv3d d)
  {
    origin = o;
    direction = d;
    inv_direction = miniv3d(1/d.x, 1/d.y, 1/d.z);
    sign[0] = (inv_direction.x < 0);
    sign[1] = (inv_direction.y < 0);
    sign[2] = (inv_direction.z < 0);
  }

  Ray(const Ray &r)
  {
    origin = r.origin;
    direction = r.direction;
    inv_direction = r.inv_direction;
    sign[0] = r.sign[0]; sign[1] = r.sign[1]; sign[2] = r.sign[2];
  }

  miniv3d origin;
  miniv3d direction;
  miniv3d inv_direction;
  int sign[3];
};

struct BoundingBox
{
    miniv3d minPoint;
    miniv3d maxPoint;
};

class Renderer
{
public:
    Renderer(QGLWidget* window);
    ~Renderer();

    bool    setMapURL(const char* baseurl, const char* baseid, const char* basepath1, const char* basepath2);

    void    initCamera(float latitude, float longitude, float altitude, float heading, float pitch, float fov, float nearplane, float farplane);

    void    init();
    void    resize(int width, int height);
    void    draw();

    void    rotateCamera(float dx, float dy);
    void    moveCamera(float dx, float dy);

    void    moveCameraForward(float delta);
    void    setCameraFastMoveForward(bool bEnable);
    void    moveCursor(const QPoint& pos);

    void    resetMapOrientation();
    bool    processResetMapOrientation(int deltaT);
    void    focusOnTarget();
    bool    processFocusOnTarget(int deltaT);
    void    resetMap();
    bool    processResetMap(int deltaT);

    void    toggleLinearMode();
    void    toggleTerrainRender();
    void    toggleFreeCamera();

    bool    isInited() { return m_bIsInited; }

    void    timerEvent(int timerId);

protected:
    void    initParameters();
    void    initVISbathymap();
    void    initNPRbathymap();
    void    initDiscVertices();

    void    resizeViewport();
    void    initView();

    void    resizeTextures(int width, int height);
    void    initFBO();
    void    bindFBO();
    void    unbindFBO();
    void    attachTexture(int textureId);
    void    loadTextureFromResource(const char* respath, GLuint& texId);

    void    setupMatrix();

    void    renderLandscape();
    void    renderTerrain();
    void    renderOverlay();
    void    renderComposition();
    void    renderHUD();

    void    updateCamera();
    void    updateFrustum();
    void    updateVisibility();

    // helper functions
    static void CalculateFrustumPlanes(miniv3d* points, miniv4d* planes);
    static void FindMinMax(const miniv4d& pos, float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ);
    static miniv3d vec3cross(const miniv3d& v0, const miniv3d& v1);
    static miniv4d points2plane(const miniv3d& v0, const miniv3d& v1, const miniv3d& v2);
    minicoord trace2ground(minicoord point, double& dist);

    bool    isBoundingBoxVisible(const BoundingBox& bb);
    bool    RayBoundingBoxIntersect(const BoundingBox& bb, const Ray& ray);

    void    startTransition(CameraTransitionMode mode);
    void    stopTransition();

private:
    void    drawRefFrame(miniv3d pos);
    void    drawLine(miniv3d pos1, miniv3d pos2, int width = 1);
    void    drawFullscreenTexQuad();
    void    drawCameraFrustum();
    void    drawScreenAlignedDisc(const miniv3d& pos, float scale = 1.0f, const miniv4d& color = miniv4d(1.0f, 1.0f, 1.0f, 1.0f));
    void    drawScreenAlignedQuadWithTex(const miniv3d& pos,  float scale = 1.0f, const miniv4d& color = miniv4d(1.0f, 1.0f, 1.0f, 1.0f), GLuint texId = 0);
    void    drawDiscNoRotation(const miniv3d& pos, float scale = 1.0f, const miniv4d& color = miniv4d(1.0f, 1.0f, 1.0f, 1.0f));
    void    drawQuadWithTexNoRotation(const miniv3d& pos, float scale = 1.0f, const miniv4d& color = miniv4d(1.0f, 1.0f, 1.0f, 1.0f), GLuint texId = 0);
    void    drawCylinder(const miniv3d& pos, float scale = 1.0f, float height = 1.0f, const miniv4d& color = miniv4d(1.0f, 1.0f, 1.0f, 1.0f));
    void    drawBoundingBox(const BoundingBox& bb, miniv3d color);
    void    drawText(float x, float y, QString& str, QColor color = QColor(255, 255, 255), bool bIsDoublePrint = true);

protected:
    QGLWidget*  window;

    bool        m_bIsInited;
    bool        m_bdrawBoundingBox;

    Camera      m_Camera;

    // for debug camera
    Camera      m_DebugCamera;
    Camera      m_CameraSave;

    char*       m_strBaseURL;
    char*       m_strBaseID;
    char*       m_strBasePath1;
    char*       m_strBasePath2;

    viewerbase* viewer;
    viewerbase::VIEWER_PARAMS* m_pViewerParams;   // the viewing parameters
    miniearth::MINIEARTH_PARAMS* m_pEarthParams;   // the earth parameters
    miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams;  // the terrain parameters

    float    m_fMoveCameraForward;
    bool     m_bFastCameraMove;

    bool     m_bCameraPanning;
    float    m_fMoveCameraX;
    float    m_fMoveCameraY;

    bool     m_bCameraRotating;

    bool     m_bRenderTerrain;
    bool     m_bFreeCamera;

    // camera transition animation
    bool     m_bInCameraTransition;
    int      m_MapTransitionTimerId;
    QTime    m_Timer;

    // disable cursor move when move camera forward
    int      m_DisableCursorMoveTimerId;
    bool     m_bDisableCursorMoveEvent;

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

    // texture ids for track data points and crosshair
    GLuint     m_CrosshairTextureId;
};

#endif // RENDERER_H
