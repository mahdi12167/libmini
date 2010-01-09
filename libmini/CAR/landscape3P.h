/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "driver.h"
#include "map.h"
#include "landscape.h"

/* modul-lokale Makro-Definitionen */
#define S ((1<<8)+1)
#define D ((float)(MAP_S-1)/(S-1)*MAP_D)

#define N ((S+1)*S/2)
#define Q (100000)

#define C (3.0)

#ifndef IMPACT
#define O (1.0)
#else
#define O (0.0)
#endif

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
float X[N];
float Y[N];
float Z[N];
#ifndef IMPACT
float NX[N];
float NY[N];
float NZ[N];
float FB[N];
#endif
char  FLAG[N];

float *x[S];
float *y[S];
float *z[S];
#ifndef IMPACT
float *nx[S];
float *ny[S];
float *nz[S];
float *fb[S];
#endif
char  *flag[S];

/* modul-lokale Funktions-Deklarationen */
double crd(double rx,double rz,double sx,double sz,double tx,double tz)
   {return(rx*(sz-tz)+sx*(tz-rz)+tx*(rz-sz));}

void drawocean(int s,float far,
               float x1,float z1,
               float x2,float z2,
               float x3,float z3,
               float r,float g,float b);

void drawisland(float c,
                float x0,float dy0,float z0,
                float r,float g,float b);

/* height field loader: */

short int map[MAP_S][MAP_S];
float H;

unsigned short int INTEL_CHECK=1;

void loadmap(void);
void swapmap(void);
void convertmap(void);
void scalemap(void);
