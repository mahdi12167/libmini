/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <values.h>
#include "defs.h"
#include "driver.h"
#include "map.h"
#include "landscape.h"

/* modul-lokale Makro-Definitionen */
#define S ((1<<8)+1)
#define D ((float)(MAP_S-1)/(S-1)*(float)MAP_D)

#define M (2.0f)
#define C (3.0f)

#define K (C/2.0f/(C-1.0f))

#define N (1<<5)

#define X(i) (((i)-S/2)*D)
#define Z(j) ((S/2-(j))*D)

#define Y(i,j) (y[i][j])
#define h(y)   (y)

#define cpr(x)  (((x)>=1.0f)?255:ftrc((x)*255.0f))
#define dcpr(x) ((x)/255.0f)

float F;
#define blendE(bc,y0,y1,y2) (((F=(bc))==255.0f)?\
                             (y0):(F*(y0)+(255.0f-F)*((y1)+(y2))/2.0f)/255.0f)
#define blendM(bc,y0,y1,y2) (((F=(bc))==0.0f)?MAXFLOAT:(F==255.0f)?\
                             (y0):(F*(y0)+(255.0f-F)*((y1)+(y2))/2.0f)/255.0f)

#define blendED(i,j,e1,e2) blendE(bc[i][j],Y(i,j),e1,e2)

#define blendMV(i,j,s2,e1,e2) blendM(min((i>=s2)?bc[i-s2][j]:255,\
                                         (i+s2<S)?bc[i+s2][j]:255),Y(i,j),e1,e2)
#define blendMH(i,j,s2,e1,e2) blendM(min((j>=s2)?bc[i][j-s2]:255,\
                                         (j+s2<S)?bc[i][j+s2]:255),Y(i,j),e1,e2)

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
short int y[S][S];

unsigned char fc[S][S],bc[S][S];

float DH[S];

float c,
      X0,DY0,Z0,
      EX,EY,EZ,
      DX,DY,DZ,
      PLANE,
      k1,k2;

float nx1,ny1,nz1,
      nx2,ny2,nz2,
      nx3,ny3,nz3,
      nx4,ny4,nz4,
      k11,k12,
      k21,k22,
      k31,k32,
      k41,k42;

/* modul-lokale Funktions-Deklarationen */
void calcmap(int i,int j,int s);
void drawmap(int i,int j,int s,float m0,float e1,float e2,float e3,float e4);

/* height field loader: */

short int *map,**tab;
float H;

unsigned short int INTEL_CHECK=1;

void loadmap(void);
void swapmap(void);
void convertmap(void);
