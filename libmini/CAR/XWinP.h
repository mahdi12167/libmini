/* Includes */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include "defs.h"
#include "driver.h"
#include "menu.h"
#include "XWin.h"

/* modul-lokale Makro-Definitionen */
#define WHICH_EVENTS (ButtonPressMask|ButtonReleaseMask|PointerMotionMask|\
                      KeyPressMask|KeyReleaseMask|\
                      ExposureMask|StructureNotifyMask)

#define string(s) XmStringCreateLocalized(s)

#define getsize()\
   {\
   XtVaGetValues(Xarea,XmNwidth,&Xareawidth,XmNheight,&Xareaheight,NULL);\
   viewportwidth=Xareawidth;\
   viewportheight=Xareaheight;\
   }

#define handle(e) gotoeventhandler(e,XWinevent->xbutton.x,\
                                     viewportheight-1-XWinevent->xbutton.y,\
                                     XWinevent->xbutton.button)

#define MAX_DEPTH (10)
#define modelview modelviewstack[depth]

#define update()\
   {\
   transformation=modelview;\
   multiplyMbyN(&transformation,&projection,M_LEFT);\
   }

#define clip(vertex,z)\
   {\
   vertex.v[0]+=(z-vertex.v[2])*(v2.v[0]-v1.v[0])/(v2.v[2]-v1.v[2]);\
   vertex.v[1]+=(z-vertex.v[2])*(v2.v[1]-v1.v[1])/(v2.v[2]-v1.v[2]);\
   vertex.v[2]=z;\
   vertex.v[3]=fabs(z);\
   }

#define LEFT   (1)
#define RIGHT  (2)
#define BOTTOM (4)
#define TOP    (8)

#define clipx2D(x,y,u)\
   {\
   y+=(u-x)*(y2-y1)/(x2-x1);\
   x=u;\
   }

#define clipy2D(x,y,u)\
   {\
   x+=(u-y)*(x2-x1)/(y2-y1);\
   y=u;\
   }

/* modul-lokale Typ-Deklarationen */

/* modul-lokale Variablen-Deklarationen */
XtAppContext app;
Widget       toplevel,mainwin,manager,Xarea=NULL,info;
GC           gc=NULL;

eventhandlertype *gotoeventhandler;
backgroundtype   *gotobackground;

Dimension Xareawidth,Xareaheight;

matrix projection,modelviewstack[MAX_DEPTH],transformation;
float  znear,zfar;
int    depth;

int compiling=FALSE,picking=FALSE;

/* modul-lokale Funktions-Deklarationen */
void    drawmenu(menItem *item);
void    menuhandler(Widget button,XtPointer clientdata,XtPointer whydata);
void    notify(char *message);
void    dummyhandler(Widget widget,XtPointer clientdata,XEvent *XWinevent,Boolean *keephandling);
Boolean dummyprocess(XtPointer clientdata);
void    drawline2D(int x1,int y1,int x2,int y2);
