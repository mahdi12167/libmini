/* globale Makro-Definitionen */

/* globale Typ-Deklarationen */

/* globale Variablen-Deklarationen */

/* globale Funktions-Deklarationen */
void XWinopenwindow(int width,int height,char *title);
void XWinstarteventloop(eventhandlertype *eventhandler,backgroundtype *background);
void XWinclearwindow(void);
void XWinflushbuffer(void);
void XWinclosewindow(void);
void XWinperspective(float fovy,float aspect,float near,float far);
void XWinlookat(float eyex,float eyey,float eyez,float centerx,float centery,float centerz,float upx,float upy,float upz);
void XWinrotatex(float spin);
void XWinrotatey(float spin);
void XWinrotatez(float spin);
void XWintranslate(float dx,float dy,float dz);
void XWinscale(float sx,float sy,float sz);
void XWinrotatexM(matrix *M,float spin,int where);
void XWinrotateyM(matrix *M,float spin,int where);
void XWinrotatezM(matrix *M,float spin,int where);
void XWintranslateM(matrix *M,float dx,float dy,float dz,int where);
void XWinscaleM(matrix *M,float sx,float sy,float sz,int where);
void XWinmultiply(matrix *M);
void XWinpush(void);
void XWinpop(void);
void XWinbeginlist(int id,int pickable);
void XWinendlist(void);
void XWinuselist(int id);
void XWinbeginpicking(int x,int y);
int  XWinendpicking(void);
void XWinsetcolor(float red,float green,float blue);
void XWinloadtexmap(char *filename,int width,int height,float scale,float offset);
void XWindrawline(float x1,float y1,float z1,float x2,float y2,float z2);
