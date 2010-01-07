/*-------------------------------------------*/
/* Real-Time CLOD Rendering of Height Fields */
/*   by Stefan Roettger as of 13.March '97   */
/*     updated version as of 30.June '97     */
/*-------------------------------------------*/

#include "landscape4P.h"

/* load the height field and preprocess it */
void initlandscape()
   {
   int s,i,j,
       m,n;

   float f,f2,
         dh,mh;

   loadmap();

   for (s=2; s<S; s*=2)
      {
      for (i=0; i<S; i+=s)
         for (j=0; j<S; j+=s) fc[i][j]=0;

      for (i=s/2; i<S; i+=s)
         for (j=s/2; j<S; j+=s)
            {
            f=fabs(Y(i-s/2,j-s/2)+Y(i+s/2,j-s/2)-2.0f*Y(i,j-s/2));
            f=fmax(f,fabs(Y(i-s/2,j+s/2)+Y(i+s/2,j+s/2)-2.0f*Y(i,j+s/2)));
            f=fmax(f,fabs(Y(i-s/2,j-s/2)+Y(i-s/2,j+s/2)-2.0f*Y(i-s/2,j)));
            f=fmax(f,fabs(Y(i+s/2,j-s/2)+Y(i+s/2,j+s/2)-2.0f*Y(i+s/2,j)));
            if (((i+j)/s)%2==1) f=fmax(f,fabs(Y(i-s/2,j-s/2)+Y(i+s/2,j+s/2)-2.0f*Y(i,j))*FSQRT2);
            else f=fmax(f,fabs(Y(i-s/2,j+s/2)+Y(i+s/2,j-s/2)-2.0f*Y(i,j))*FSQRT2);

            f/=s*D*M;

            if (s>2)
               {
               f2=dcpr(fc[i+s/2][j]);
               f2=fmax(f2,dcpr(fc[i][j+s/2]));
               f2=fmax(f2,dcpr(fc[i-s/2][j]));
               f2=fmax(f2,dcpr(fc[i][j-s/2]));

               f=fmax(f,f2*K);
               }

            if ((fc[i][j]=cpr(f))>0)
               while (dcpr(fc[i][j])<=f && fc[i][j]<255) fc[i][j]++;

            if (fc[i][j]>fc[i+s/2][j+s/2]) fc[i+s/2][j+s/2]=fc[i][j];
            if (fc[i][j]>fc[i-s/2][j+s/2]) fc[i-s/2][j+s/2]=fc[i][j];
            if (fc[i][j]>fc[i-s/2][j-s/2]) fc[i-s/2][j-s/2]=fc[i][j];
            if (fc[i][j]>fc[i+s/2][j-s/2]) fc[i+s/2][j-s/2]=fc[i][j];
            }
      }

   for (s=max(S/N,2); s<S; s*=2)
      {
      DH[s]=0.0f;
      for (i=s/2; i<S; i+=s)
         for (j=s/2; j<S; j+=s)
            {
            mh=Y(i,j);
            for (m=-s/2; m<=s/2; m++)
               for (n=-s/2; n<=s/2; n++)
                  if ((dh=fabs(Y(i+m,j+n)-mh))>DH[s]) DH[s]=dh;
            }
      }

   loadtexmap(TEX_MAP_FILENAME,TEX_MAP_S,TEX_MAP_S,1.0f/(S-1)/D,0.5f);
   }

/* triangulate the height field using an adaptive subdivision algorithm */
void calcmap(int i,int j,int s)
   {
   float f;

   int s2,s4;

   f=(fabs(X(i)-X0)+fabs(Z(j)-Z0)+DY0)/
     (s*D*C*fmax(c*dcpr(fc[i][j]),1.0f));

   if (f<1.0f)
      {
      if (f<=0.5f) bc[i][j]=255;
      else bc[i][j]=510-ftrc(510.0f*f);

      s2=s/2;
      s4=s/4;

      if (s4>0)
         {
         if (s>=S/N)
            {
            if (DX*(X(i)-EX)+DY*(Y(i,j)-EY)+DZ*(Z(j)-EZ)>PLANE+3.0f*(k1*s2+k2*DH[s])) return;
            if (nx1*(X(i)-EX)+ny1*(Y(i,j)-EY)+nz1*(Z(j)-EZ)>3.0f*(k11*s2+k12*DH[s])) return;
            if (nx2*(X(i)-EX)+ny2*(Y(i,j)-EY)+nz2*(Z(j)-EZ)>3.0f*(k21*s2+k22*DH[s])) return;
            if (nx3*(X(i)-EX)+ny3*(Y(i,j)-EY)+nz3*(Z(j)-EZ)>3.0f*(k31*s2+k32*DH[s])) return;
            if (nx4*(X(i)-EX)+ny4*(Y(i,j)-EY)+nz4*(Z(j)-EZ)>3.0f*(k41*s2+k42*DH[s])) return;
            }

         calcmap(i+s4,j+s4,s2);
         calcmap(i-s4,j+s4,s2);
         calcmap(i-s4,j-s4,s2);
         calcmap(i+s4,j-s4,s2);
         }
      }
   else bc[i][j]=0;
   }

/* draw the triangulated height field using triangle fans */
void drawmap(int i,int j,int s,float m0,float e1,float e2,float e3,float e4)
   {
   int s2,s4;

   float x1,x2,x3,
         z1,z2,z3,
         m1,m2,m3,m4;

   int state,last;

   s2=s/2;
   s4=s/4;

   if (s>=S/N)
      {
      if (DX*(X(i)-EX)+DY*(Y(i,j)-EY)+DZ*(Z(j)-EZ)>PLANE+k1*s2+k2*DH[s]) return;
      if (nx1*(X(i)-EX)+ny1*(Y(i,j)-EY)+nz1*(Z(j)-EZ)>k11*s2+k12*DH[s]) return;
      if (nx2*(X(i)-EX)+ny2*(Y(i,j)-EY)+nz2*(Z(j)-EZ)>k21*s2+k22*DH[s]) return;
      if (nx3*(X(i)-EX)+ny3*(Y(i,j)-EY)+nz3*(Z(j)-EZ)>k31*s2+k32*DH[s]) return;
      if (nx4*(X(i)-EX)+ny4*(Y(i,j)-EY)+nz4*(Z(j)-EZ)>k41*s2+k42*DH[s]) return;
      }

   x1=X(i-s2);
   x2=X(i);
   x3=X(i+s2);

   z1=Z(j-s2);
   z2=Z(j);
   z3=Z(j+s2);

   m1=blendMV(i+s2,j,s2,e4,e1);
   m2=blendMH(i,j+s2,s2,e1,e2);
   m3=blendMV(i-s2,j,s2,e2,e3);
   m4=blendMH(i,j-s2,s2,e3,e4);

   state=FALSE;
   if (s4>0)
      if (bc[i+s4][j+s4]!=0)
         {
         drawmap(i+s4,j+s4,s2,blendED(i+s4,j+s4,m0,e1),e1,m2,m0,m1);
         state=last=TRUE;
         }
   if (!state)
      {
      beginfan();
      fanvertex(x2,m0,z2);
      if (m1!=MAXFLOAT) fanvertex(x3,m1,z2);
      fanvertex(x3,e1,z3);
      if (m2!=MAXFLOAT) fanvertex(x2,m2,z3);
      last=FALSE;
      }

   state=FALSE;
   if (s4>0)
      if (bc[i-s4][j+s4]!=0)
         {
         drawmap(i-s4,j+s4,s2,blendED(i-s4,j+s4,m0,e2),m2,e2,m3,m0);
         state=last=TRUE;
         }
   if (!state)
      {
      if (last)
         {
         beginfan();
         fanvertex(x2,m0,z2);
         fanvertex(x2,m2,z3);
         }
      fanvertex(x1,e2,z3);
      if (m3!=MAXFLOAT) fanvertex(x1,m3,z2);
      last=FALSE;
      }

   state=FALSE;
   if (s4>0)
      if (bc[i-s4][j-s4]!=0)
         {
         drawmap(i-s4,j-s4,s2,blendED(i-s4,j-s4,m0,e3),m0,m3,e3,m4);
         state=last=TRUE;
         }
   if (!state)
      {
      if (last)
         {
         beginfan();
         fanvertex(x2,m0,z2);
         fanvertex(x1,m3,z2);
         }
      fanvertex(x1,e3,z1);
      if (m4!=MAXFLOAT) fanvertex(x2,m4,z1);
      last=FALSE;
      }

   state=FALSE;
   if (s4>0)
      if (bc[i+s4][j-s4]!=0)
         {
         drawmap(i+s4,j-s4,s2,blendED(i+s4,j-s4,m0,e4),m1,m0,m4,e4);
         state=TRUE;
         }
   if (!state)
      {
      if (last)
         {
         beginfan();
         fanvertex(x2,m0,z2);
         fanvertex(x2,m4,z1);
         }
      fanvertex(x3,e4,z1);
      if (m1!=MAXFLOAT) fanvertex(x3,m1,z2);
      else fanvertex(x3,e1,z3);
      }
   }

/* draw the landscape */
void drawlandscape(float res,
                   float x0,float dy0,float z0,
                   float ex,float ey,float ez,
                   float dx,float dy,float dz,
                   float ux,float uy,float uz,
                   float fovy,float aspect,
                   float near,float far,
                   float r1,float g1,float b1,
                   float r2,float g2,float b2)
   {
   float length,
         rx,ry,rz;

   if ((length=fsqrt(dx*dx+dy*dy+dz*dz))==0.0f) ERROR();
   dx/=length;
   dy/=length;
   dz/=length;

   c=res;

   X0=x0;
   DY0=fabs(dy0);
   Z0=z0;

   EX=ex;
   EY=ey;
   EZ=ez;

   DX=dx;
   DY=dy;
   DZ=dz;

   PLANE=far;

   k1=(fabs(dx)+fabs(dz))*D;
   k2=fabs(dy);

   rx=uy*dz-dy*uz;
   ry=uz*dx-dz*ux;
   rz=ux*dy-dx*uy;

   ux=ry*dz-dy*rz;
   uy=rz*dx-dz*rx;
   uz=rx*dy-dx*ry;

   if ((length=fsqrt(rx*rx+ry*ry+rz*rz)/ftan(fovy/360.0f*PI)/aspect)==0.0f) ERROR();
   rx/=length;
   ry/=length;
   rz/=length;

   if ((length=fsqrt(ux*ux+uy*uy+uz*uz)/ftan(fovy/360.0f*PI))==0.0f) ERROR();
   ux/=length;
   uy/=length;
   uz/=length;

   nx1=(dy+ry-uy)*(dz+rz+uz)-(dy+ry+uy)*(dz+rz-uz);
   ny1=(dz+rz-uz)*(dx+rx+ux)-(dz+rz+uz)*(dx+rx-ux);
   nz1=(dx+rx-ux)*(dy+ry+uy)-(dx+rx+ux)*(dy+ry-uy);

   if ((length=fsqrt(nx1*nx1+ny1*ny1+nz1*nz1))==0.0f) ERROR();
   nx1/=length;
   ny1/=length;
   nz1/=length;

   k11=(fabs(nx1)+fabs(nz1))*D;
   k12=fabs(ny1);

   nx2=(dy-ry+uy)*(dz-rz-uz)-(dy-ry-uy)*(dz-rz+uz);
   ny2=(dz-rz+uz)*(dx-rx-ux)-(dz-rz-uz)*(dx-rx+ux);
   nz2=(dx-rx+ux)*(dy-ry-uy)-(dx-rx-ux)*(dy-ry+uy);

   if ((length=fsqrt(nx2*nx2+ny2*ny2+nz2*nz2))==0.0f) ERROR();
   nx2/=length;
   ny2/=length;
   nz2/=length;

   k21=(fabs(nx2)+fabs(nz2))*D;
   k22=fabs(ny2);

   nx3=(dy-ry-uy)*(dz+rz-uz)-(dy+ry-uy)*(dz-rz-uz);
   ny3=(dz-rz-uz)*(dx+rx-ux)-(dz+rz-uz)*(dx-rx-ux);
   nz3=(dx-rx-ux)*(dy+ry-uy)-(dx+rx-ux)*(dy-ry-uy);

   if ((length=fsqrt(nx3*nx3+ny3*ny3+nz3*nz3))==0.0f) ERROR();
   nx3/=length;
   ny3/=length;
   nz3/=length;

   k31=(fabs(nx3)+fabs(nz3))*D;
   k32=fabs(ny3);

   nx4=(dy+ry+uy)*(dz-rz+uz)-(dy-ry+uy)*(dz+rz+uz);
   ny4=(dz+rz+uz)*(dx-rx+ux)-(dz-rz+uz)*(dx+rx+ux);
   nz4=(dx+rx+ux)*(dy-ry+uy)-(dx-rx+ux)*(dy+ry+uy);

   if ((length=fsqrt(nx4*nx4+ny4*ny4+nz4*nz4))==0.0f) ERROR();
   nx4/=length;
   ny4/=length;
   nz4/=length;

   k41=(fabs(nx4)+fabs(nz4))*D;
   k42=fabs(ny4);

   calcmap(S/2,S/2,S-1);
   drawmap(S/2,S/2,S-1,
           blendED(S/2,S/2,Y(S-1,S-1),Y(0,0)),
           Y(S-1,S-1),Y(0,S-1),Y(0,0),Y(0,S-1));
   }

/* calculate the height at position (x0,z0) */
double height(double x0,double z0)
   {
   int mi,mj;

   double ri,rj;

   x0-=X(0);
   z0-=Z(S-1);
   if (x0<=0.0 || z0<=0.0) return(0.0);

   mi=ftrc(x0/D);
   mj=ftrc(z0/D);
   if (mi>=S-1 || mj>=S-1) return(0.0);

   ri=x0/D-mi;
   rj=z0/D-mj;

   if ((mi+mj)%2==0)
      if (rj<ri) return(Y(mi,S-1-mj)+
                        ri*(Y(mi+1,S-1-mj)-Y(mi,S-1-mj))+
                        rj*(Y(mi+1,S-1-mj-1)-Y(mi+1,S-1-mj)));
      else return(Y(mi,S-1-mj)+
                  rj*(Y(mi,S-1-mj-1)-Y(mi,S-1-mj))+
                  ri*(Y(mi+1,S-1-mj-1)-Y(mi,S-1-mj-1)));
   else
      if (ri+rj<1.0) return(Y(mi,S-1-mj)+
                            ri*(Y(mi+1,S-1-mj)-Y(mi,S-1-mj))+
                            rj*(Y(mi,S-1-mj-1)-Y(mi,S-1-mj)));
      else return(Y(mi+1,S-1-mj-1)+
                  (1.0-ri)*(Y(mi,S-1-mj-1)-Y(mi+1,S-1-mj-1))+
                  (1.0-rj)*(Y(mi+1,S-1-mj)-Y(mi+1,S-1-mj-1)));
   }

/* smooth global distance to the ground */
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
   int i;

   FILE *infile;

   if ((map=malloc(MAP_S*MAP_S*2))==NULL) ERROR();
   if ((tab=malloc(MAP_S*sizeof(short int *)))==NULL) ERROR();
   for (i=0; i<MAP_S; i++) tab[i]=&map[i*MAP_S];

   if ((infile=fopen(MAP_FILENAME,"r"))==NULL) ERROR();
   if (fread(map,MAP_S*MAP_S*2,1,infile)!=1) ERROR();
   fclose(infile);

   convertmap();

   free(tab);
   free(map);
   }

void convertmap()
   {
   int i,j,
       mi,mj;

   float pi,pj,
         ri,rj;

   H=0.0f;
   for (i=0; i<S; i++)
      for (j=0; j<S; j++)
         {
         pi=(MAP_S-1)*(float)MAP_D/2.0f+X(i);
         pj=(MAP_S-1)*(float)MAP_D/2.0f+Z(j);

         mi=ftrc(pi/(float)MAP_D);
         mj=ftrc(pj/(float)MAP_D);

         ri=pi/(float)MAP_D-mi;
         rj=pj/(float)MAP_D-mj;

         if (mi==MAP_S-1)
            {
            mi--;
            ri=1.0f;
            }

         if (mj==MAP_S-1)
            {
            mj--;
            rj=1.0f;
            }

         y[i][j]=h(((1.0f-rj)*((1.0f-ri)*tab[mj][mi]+
                                      ri*tab[mj][mi+1])+
                           rj*((1.0f-ri)*tab[mj+1][mi]+
                                      ri*tab[mj+1][mi+1]))*(float)MAP_SCALE);

         if (Y(i,j)>H) H=Y(i,j);
         }
   }
