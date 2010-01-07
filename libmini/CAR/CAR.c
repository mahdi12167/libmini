#include "CARP.h"

/* (C)omputer (A)ided (R)acing */
int main(int argc,char *argv[])
   {
   char message[]="Computer Aided Racing by Stefan Roettger as of 30.June '97";

   if (argc<=1) openwindow(message,NULL);
   else openwindow(message,argv[1]);

   initlandscape();
   makelists();
   initscene();

   starteventloop(eventhandler,drawscene);

   return(0);
   }

/* handle one event */
void eventhandler(Event e,int x,int y,int button)
   {
   static int button1=UP,button2=UP,button3=UP,lx;

   switch (e)
      {
      case Epressed    : switch (button)
                            {
                            case 1 : button1=DOWN; break;
                            case 2 : button2=DOWN; break;
                            case 3 : button3=DOWN; break;
                            }
                         if (button2==DOWN)
                            {
                            accel=ACCEL;
                            brake=0.0;
                            }
                         if (button3==DOWN)
                            {
                            brake=BRAKE;
                            accel=0.0;
                            }
                         lx=x;
                         break;
      case Ereleased   : switch (button)
                            {
                            case 1 : button1=UP; turn=0.0; break;
                            case 2 : button2=UP; accel=0.0; break;
                            case 3 : button3=UP; brake=0.0; break;
                            }
                         break;
      case Emotion     : if (x<0) x=0;
                         else if (x>viewportwidth-1) x=viewportwidth-1;
                         if (button1==DOWN) turn+=SPIN;
                         lx=x;
                         break;
      case Eredraw     : break;
      case Equit       : closewindow(); exit(EXIT_SUCCESS);
      case Enone       : break;
      case Eflip       : if (toggle1==toggle2) toggle1=!toggle1;
                         else toggle2=!toggle2;
                         break;
      case Ewarp       : switch (planet)
                            {
                            case EARTH : planet=VENUS; break;
                            case VENUS : planet=MARS; break;
                            case MARS  : planet=MOON; break;
                            case MOON  : planet=EARTH; break;
                            }
                         initscene();
                         break;
      }
   }

/* initialize the scene */
void initscene()
   {
   switch (planet)
      {
      case EARTH : g=G;
                   r1=0.0; g1=0.7; b1=0.0;
                   r2=0.0; g2=0.2; b2=0.3;
                   noair=FALSE;
                   noice=FALSE;
                   break;
      case VENUS : g=G;
                   r1=1.0; g1=0.5; b1=0.0;
                   r2=0.0; g2=0.0; b2=0.1;
                   noair=FALSE;
                   noice=TRUE;
                   break;
      case MARS  : g=G/2.0;
                   r1=0.7; g1=0.0; b1=0.0;
                   r2=0.0; g2=0.5; b2=0.0;
                   noair=TRUE;
                   noice=FALSE;
                   break;
      case MOON  : g=G/6.0;
                   r1=0.7; g1=0.7; b1=0.5;
                   r2=0.5; g2=0.0; b2=0.0;
                   noair=TRUE;
                   noice=TRUE;
                   break;
      }

   turn=accel=0.0;
   brake=BRAKE;

   x0=z0=0.0;
   h=height(x0,z0);

   dy11=dy21=dy12=dy22=0.0;
   ry=rz=ux=uz=fx=fy=0.0;
   rx=uy=fz=1.0;
   fz=-fz;

   wreck=front=dir=speed=df=dh=0.0;

   px=XSTART;
   py=YSTART;
   pz=ZSTART;
   toggle1=toggle2=FALSE;
   }

/* draw the scene */
void drawscene()
   {
   float t,dt,
         si,co,
         dy1,dy2,
         dy,y,c,l,
         lh,ldh,ls,
         slowdown,ice,dd,
         dx1,dz1,dx2,dz2,
         y11,y21,y12,y22,
         ex,ey,ez;

   struct timeval t1,t2;

   matrix rot=M_1;

   int contact;

   if (turn<-MAX_TURN) turn=-MAX_TURN;
   else if (turn>MAX_TURN) turn=MAX_TURN;

   gettimeofday(&t1,NULL);

   clearwindow();
   perspective(FOVY,ASPECT,NEAR,FAR);

   si=fsin(dir/180.0*PI);
   co=-fcos(dir/180.0*PI);

   dy1=(dy11+dy22)/2.0;
   dy2=(dy21+dy12)/2.0;
   if (dy1>dy2) dy=dy1;
   else dy=dy2;

   y=h+dy;
   if (y<SAFETY-CAR_R-1.1) y=SAFETY-CAR_R-1.1;

   ex=x0-fx*DOFFSET;
   ey=y-fy*DOFFSET+HOFFSET;
   ez=z0-fz*DOFFSET;

   if (toggle1==toggle2)
      {
      lookat(px,py,pz,
             x0,y,z0,
             0.0,1.0,0.0);

      light(SUN);
      drawlandscape(res,
                    x0,0.0,z0,
                    px,py,pz,
                    x0-px,y-py,z0-pz,
                    0.0,1.0,0.0,
                    FOVY,ASPECT,
                    NEAR,FAR,
                    r1,g1,b1,
                    r2,g2,b2);
      }
   else if (toggle2)
      {
      float dx,dy,dz,
            ex,ey,ez;

      dx=-CAR_W/4.0;
      dy=CAR_R+1.1;
      dz=-CAR_L/4.0+0.1;

      ex=x0+dx*rx+dy*ux+dz*fx;
      ey=y+dx*ry+dy*uy+dz*fy;
      ez=z0+dx*rz+dy*uz+dz*fz;

      lookat(ex,ey,ez,
             ex+fx,ey+fy,ez+fz,
             ux,uy,uz);

      light(SUN);
      drawlandscape(res,
                    ex,0.0,ez,
                    ex,ey,ez,
                    fx,fy,fz,
                    ux,uy,uz,
                    FOVY,ASPECT,
                    NEAR,FAR,
                    r1,g1,b1,
                    r2,g2,b2);
      }
   else
      {
      si=fsin(front/180.0*PI);
      co=-fcos(front/180.0*PI);

      ex=x0-si*BOFFSET;
      ey=y+YOFFSET;
      ez=z0-co*BOFFSET;

      lookat(px,py,pz,
             x0,y,z0,
             si,1.0,co);

      light(SUN);
      drawlandscape(res,
                    x0,0.0,z0,
                    px,py,pz,
                    x0-px,y-py,z0-pz,
                    si,1.0,co,
                    FOVY,ASPECT,
                    NEAR,FAR,
                    r1,g1,b1,
                    r2,g2,b2);
      }

   heada2=turn/3.0;
   beginlist(head2,FALSE);
      rotatey(heada2);
   endlist();

   steera=turn;
   beginlist(steer,FALSE);
      rotatey(steera);
   endlist();

   leftarma1=5.0-((turn<0.0)?turn/7.0:0.0);
   beginlist(leftarm1,FALSE);
      rotatez(leftarma1);
   endlist();

   leftarma3=-60.0-((turn<MAX_TURN/3.0)?0.7*turn:0.7*MAX_TURN/3.0);
   beginlist(leftarm3,FALSE);
      rotatex(leftarma3);
   endlist();

   rightarma1=-5.0-((turn>0.0)?turn/7.0:0.0);
   beginlist(rightarm1,FALSE);
      rotatez(rightarma1);
   endlist();

   rightarma3=-60.0+((turn>-MAX_TURN/3.0)?0.7*turn:-0.7*MAX_TURN/3.0);
   beginlist(rightarm3,FALSE);
      rotatex(rightarma3);
   endlist();

   beginlist(drive,FALSE);
      rotatex(rpm);
   endlist();

   beginlist(red,FALSE);
      if (wreck==1.0 || brake==0.0) setcolor(0.3,0.0,0.0);
      else setcolor(1.0,0.0,0.0);
   endlist();

   rot.M[0][0]=rx;
   rot.M[0][1]=ry;
   rot.M[0][2]=rz;
   rot.M[1][0]=ux;
   rot.M[1][1]=uy;
   rot.M[1][2]=uz;
   rot.M[2][0]=fx;
   rot.M[2][1]=fy;
   rot.M[2][2]=fz;

   translate(x0,h+dy,z0);
   multiply(&rot);
   uselist(car);
   swapbuffers();

   gettimeofday(&t2,NULL);
   if ((t=t2.tv_sec-t1.tv_sec+(t2.tv_usec-t1.tv_usec)/1.0E6)==0.0) ERROR();

#ifdef VERBOSE
   printf("frames per second: %f, resolution: %f\n",1.0/t,res);
#endif

   res/=(t*FRAMERATE-1.0)*ADAPTION*t+1.0;
   if (res<1.0) res=1.0;
   else if (res>MAX_RES) res=MAX_RES;

   dt=t/ftrunc(t*STEPRATE+1.0);
   while (t>dt/2.0)
      {
      if (FOLLOW*dt>=1.0 || (!toggle1 && toggle2))
         {
         px=ex;
         py=ey;
         pz=ez;
         }
      else
         {
         px+=(ex-px)*FOLLOW*dt;
         py+=(ey-py)*FOLLOW*dt;
         pz+=(ez-pz)*FOLLOW*dt;

         if (toggle1==toggle2) y=height(px,pz)+HOFFSET;
         else y=height(px,pz)+YOFFSET;
         if (py<y) py=y;
         }

      si=fsin(dir/180.0*PI);
      co=-fcos(dir/180.0*PI);

      c=fsqrt(1.0-fy*fy);

      x0+=c*speed*si*dt;
      z0+=c*speed*co*dt;

      ldh=dh;
      dh*=dt;
      if (h>0.0) dh+=g*dt*dt/2.0;
      else dh+=SINK*dt;

      lh=h;
      h+=dh;
      if (contact=(h<=(y=height(x0,z0)))) h=y;
      else if (h-y<CAR_S) contact=TRUE;
      dh=(h-lh)/dt;

      c=dh-ldh;
      if (c<=MAX_SHOCK/2.0) c=0.0;
      else if (c>=MAX_SHOCK) c=1.0;
      else c=c/MAX_SHOCK*2.0-1.0;
      if ((wreck+=c/MAX_HIT)>1.0) wreck=1.0;

      speed*=1.0-c;
      dh*=1.0-c;
      df*=1.0-c;

      if (h<-CAR_R)
         {
         wreck=1.0;
         contact=TRUE;
         slowdown=BRAKE;
         }
      else if (h<0.0)
         {
         contact=TRUE;
         slowdown=-WATER*h/CAR_R;
         }
      else slowdown=-ACCEL*speed/MAX_SPEED*speed/MAX_SPEED/2.0;

      ls=speed;
      if (speed>0.0)
         {
         if (contact) speed+=(wreck*WATER+slowdown)*dt;
         if (!noair) speed+=slowdown*dt;
         if (speed<0.0) speed=0.0;
         }
      else
         {
         if (contact) speed-=(wreck*WATER+slowdown)*dt;
         if (!noair) speed-=slowdown*dt;
         if (speed>0.0) speed=0.0;
         }

      if (contact)
         {
         if (noice || h<0.0) ice=0.0;
         else ice=h/H/4.0;

         dd=(1.0-ice)*MAX_TURN/SLIDE*dt;
         if (front-dir>0.0)
            if (front-dir>180.0)
               if (dd<360.0-front+dir) dir-=dd;
               else dir=front;
            else
               if (dd<front-dir) dir+=dd;
               else dir=front;
         else
            if (dir-front>180.0)
               if (dd<360.0-dir+front) dir+=dd;
               else dir=front;
            else
               if (dd<dir-front) dir-=dd;
               else dir=front;
         normalize(dir);

         c=fcos((front-dir)/180.0*PI);
         df+=(1.0-ice)*(c*speed*turn/MAX_TURN*TURN-df)*DRIFT*dt;

         if (c<0.0) c=0.0;
         speed+=((1.0-wreck)*fsqrt(1.0-fy*fy)*(1.0-ice)*c*accel+fy*g)*dt;

         rpm+=c*ls*dt/CAR_R/PI*180.0;
         normalize(rpm);

         if (speed>0.0)
            {
            speed+=fsqrt(1.0-fy*fy)*(1.0-ice)*
                   ((1.0-wreck)*c*brake+fsqrt(1.0-c*c)/SLICK*BRAKE)*dt;
            if (speed<0.0) speed=0.0;
            }
         else
            {
            speed-=fsqrt(1.0-fy*fy)*(1.0-ice)*
                   ((1.0-wreck)*c*brake+fsqrt(1.0-c*c)/SLICK*BRAKE)*dt;
            if (speed>0.0) speed=0.0;
            }
         }

      if (ls==0.0) dh=fy*speed;
      else dh*=speed/ls;

      front+=df*dt;
      normalize(front);

      si=fsin(front/180.0*PI);
      co=-fcos(front/180.0*PI);

      c=fsqrt(1.0-ry*ry)*CAR_W/2.0;
      dx1=-c*co;
      dz1=c*si;
      dx2=c*co;
      dz2=-c*si;

      c=fsqrt(1.0-fy*fy)*CAR_L/2.0;
      dx1+=c*si;
      dz1+=c*co;
      dx2+=c*si;
      dz2+=c*co;

      y11=height(x0-dx1,z0-dz1);
      y21=height(x0-dx2,z0-dz2);
      y12=height(x0+dx2,z0+dz2);
      y22=height(x0+dx1,z0+dz1);

      if (h-dy22<y11)
         if (h-dy11<y22)
            {
            dy11=y11-h;
            dy22=y22-h;
            }
         else
            {
            dy11=y11-h;
            dy22=-dy11;
            if (h+dy22<y22) dy22=y22-h;
            }
      else
         if (h-dy11<y22)
            {
            dy22=y22-h;
            dy11=-dy22;
            if (h+dy11<y11) dy11=y11-h;
            }

      if (h-dy12<y21)
         if (h-dy21<y12)
            {
            dy21=y21-h;
            dy12=y12-h;
            }
         else
            {
            dy21=y21-h;
            dy12=-dy21;
            if (h+dy12<y12) dy12=y12-h;
            }
      else
         if (h-dy21<y12)
            {
            dy12=y12-h;
            dy21=-dy12;
            if (h+dy21<y21) dy21=y21-h;
            }

      rx=dx1-dx2;
      ry=(dy22+dy21-dy12-dy11)/2.0;
      rz=dz1-dz2;
      if ((l=fsqrt(rx*rx+ry*ry+rz*rz))==0.0) ERROR();
      rx/=l;
      ry/=l;
      rz/=l;

      fx=dx1+dx2;
      fy=(dy12+dy22-dy11-dy21)/2.0;
      fz=dz1+dz2;
      if ((l=fsqrt(fx*fx+fy*fy+fz*fz))==0.0) ERROR();
      fx/=l;
      fy/=l;
      fz/=l;

      ux=ry*fz-fy*rz;
      uy=rz*fx-fz*rx;
      uz=rx*fy-fx*ry;
      if ((l=fsqrt(ux*ux+uy*uy+uz*uz))==0.0) ERROR();
      ux/=l;
      uy/=l;
      uz/=l;

      rx=fy*uz-uy*fz;
      ry=fz*ux-uz*fx;
      rz=fx*uy-ux*fy;

      t-=dt;
      }
   }
