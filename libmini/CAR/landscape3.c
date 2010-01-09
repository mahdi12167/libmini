/*-------------------------------------------*/
/* Real-Time CLOD Rendering of Height Fields */
/*   by Stefan Roettger as of 13.March '97   */
/*-------------------------------------------*/

#include "landscape3P.h"

/* load the height field and preprocess it */
void initlandscape()
   {
   int i,j,k;

   float l,h,dh,b,
         dx1,dy1,dz1,
         dx2,dy2,dz2,
         dx3,dy3,dz3,
         dx4,dy4,dz4,
         dx5,dy5,dz5,
         dx6,dy6,dz6,
         ux,uy,uz;

   j=0;
   for (i=0; i<S; i++)
      {
      x[i]=&X[j];
      y[i]=&Y[j];
      z[i]=&Z[j];
#ifndef IMPACT
      nx[i]=&NX[j];
      ny[i]=&NY[j];
      nz[i]=&NZ[j];
      fb[i]=&FB[j];
#endif
      flag[i]=&FLAG[j];
      j+=S-i;
      }

   for (i=0; i<S; i++)
      for (j=0; j<S-i; j++)
         {
         x[i][j]=(i+(j-(S-1))/2.0)*D;
         z[i][j]=((S-1)/FSQRT3-j*FSQRT3)/2.0*D;
         }

   loadmap();

#ifndef IMPACT
   for (i=0; i<S; i++)
      for (j=0; j<S-i; j++)
         {
         if (i+j<S && j>0)
            {
            dx1=x[i][j]-x[i+1][j-1];
            dy1=y[i][j]-y[i+1][j-1];
            dz1=z[i][j]-z[i+1][j-1];

            if ((l=fsqrt(dx1*dx1+dy1*dy1+dz1*dz1))==0.0) ERROR();
            dx1/=l;
            dy1/=l;
            dz1/=l;
            }
         else
            {
            dx1=0.0;
            dy1=0.0;
            dz1=0.0;
            }

         if (i+j<S-1)
            {
            dx2=x[i][j]-x[i+1][j];
            dy2=y[i][j]-y[i+1][j];
            dz2=z[i][j]-z[i+1][j];

            if ((l=fsqrt(dx2*dx2+dy2*dy2+dz2*dz2))==0.0) ERROR();
            dx2/=l;
            dy2/=l;
            dz2/=l;
            }
         else
            {
            dx2=0.0;
            dy2=0.0;
            dz2=0.0;
            }

         if (i+j<S-1)
            {
            dx3=x[i][j]-x[i][j+1];
            dy3=y[i][j]-y[i][j+1];
            dz3=z[i][j]-z[i][j+1];

            if ((l=fsqrt(dx3*dx3+dy3*dy3+dz3*dz3))==0.0) ERROR();
            dx3/=l;
            dy3/=l;
            dz3/=l;
            }
         else
            {
            dx3=0.0;
            dy3=0.0;
            dz3=0.0;
            }

         if (i>0 && i+j<S)
            {
            dx4=x[i][j]-x[i-1][j+1];
            dy4=y[i][j]-y[i-1][j+1];
            dz4=z[i][j]-z[i-1][j+1];

            if ((l=fsqrt(dx4*dx4+dy4*dy4+dz4*dz4))==0.0) ERROR();
            dx4/=l;
            dy4/=l;
            dz4/=l;
            }
         else
            {
            dx4=0.0;
            dy4=0.0;
            dz4=0.0;
            }

         if (i>0)
            {
            dx5=x[i][j]-x[i-1][j];
            dy5=y[i][j]-y[i-1][j];
            dz5=z[i][j]-z[i-1][j];

            if ((l=fsqrt(dx5*dx5+dy5*dy5+dz5*dz5))==0.0) ERROR();
            dx5/=l;
            dy5/=l;
            dz5/=l;
            }
         else
            {
            dx5=0.0;
            dy5=0.0;
            dz5=0.0;
            }

         if (j>0)
            {
            dx6=x[i][j]-x[i][j-1];
            dy6=y[i][j]-y[i][j-1];
            dz6=z[i][j]-z[i][j-1];

            if ((l=fsqrt(dx6*dx6+dy6*dy6+dz6*dz6))==0.0) ERROR();
            dx6/=l;
            dy6/=l;
            dz6/=l;
            }
         else
            {
            dx6=0.0;
            dy6=0.0;
            dz6=0.0;
            }

         ux=dy6*dz1-dy1*dz6+dy2*dz3-dy3*dz2+dy4*dz5-dy5*dz4;
         uy=dz6*dx1-dz1*dx6+dz2*dx3-dz3*dx2+dz4*dx5-dz5*dx4;
         uz=dx6*dy1-dx1*dy6+dx2*dy3-dx3*dy2+dx4*dy5-dx5*dy4;

         if ((l=fsqrt(ux*ux+uy*uy+uz*uz))==0.0) ERROR();
         nx[i][j]=ux/l;
         ny[i][j]=uy/l;
         nz[i][j]=uz/l;
         }

   for (i=0; i<S; i++)
      for (j=0; j<S-i; j++)
         {
         k=j;
         h=y[i][k];
         fb[i][j]=1.0;
         while (--k>=0)
            {
            h+=D/2.0;
            dh=(j-k)*D/4.0;
            if (y[i][k]>h-dh)
               if (y[i][k]>=h+dh)
                  {
                  fb[i][j]=0.5;
                  break;
                  }
               else
                  {
                  b=0.75-(y[i][k]-h)/dh*0.25;
                  if (b<fb[i][j]) fb[i][j]=b;
                  }
            }
         }
#else
   loadtexmap(TEX_MAP_FILENAME,TEX_MAP_S,TEX_MAP_S,1.0/(S-1)/D,0.5);
#endif
   }

/* draw one part of the ocean */
void drawocean(int s,float far,
               float x1,float z1,
               float x2,float z2,
               float x3,float z3,
               float r,float g,float b)
   {
   float mx1,mz1,mx2,mz2,mx3,mz3;

   if (s>1 && far<s*D*O)
      {
      mx1=(x1+x2)/2.0;
      mz1=(z1+z2)/2.0;
      mx2=(x2+x3)/2.0;
      mz2=(z2+z3)/2.0;
      mx3=(x3+x1)/2.0;
      mz3=(z3+z1)/2.0;

      drawocean(s/2,far,x1,z1,mx1,mz1,mx3,mz3,r,g,b);
      drawocean(s/2,far,mx1,mz1,x2,z2,mx2,mz2,r,g,b);
      drawocean(s/2,far,mx3,mz3,mx2,mz2,x3,z3,r,g,b);

      drawocean(s/2,far,mx2,mz2,mx3,mz3,mx1,mz1,r,g,b);
      }
   else filltriangle(x1,0.0,z1,0.0,1.0,0.0,r,g,b,
                     x2,0.0,z2,0.0,1.0,0.0,r,g,b,
                     x3,0.0,z3,0.0,1.0,0.0,r,g,b);
   }

/* draw the island using an adaptive subdivision algorithm */
void drawisland(float c,
                float x0,float dy0,float z0,
                float r,float g,float b)
   {
   int q1,q2,
       i,j,s,s2;

   short int qi[Q],qj[Q],qs[Q];

   float y1,y2,y3,
         b1,b2,b3;

   if (c>1.0) c*=C;
   else c=C;

   memset(FLAG,0,sizeof(FLAG));

   q1=q2=0;
   qi[q2]=qj[q2]=0;
   qs[q2]=S-1;

   repeat
      i=qi[q1];
      j=qj[q1];
      s=qs[q1];
      s2=s/2;

      if (s2!=0)
         {
         if (flag[i][j]==0)
            if (flag[i+s][j]==0)
               if (flag[i][j+s]==0)
                  if (fsqr((x[i][j]+x[i+s][j]+x[i][j+s])/3.0-x0)+
                      fsqr((z[i][j]+z[i+s][j]+z[i][j+s])/3.0-z0)<
                      fsqr(s*D*c)-dy0)
                     {
                     if (++q2==Q) ERROR();
                     qi[q2]=i;
                     qj[q2]=j;
                     qs[q2]=s2;

                     if (++q2==Q) ERROR();
                     qi[q2]=i+s2;
                     qj[q2]=j;
                     qs[q2]=s2;

                     if (++q2==Q) ERROR();
                     qi[q2]=i;
                     qj[q2]=j+s2;
                     qs[q2]=s2;

                     if (++q2==Q) ERROR();
                     qi[q2]=i+s2;
                     qj[q2]=j+s2;
                     qs[q2]=-s2;

                     continue;
                     }

         flag[i+s2][j]=1;
         flag[i+s2][j+s2]=2;
         flag[i][j+s2]=3;
         }

      switch (flag[i][j])
         {
         case 0 : y1=y[i][j]; break;
         case 1 : y1=(y[i-s][j]+y[i+s][j])/2.0; break;
         case 2 : y1=(y[i+s][j-s]+y[i-s][j+s])/2.0; break;
         case 3 : y1=(y[i][j+s]+y[i][j-s])/2.0; break;
         }

      switch (flag[i+s][j])
         {
         case 0 : y2=y[i+s][j]; break;
         case 1 : y2=(y[i][j]+y[i+s+s][j])/2.0; break;
         case 2 : y2=(y[i+s+s][j-s]+y[i][j+s])/2.0; break;
         case 3 : y2=(y[i+s][j+s]+y[i+s][j-s])/2.0; break;
         }

      switch (flag[i][j+s])
         {
         case 0 : y3=y[i][j+s]; break;
         case 1 : y3=(y[i-s][j+s]+y[i+s][j+s])/2.0; break;
         case 2 : y3=(y[i+s][j]+y[i-s][j+s+s])/2.0; break;
         case 3 : y3=(y[i][j+s+s]+y[i][j])/2.0; break;
         }

#ifndef IMPACT
      b1=fb[i][j];
      b2=fb[i+s][j];
      b3=fb[i][j+s];

      filltriangle(x[i][j],y1,z[i][j],
                   nx[i][j],ny[i][j],nz[i][j],
                   r*b1,g*b1,b*b1,
                   x[i+s][j],y2,z[i+s][j],
                   nx[i+s][j],ny[i+s][j],nz[i+s][j],
                   r*b2,g*b2,b*b2,
                   x[i][j+s],y3,z[i][j+s],
                   nx[i][j+s],ny[i][j+s],nz[i][j+s],
                   r*b3,g*b3,b*b3);
#else
      texturetriangle(x[i][j],y1,z[i][j],
                      x[i+s][j],y2,z[i+s][j],
                      x[i][j+s],y3,z[i][j+s]);
#endif
   until (++q1>q2);
   }

/* draw the island and all 15 parts of the ocean */
void drawlandscape(float c,
                   float x0,float dy0,float z0,
                   float ex,float ey,float ez,
                   float dx,float dy,float dz,
                   float ux,float uy,float uz,
                   float fovy,float aspect,
                   float near,float far,
                   float r1,float g1,float b1,
                   float r2,float g2,float b2)
   {
   float ox,oz,px,pz,
         dx1,dz1,dx2,dz2;

   dx1=x[S-1][0]-x[0][0];
   dz1=z[S-1][0]-z[0][0];
   dx2=x[0][S-1]-x[0][0];
   dz2=z[0][S-1]-z[0][0];

   ox=x[0][0]-dx1-dx2;
   oz=z[0][0]-dz1-dz2;

   px=ox+0*dx1+0*dx2;
   pz=oz+0*dz1+0*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+1*dx1+0*dx2;
   pz=oz+1*dz1+0*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+2*dx1+0*dx2;
   pz=oz+2*dz1+0*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+3*dx1+0*dx2;
   pz=oz+3*dz1+0*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);

   px=ox+0*dx1+1*dx2;
   pz=oz+0*dz1+1*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+1*dx1+1*dx2;
   pz=oz+1*dz1+1*dz2;
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+2*dx1+1*dx2;
   pz=oz+2*dz1+1*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);

   px=ox+0*dx1+2*dx2;
   pz=oz+0*dz1+2*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);
   drawocean(S-1,far,px+dx1+dx2,pz+dz1+dz2,px+dx2,pz+dz2,px+dx1,pz+dz1,r2,g2,b2);

   px=ox+1*dx1+2*dx2;
   pz=oz+1*dz1+2*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);

   px=ox+0*dx1+3*dx2;
   pz=oz+0*dz1+3*dz2;
   drawocean(S-1,far,px,pz,px+dx1,pz+dz1,px+dx2,pz+dz2,r2,g2,b2);

   drawisland(c,x0,fabs(dy0),z0,r1,g1,b1);
   }

/* calculate the height at position (x0,z0) */
double height(double x0,double z0)
   {
   int i,j,k,s;

   double rx,rz,sx,sz,tx,tz,
          dst,dtr,drs,a;

   rx=x[0][0];
   rz=z[0][0];
   sx=x[S-1][0];
   sz=z[S-1][0];
   tx=x[0][S-1];
   tz=z[0][S-1];

   if ((a=crd(rx,rz,sx,sz,tx,tz))==0.0) ERROR();
   dst=crd(x0,z0,sx,sz,tx,tz)/a;
   dtr=crd(rx,rz,x0,z0,tx,tz)/a;
   drs=crd(rx,rz,sx,sz,x0,z0)/a;

   if (dst<=0.0 || dtr<=0.0 || drs<=0.0)
      return((((dst<=0.0)?dst:0.0)+
              ((dtr<=0.0)?dtr:0.0)+
              ((drs<=0.0)?drs:0.0))*H/2.0);

   i=trunc(dtr*(S-1));
   j=trunc(drs*(S-1));
   k=trunc(dst*(S-1));

   if (i+j>=S-2-k) s=1;
   else
      {
      i++;
      j++;
      s=-1;
      }

   rx=x[i][j];
   rz=z[i][j];
   sx=x[i+s][j];
   sz=z[i+s][j];
   tx=x[i][j+s];
   tz=z[i][j+s];

   if ((a=crd(rx,rz,sx,sz,tx,tz))==0.0) ERROR();
   dst=crd(x0,z0,sx,sz,tx,tz)/a;
   dtr=crd(rx,rz,x0,z0,tx,tz)/a;
   drs=crd(rx,rz,sx,sz,x0,z0)/a;

   return(dst*y[i][j]+dtr*y[i+s][j]+drs*y[i][j+s]);
   }

double dheight(double x0,double y0,double z0,double smoothing)
   {
   static double dh=0.0;

   if (smoothing<1.0) smoothing=1.0;
   else smoothing=1.0/smoothing;
   return(dh=smoothing*abs(height(x0,z0)-y0)+(1.0-smoothing)*dh);
   }

/* height field loader: */

void loadmap()
   {
   FILE *infile;

   if ((infile=fopen(MAP_FILENAME,"r"))==NULL) ERROR();
   if (fread(&map,sizeof(map),1,infile)!=1) ERROR();
   fclose(infile);

   if (*((unsigned char *)(&INTEL_CHECK))!=0) swapmap();

   convertmap();
   scalemap();
   }

void swapmap()
   {
   int i;

   unsigned char *ptr,c1,c2;

   ptr=(unsigned char *)map;

   for (i=0; i<MAP_S*MAP_S; i++)
      {
      c1=*ptr;
      c2=*(ptr+1);
      *ptr++=c2;
      *ptr++=c1;
      }
   }

void convertmap()
   {
   int i,j,
       mi,mj;

   float pi,pj,
         ri,rj;

   for (i=0; i<S; i++)
      for (j=0; j<S-i; j++)
         {
         pi=(MAP_S-1)*MAP_D/2.0+x[i][j];
         pj=(MAP_S-1)*MAP_D*(0.5+FSQRT3/12.0)+z[i][j];

         mi=(int)ftrunc(pi/MAP_D);
         mj=(int)ftrunc(pj/MAP_D);

         ri=pi/MAP_D-mi;
         rj=pj/MAP_D-mj;

         if (mi==MAP_S-1)
            {
            mi--;
            ri=1.0;
            }

         if (mj==MAP_S-1)
            {
            mj--;
            rj=1.0;
            }

         y[i][j]=((1.0-rj)*((1.0-ri)*map[mj][mi]+
                                  ri*map[mj][mi+1])+
                        rj*((1.0-ri)*map[mj+1][mi]+
                                  ri*map[mj+1][mi+1]))*MAP_SCALE;
         }
   }

void scalemap()
   {
   int i,j;

   float c;

   for (i=0; i<S/2; i++)
      for (j=0; j<S-1-3*i; j++)
         {
         c=1.0-pow(1.0-(float)i/(S/2-1),10);
         y[j+i][i]*=c;
         y[S-1-j-2*i][j+i]*=c;
         y[i][S-1-j-2*i]*=c;
         }

   H=0.0;
   for (i=0; i<S; i++)
      for (j=0; j<S-i; j++)
         if (y[i][j]>H) H=y[i][j];
   }
