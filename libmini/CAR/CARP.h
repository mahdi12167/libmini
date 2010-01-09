/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "defs.h"
#include "driver.h"
#include "landscape.h"
#include "data.h"

/* modul-lokale Makro-Definitionen */
#define G (-9.81)

#define MPH *1.6/3.6

#define TURN  (7.0)
#define ACCEL (5.0)
#define BRAKE (-13.0)

#define MAX_TURN  (30.0)
#define MAX_SPEED (110 MPH)

#define MAX_SHOCK (6.0)
#define MAX_HIT   (100.0)

#define SLIDE ((accel==0.0)?1.0:2.0)
#define DRIFT ((accel==0.0)?3.0:5.0)
#define SLICK (7.0)

#define WATER (-2.0)
#define SINK  (-1 MPH)

#define FRAMERATE (15)
#define STEPRATE  (50)
#define ADAPTION  (0.5)
#define MAX_RES   (100.0)

#define XSTART (-YSTART)
#define YSTART (H+FAR/2.0)
#define ZSTART (YSTART)

#define DOFFSET (7.0)
#define HOFFSET (3.0)

#define BOFFSET (60.0)
#define YOFFSET (40.0)

#define FOLLOW (5.0)
#define SAFETY (3.0*NEAR/cos(FOVY/360.0*PI))

#define SPIN ((x-lx)/(viewportwidth/2.0)*MAX_TURN)

#define normalize(alpha)\
   {\
   while (alpha<0.0) alpha+=360.0;\
   while (alpha>=360.0) alpha-=360.0;\
   }

/* modul-lokale Typ-Deklarationen */
enum{UP,DOWN};

enum{EARTH,VENUS,MARS,MOON};

/* modul-lokale Variablen-Deklarationen */
int planet=EARTH,
    noair,noice,
    toggle1,toggle2;

float r1,g1,b1,
      r2,g2,b2;

double g,
       turn,accel,brake,
       x0,z0,h,
       dy11,dy21,dy12,dy22,
       rx,ry,rz,
       ux,uy,uz,
       fx,fy,fz,
       wreck,
       front,dir,speed,
       df,dh,
       px,py,pz;

float res=1.0;

/* modul-lokale Funktions-Deklarationen */
void eventhandler(Event e,int x,int y,int button);
void initscene(void);
void drawscene(void);
