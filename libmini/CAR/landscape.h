/* globale Makro-Definitionen */

/* globale Typ-Deklarationen */

/* globale Variablen-Deklarationen */

/* globale Funktions-Deklarationen */
void initlandscape(void);
void drawlandscape(float c,
                   float x0,float dy0,float z0,
                   float ex,float ey,float ez,
                   float dx,float dy,float dz,
                   float ux,float uy,float uz,
                   float fovy,float aspect,
                   float near,float far,
                   float r1,float g1,float b1,
                   float r2,float g2,float b2);

double height(double x0,double z0);
double dheight(double x0,double y0,double z0,double smoothing);

/* height field loader: */
extern float H;
