#include "XWinP.h"

/* open a window */
void XWinopenwindow(int width,int height,char *title)
   {
   Display   *display;
   XGCValues values;
   XEvent    XWinevent;
   int       dummy=1;

   /* erzeuge alle widgets */
   toplevel=XtVaAppInitialize(&app,"XMotif",NULL,0,&dummy,&title,NULL,NULL);
   mainwin=XmCreateMainWindow(toplevel,"MainWindow",NULL,0);
   XtVaSetValues(mainwin,XmNtraversalOn,FALSE,XmNshowSeparator,TRUE,NULL);
   manager=XmCreateForm(mainwin,"Manager",NULL,0);
   Xarea=XmCreateDrawingArea(manager,"X-Area",NULL,0);
   display=XtDisplay(Xarea);
   XtVaSetValues(Xarea,XmNwidth,width,XmNheight,height,
                       XmNleftAttachment,XmATTACH_FORM,
                       XmNrightAttachment,XmATTACH_FORM,
                       XmNbottomAttachment,XmATTACH_FORM,
                       XmNtopAttachment,XmATTACH_FORM,
                       XmNbackground,WhitePixelOfScreen(XtScreen(Xarea)),NULL);

   XtManageChild(Xarea);
   XtManageChild(manager);
   XtManageChild(mainwin);

   /* zeichne das Fenster und warte auf Vollzugsmeldung */
   XtRealizeWidget(toplevel);
   while (!XCheckMaskEvent(display,ExposureMask,&XWinevent));

   /* erzeuge einen Graphik-Kontext */
   gc=XCreateGC(display,XtWindow(Xarea),0,&values);
   XSetLineAttributes(display,gc,LINE_WIDTH,LineSolid,CapRound,JoinRound);
   }

/* install the menu callback and the event loop */
void XWinstarteventloop(eventhandlertype *eventhandler,backgroundtype *background)
   {
   drawmenu(menInit());
   getsize();

   gotoeventhandler=eventhandler;
   gotoeventhandler(Eredraw,-1,-1,-1);
   gotoeventhandler(Enone,-1,-1,-1);

   XtAddEventHandler(Xarea,WHICH_EVENTS,0,dummyhandler,gc);
   if ((gotobackground=background)!=NULL)
      XtAppAddWorkProc(XtWidgetToApplicationContext(Xarea),dummyprocess,NULL);
   XtAppMainLoop(app);
   }

/* draw a menu */
void drawmenu(menItem *item)
   {
   static int      flag=FALSE;
   static Widget   lastmenu,menu,frame,button;
   static XmString label;

   if (item==NULL)
      {
      if (flag) XtDestroyWidget(menu);
      flag=FALSE;
      return;
      }

   /* create menu bar */
   if (flag) lastmenu=menu;
   menu=XmCreateRowColumn(mainwin,"MenuBar",NULL,0);
   XtVaSetValues(menu,XmNorientation,XmHORIZONTAL,NULL);

   /* create text area */
   frame=XmCreateFrame(menu,"TitleFrame",NULL,0);
   XtVaSetValues(frame,XmNshadowType,XmSHADOW_ETCHED_OUT,XmNshadowThickness,2,NULL);
   info=XmCreateLabel(frame,"Title",NULL,0);
   label=string(item->title);
   XtVaSetValues(info,XmNlabelType,XmSTRING,XmNlabelString,label,NULL);
   XmStringFree(label);
   XtManageChild(info);
   XtManageChild(frame);

   /* create a button for each menu item */
   for (item=item->items; item!=NULL; item=item->next)
      {
      button=XmCreatePushButton(menu,"Button",NULL,0);
      label=string(item->title);
      XtVaSetValues(button,XmNlabelType,XmSTRING,XmNlabelString,label,NULL);
      XmStringFree(label);
      XtAddCallback(button,XmNactivateCallback,menuhandler,(XtPointer)item);
      XtManageChild(button);
      }

   /* create status area */
   info=XmCreateLabel(menu,"String",NULL,0);
   label=string("Choice:");
   XtVaSetValues(info,XmNlabelType,XmSTRING,XmNlabelString,label,NULL);
   XmStringFree(label);
   XtManageChild(info);
   frame=XmCreateFrame(menu,"StatusFrame",NULL,0);
   XtVaSetValues(frame,XmNshadowType,XmSHADOW_ETCHED_OUT,XmNshadowThickness,2,NULL);
   info=XmCreateLabel(frame,"Status",NULL,0);
   label=string("None");
   XtVaSetValues(info,XmNlabelType,XmSTRING,XmNlabelString,label,NULL);
   XmStringFree(label);
   XtManageChild(info);
   XtManageChild(frame);

   /* draw the menu */
   XtManageChild(menu);
   XtVaSetValues(mainwin,XmNmenuBar,menu,NULL);

   /* if necessary get rid of the last menu and it's children */
   if (flag) XtDestroyWidget(lastmenu);
   flag=TRUE;
   }

/* handle menu events */
void menuhandler(Widget button,XtPointer clientdata,XtPointer whydata)
   {
   menItem *item=(menItem *)clientdata;

   switch (item->type)
      {
      case MEN_SUB    : drawmenu(item);
                        gotoeventhandler(item->id,-1,-1,-1);
                        break;
      case MEN_LEAF   : notify(item->title);
                        gotoeventhandler(item->id,-1,-1,-1);
                        break;
      case MEN_RETURN : drawmenu(item->upper->upper);
                        gotoeventhandler(item->id,-1,-1,-1);
                        break;
      }
   }

/* notify a message */
void notify(char *message)
   {
   static XmString label;

   label=string(message);
   XtVaSetValues(info,XmNlabelType,XmSTRING,XmNlabelString,label,NULL);
   XmStringFree(label);
   }

/* check the events and handle them */
void dummyhandler(Widget widget,XtPointer clientdata,XEvent *XWinevent,Boolean *keephandling)
   {
   Display *display=XtDisplay(Xarea);

   char           key;
   KeySym         symbol;
   XComposeStatus status;

   switch (XWinevent->type)
      {
      case ButtonPress     : handle(Epressed); break;
      case ButtonRelease   : handle(Ereleased); break;
      case MotionNotify    : while (XCheckMaskEvent(display,PointerMotionMask,XWinevent));
                             handle(Emotion);
                             break;
      case Expose          : if (XWinevent->xexpose.count==0) handle(Eredraw);
                             break;
      case ConfigureNotify : getsize();
                             handle(Eredraw);
                             break;
      case KeyPress        : XLookupString((XKeyEvent *)XWinevent,&key,1,&symbol,&status);
                             switch (symbol)
                                {
                                case XK_Escape : handle(Equit); break;
                                default        : notify("None"); handle(Enone); break;
                                }
                             break;
      }

   *keephandling=TRUE;
   }

/* when idle jump to the background process */
Boolean dummyprocess(XtPointer clientdata)
   {
   gotobackground();
   return(FALSE);
   }

/* clear window */
void XWinclearwindow()
   {
   if (compiling || picking) return;
   XClearWindow(XtDisplay(Xarea),XtWindow(Xarea));
   }

/* flush event buffer */
void XWinflushbuffer()
   {
   if (compiling || picking) return;
   XFlush(XtDisplay(Xarea));
   }

/* free memory and close window */
void XWinclosewindow()
   {
   Display *display=XtDisplay(Xarea);

   /* weg damit! */
   menDestroy();
   XFreeGC(display,gc);
   XCloseDisplay(display);
   }

/* initialize 3D-graphix */
void XWinperspective(float fovy,float aspect,float near,float far)
   {
   float height2,width2;

   /* compute projection matrix */
   height2=ftan(fovy/360.0*PI)*near;
   width2=height2*aspect;
   if (height2==0.0 || width2==0.0) ERROR();
   projection=M_0;
   projection.M[0][0]=near/width2;
   projection.M[1][1]=near/height2;
   projection.M[2][2]=(near+far)/(near-far);
   projection.M[3][2]=2.0*near*far/(near-far);
   projection.M[2][3]=-1.0;

   /* save clipping plane positions */
   znear=-near;
   zfar=far;

   /* initialize modelview matrix stack */
   depth=0;
   modelview=M_1;
   update();
   }

/* set lookat parameters */
void XWinlookat(float eyex,float eyey,float eyez,
                float centerx,float centery,float centerz,
                float upx,float upy,float upz)
   {
   matrix lookat=M_1;

   float rightx,righty,rightz,length;

   centerx-=eyex;
   centery-=eyey;
   centerz-=eyez;
   if ((length=-fsqrt(centerx*centerx+centery*centery+centerz*centerz))!=0.0)
      {
      centerx/=length;
      centery/=length;
      centerz/=length;

      rightx=upy*centerz-centery*upz;
      righty=upz*centerx-centerz*upx;
      rightz=upx*centery-centerx*upy;
      if ((length=fsqrt(rightx*rightx+righty*righty+rightz*rightz))!=0.0)
         {
         rightx/=length;
         righty/=length;
         rightz/=length;

         upx=centery*rightz-righty*centerz;
         upy=centerz*rightx-rightz*centerx;
         upz=centerx*righty-rightx*centery;

         lookat.M[0][0]=rightx;
         lookat.M[1][0]=righty;
         lookat.M[2][0]=rightz;
         lookat.M[0][1]=upx;
         lookat.M[1][1]=upy;
         lookat.M[2][1]=upz;
         lookat.M[0][2]=centerx;
         lookat.M[1][2]=centery;
         lookat.M[2][2]=centerz;

         XWinmultiply(&lookat);
         }
      }
   XWintranslate(-eyex,-eyey,-eyez);
   }

/* rotate around the x-axis */
void XWinrotatex(float spin)
   {XWinrotatexM(&modelview,spin,M_RIGHT); update();}

/* rotate around the y-axis */
void XWinrotatey(float spin)
   {XWinrotateyM(&modelview,spin,M_RIGHT); update();}

/* rotate around the z-axis */
void XWinrotatez(float spin)
   {XWinrotatezM(&modelview,spin,M_RIGHT); update();}

/* translate by (dx,dy,dz) */
void XWintranslate(float dx,float dy,float dz)
   {XWintranslateM(&modelview,dx,dy,dz,M_RIGHT); update();}

/* scale by sx/sy/sz */
void XWinscale(float sx,float sy,float sz)
   {XWinscaleM(&modelview,sx,sy,sz,M_RIGHT); update();}

/* rotate around the x-axis (multiply with M) */
void XWinrotatexM(matrix *M,float spin,int where)
   {
   static matrix N=C_1;

   float s,c;

   spin=spin/180.0*PI;
   s=fsin(spin);
   c=fcos(spin);
   N.M[1][1]=c;
   N.M[2][1]=-s;
   N.M[1][2]=s;
   N.M[2][2]=c;
   multiplyMbyN(M,&N,where);
   }

/* rotate around the y-axis (multiply with M) */
void XWinrotateyM(matrix *M,float spin,int where)
   {
   static matrix N=C_1;

   float s,c;

   spin=spin/180.0*PI;
   s=fsin(spin);
   c=fcos(spin);
   N.M[0][0]=c;
   N.M[2][0]=s;
   N.M[0][2]=-s;
   N.M[2][2]=c;
   multiplyMbyN(M,&N,where);
   }

/* rotate around the z-axis (multiply with M) */
void XWinrotatezM(matrix *M,float spin,int where)
   {
   static matrix N=C_1;

   float s,c;

   spin=spin/180.0*PI;
   s=fsin(spin);
   c=fcos(spin);
   N.M[0][0]=c;
   N.M[1][0]=-s;
   N.M[0][1]=s;
   N.M[1][1]=c;
   multiplyMbyN(M,&N,where);
   }

/* translate by (dx,dy,dz) (multiply with M) */
void XWintranslateM(matrix *M,float dx,float dy,float dz,int where)
   {
   static matrix N=C_1;

   N.M[3][0]=dx;
   N.M[3][1]=dy;
   N.M[3][2]=dz;
   multiplyMbyN(M,&N,where);
   }

/* scale by sx/sy/sz (multiply with M) */
void XWinscaleM(matrix *M,float sx,float sy,float sz,int where)
   {
   static matrix N=C_1;

   N.M[0][0]=sx;
   N.M[1][1]=sy;
   N.M[2][2]=sz;
   multiplyMbyN(M,&N,where);
   }

/* multiply by M */
void XWinmultiply(matrix *M)
   {multiplyMbyN(&modelview,M,M_RIGHT); update();}

/* push modelview matrix */
void XWinpush()
   {
   if (++depth>=MAX_DEPTH) ERROR();
   modelviewstack[depth]=modelviewstack[depth-1];
   }

/* pop modelview matrix */
void XWinpop()
   {
   if (--depth<0) ERROR();
   update();
   }

/* begin display list */
void XWinbeginlist(int id,int pickable)
   {
   if (id<=PICK_NOID) ERROR();
   XWinpush();
   compiling=TRUE;
   }

/* end display list */
void XWinendlist()
   {
   compiling=FALSE;
   XWinpop();
   }

/* use display list */
void XWinuselist(int id)
   {if (id<=PICK_NOID) ERROR();}

/* enter pick mode */
void XWinbeginpicking(int x,int y)
   {picking=TRUE;}

/* leave pick mode */
int XWinendpicking()
   {
   picking=FALSE;
   return(PICK_NOID);
   }

/* set current color */
void XWinsetcolor(float red,float green,float blue)
   {
   Display *display=XtDisplay(Xarea);

   static XColor color;

   if (compiling || picking) return;

   /* modify the graphix context */
   color.red=red*0xFFFF;
   color.green=green*0xFFFF;
   color.blue=blue*0xFFFF;
   XAllocColor(display,DefaultColormapOfScreen(XtScreen(Xarea)),&color);
   XSetForeground(display,gc,color.pixel);
   }

/* load a texture map */
void XWinloadtexmap(char *filename,int width,int height,float scale,float offset)
   {}

/* draw 3D-line (we don't need to care about NURBS :-) */
void XWindrawline(float x1,float y1,float z1,
                  float x2,float y2,float z2)
   {
   vertex v1,v2;

   int outcode1=0,outcode2=0;

   if (compiling || picking) return;

   v1.v[0]=x1;
   v1.v[1]=y1;
   v1.v[2]=z1;
   v1.v[3]=1.0;
   v2.v[0]=x2;
   v2.v[1]=y2;
   v2.v[2]=z2;
   v2.v[3]=1.0;

   /* multiply vertices by combined transformation matrix */
   multiplyMbyv(&transformation,&v1);
   multiplyMbyv(&transformation,&v2);

   /* trivial reject */
   if (v1.v[2]<znear && v2.v[2]<znear) return;
   if (v1.v[2]>zfar && v2.v[2]>zfar) return;

   /* clip line against the near and far plane */
   if (v1.v[2]<znear) clip(v1,znear)
   else if (v1.v[2]>zfar) clip(v1,zfar);
   if (v2.v[2]<znear) clip(v2,znear)
   else if (v2.v[2]>zfar) clip(v2,zfar);

   /* perspective division */
   if (v1.v[3]==0.0 || v2.v[3]==0.0) ERROR();
   x1=v1.v[0]/v1.v[3];
   y1=v1.v[1]/v1.v[3];
   z1=v1.v[2]/v1.v[3];
   x2=v2.v[0]/v2.v[3];
   y2=v2.v[1]/v2.v[3];
   z2=v2.v[2]/v2.v[3];

   /* Cohen&Sutherland */

   if (x1<-1.0) outcode1+=LEFT; else
   if (x1>1.0) outcode1+=RIGHT;
   if (y1<-1.0) outcode1+=BOTTOM; else
   if (y1>1.0) outcode1+=TOP;
   if (x2<-1.0) outcode2+=LEFT; else
   if (x2>1.0) outcode2+=RIGHT;
   if (y2<-1.0) outcode2+=BOTTOM; else
   if (y2>1.0) outcode2+=TOP;

   /* trivial reject */
   if ((outcode1&outcode2)!=0) return;

   /* trivial accept */
   if (outcode1!=0 || outcode2!=0)
      {
      /* clip line against the normalized viewport */
      if (x1<-1.0) clipx2D(x1,y1,-1.0)
      else if (x1>1.0) clipx2D(x1,y1,1.0);
      if (y1<-1.0) clipy2D(x1,y1,-1.0)
      else if (y1>1.0) clipy2D(x1,y1,1.0);
      if (x1<-1.0 || x1>1.0) return;
      if (x2<-1.0) clipx2D(x2,y2,-1.0)
      else if (x2>1.0) clipx2D(x2,y2,1.0);
      if (y2<-1.0) clipy2D(x2,y2,-1.0)
      else if (y2>1.0) clipy2D(x2,y2,1.0);
      if (x2<-1.0 || x2>1.0) return;
      }

   /* map line onto viewport */
   x1=(x1*VIEWPORT_SIZE+1.0)/2.0*(viewportwidth-1);
   y1=(y1*VIEWPORT_SIZE+1.0)/2.0*(viewportheight-1);
   x2=(x2*VIEWPORT_SIZE+1.0)/2.0*(viewportwidth-1);
   y2=(y2*VIEWPORT_SIZE+1.0)/2.0*(viewportheight-1);
   drawline2D(x1,y1,x2,y2);
   }

/* draw 2D-line */
void drawline2D(int x1,int y1,int x2,int y2)
   {XDrawLine(XtDisplay(Xarea),XtWindow(Xarea),gc,
              x1,viewportheight-1-y1,x2,viewportheight-1-y2);}
