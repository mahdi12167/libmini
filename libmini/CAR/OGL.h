/* globale Makro-Definitionen */

/* globale Typ-Deklarationen */

/* globale Variablen-Deklarationen */

/* globale Funktions-Deklarationen */
void OGLopenwindow(int width,int height,char *title);
void OGLclearwindow(void);
void OGLswapbuffers(void);
void OGLclosewindow(void);
void OGLperspective(float fovy,float aspect,float near,float far);
void OGLlookat(float eyex,float eyey,float eyez,float centerx,float centery,float centerz,float upx,float upy,float upz);
void OGLrotatex(float spin);
void OGLrotatey(float spin);
void OGLrotatez(float spin);
void OGLtranslate(float dx,float dy,float dz);
void OGLscale(float sx,float sy,float sz);
void OGLrotatexM(matrix *M,float spin,int where);
void OGLrotateyM(matrix *M,float spin,int where);
void OGLrotatezM(matrix *M,float spin,int where);
void OGLtranslateM(matrix *M,float dx,float dy,float dz,int where);
void OGLscaleM(matrix *M,float sx,float sy,float sz,int where);
void OGLmultiply(matrix *M);
void OGLpush(void);
void OGLpop(void);
void OGLbeginlist(int id,int pickable);
void OGLendlist(void);
void OGLuselist(int id);
void OGLbeginpicking(int x,int y);
int  OGLendpicking(void);
void OGLlight(float dx,float dy,float dz);
void OGLsetcolor(float red,float green,float blue);
void OGLloadtexmap(char *filename,int width,int height,float scale,float offset);
void OGLdrawline(float x1,float y1,float z1,float x2,float y2,float z2);
void OGLfilltriangle(float x1,float y1,float z1,float nx1,float ny1,float nz1,float r1,float g1,float b1,float x2,float y2,float z2,float nx2,float ny2,float nz2,float r2,float g2,float b2,float x3,float y3,float z3,float nx3,float ny3,float nz3,float r3,float g3,float b3);
void OGLtexturetriangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3);
void OGLfillrectangle(float x,float y,float z,float dx1,float dy1,float dz1,float dx2,float dy2,float dz2);
void OGLtexturerectangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float x4,float y4,float z4);
void OGLbeginfan(void);
void OGLfanvertex(float x,float y,float z);
