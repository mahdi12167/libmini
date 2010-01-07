/* globale Makro-Definitionen */
#define ERROR()\
   {\
   fprintf(stderr,"fatal error in %s at line %d!\n",__FILE__,__LINE__);\
   exit(EXIT_FAILURE);\
   }

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#define FSQRT2 ((float)1.414214)
#define FSQRT3 ((float)1.732051)
#define PI     ((float)3.141593)

#define FOVY   (60.0)
#define ASPECT ((float)viewportwidth/viewportheight)
#define NEAR   (0.3)
#define FAR    (3000.0)

#define SUN -1.0,1.0,FSQRT3

#define BLACK 0.0,0.0,0.0
#define WHITE 1.0,1.0,1.0

#define LINE_WIDTH    (2)
#define VIEWPORT_SIZE (1.0)

#define PICK_SIZE (3)
#define PICK_NOID (0)

#define c_0 {{0.0,0.0,0.0,0.0}}

#define C_0 {{{0.0,0.0,0.0,0.0},\
              {0.0,0.0,0.0,0.0},\
              {0.0,0.0,0.0,0.0},\
              {0.0,0.0,0.0,0.0}}}

#define C_1 {{{1.0,0.0,0.0,0.0},\
              {0.0,1.0,0.0,0.0},\
              {0.0,0.0,1.0,0.0},\
              {0.0,0.0,0.0,1.0}}}

/* globale Typ-Deklarationen */
typedef enum{Epressed,Ereleased,Emotion,Eredraw,Equit,Enone,
             Eflip,Ewarp} Event;

typedef void eventhandlertype(Event e,int x,int y,int button);
typedef void backgroundtype(void);

typedef struct{float M[4][4];} matrix;
typedef struct{float v[4];}    vertex;

enum{M_RIGHT,M_LEFT};

/* globale Variablen-Deklarationen */
extern int viewportwidth,viewportheight;

extern vertex v_0;
extern matrix M_0,M_1;

/* globale Funktions-Deklarationen */
void error(char *message);
void openwindow(char *title,char *driver);
void starteventloop(eventhandlertype *eventhandler,backgroundtype *background);
void clearwindow(void);
void swapbuffers(void);
void closewindow(void);
void perspective(float fovy,float aspect,float near,float far);
void lookat(float eyex,float eyey,float eyez,float centerx,float centery,float centerz,float upx,float upy,float upz);
void rotatex(float spin);
void rotatey(float spin);
void rotatez(float spin);
void translate(float dx,float dy,float dz);
void scale(float sx,float sy,float sz);
void rotatexM(matrix *M,float spin,int where);
void rotateyM(matrix *M,float spin,int where);
void rotatezM(matrix *M,float spin,int where);
void translateM(matrix *M,float dx,float dy,float dz,int where);
void scaleM(matrix *M,float sx,float sy,float sz,int where);
void multiply(matrix *M);
void push(void);
void pop(void);
void beginlist(int id,int pickable);
void endlist(void);
void uselist(int id);
void beginpicking(int x,int y);
int  endpicking(void);
void light(float dx,float dy,float dz);
void setcolor(float red,float green,float blue);
void loadtexmap(char *filename,int width,int height,float scale,float offset);
void drawline(float x1,float y1,float z1,float x2,float y2,float z2);
void drawtriangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3);
void filltriangle(float x1,float y1,float z1,float nx1,float ny1,float nz1,float r1,float g1,float b1,float x2,float y2,float z2,float nx2,float ny2,float nz2,float r2,float g2,float b2,float x3,float y3,float z3,float nx3,float ny3,float nz3,float r3,float g3,float b3);
void texturetriangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3);
void drawrectangle(float x,float y,float z,float dx1,float dy1,float dz1,float dx2,float dy2,float dz2);
void fillrectangle(float x,float y,float z,float dx1,float dy1,float dz1,float dx2,float dy2,float dz2);
void texturerectangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float x4,float y4,float z4);
void beginfan(void);
void fanvertex(float x,float y,float z);

void multiplyMbyv(matrix *M,vertex *v);
void multiplyMbyN(matrix *M,matrix *N,int where);
