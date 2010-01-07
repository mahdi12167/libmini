#include "driverP.h"

void error(char *message)
   {
   fprintf(stderr,"%s!\n",message);
   exit(EXIT_FAILURE);
   }

void openwindow(char *title,char *driver)
   {
   if (driver!=NULL)
      if ((output=toupper(*driver++))=='-') output=toupper(*driver);
   switch (output)
      {
      case 'X' : XWinopenwindow(INITIAL_WIDTH,INITIAL_HEIGHT,title); break;
      case 'O' : OGLopenwindow(INITIAL_WIDTH,INITIAL_HEIGHT,title); break;
      default  : error("bad graphix driver specification");
      }
   perspective(FOVY,ASPECT,NEAR,FAR);
   light(SUN);
   setcolor(BLACK);
   clearwindow();
   swapbuffers();
   }

void starteventloop(eventhandlertype *eventhandler,backgroundtype *background)
   {
   switch (output)
      {
      case 'X' :
      case 'O' : XWinstarteventloop(eventhandler,background); break;
      default  : ERROR();
      }
   }

void clearwindow()
   {
   switch (output)
      {
      case 'X' : XWinclearwindow(); break;
      case 'O' : OGLclearwindow(); break;
      default  : ERROR();
      }
   }

void swapbuffers()
   {
   switch (output)
      {
      case 'X' : XWinflushbuffer(); break;
      case 'O' : OGLswapbuffers(); break;
      default  : ERROR();
      }
   }

void closewindow()
   {
   switch (output)
      {
      case 'X' : XWinclosewindow(); break;
      case 'O' : OGLclosewindow(); break;
      default  : ERROR();
      }
   }

void perspective(float fovy,float aspect,float near,float far)
   {
   switch (output)
      {
      case 'X' : XWinperspective(fovy,aspect,near,far); break;
      case 'O' : OGLperspective(fovy,aspect,near,far); break;
      default  : ERROR();
      }
   }

void lookat(float eyex,float eyey,float eyez,
            float centerx,float centery,float centerz,
            float upx,float upy,float upz)
   {
   switch (output)
      {
      case 'X' : XWinlookat(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz); break;
      case 'O' : OGLlookat(eyex,eyey,eyez,centerx,centery,centerz,upx,upy,upz); break;
      default  : ERROR();
      }
   }

void rotatex(float spin)
   {
   switch (output)
      {
      case 'X' : XWinrotatex(spin); break;
      case 'O' : OGLrotatex(spin); break;
      default  : ERROR();
      }
   }

void rotatey(float spin)
   {
   switch (output)
      {
      case 'X' : XWinrotatey(spin); break;
      case 'O' : OGLrotatey(spin); break;
      default  : ERROR();
      }
   }

void rotatez(float spin)
   {
   switch (output)
      {
      case 'X' : XWinrotatez(spin); break;
      case 'O' : OGLrotatez(spin); break;
      default  : ERROR();
      }
   }

void translate(float dx,float dy,float dz)
   {
   switch (output)
      {
      case 'X' : XWintranslate(dx,dy,dz); break;
      case 'O' : OGLtranslate(dx,dy,dz); break;
      default  : ERROR();
      }
   }

void scale(float sx,float sy,float sz)
   {
   switch (output)
      {
      case 'X' : XWinscale(sx,sy,sz); break;
      case 'O' : OGLscale(sx,sy,sz); break;
      default  : ERROR();
      }
   }

void rotatexM(matrix *M,float spin,int where)
   {
   switch (output)
      {
      case 'X' : XWinrotatexM(M,spin,where); break;
      case 'O' : OGLrotatexM(M,spin,where); break;
      default  : ERROR();
      }
   }

void rotateyM(matrix *M,float spin,int where)
   {
   switch (output)
      {
      case 'X' : XWinrotateyM(M,spin,where); break;
      case 'O' : OGLrotateyM(M,spin,where); break;
      default  : ERROR();
      }
   }

void rotatezM(matrix *M,float spin,int where)
   {
   switch (output)
      {
      case 'X' : XWinrotatezM(M,spin,where); break;
      case 'O' : OGLrotatezM(M,spin,where); break;
      default  : ERROR();
      }
   }

void translateM(matrix *M,float dx,float dy,float dz,int where)
   {
   switch (output)
      {
      case 'X' : XWintranslateM(M,dx,dy,dz,where); break;
      case 'O' : OGLtranslateM(M,dx,dy,dz,where); break;
      default  : ERROR();
      }
   }

void scaleM(matrix *M,float sx,float sy,float sz,int where)
   {
   switch (output)
      {
      case 'X' : XWinscaleM(M,sx,sy,sz,where); break;
      case 'O' : OGLscaleM(M,sx,sy,sz,where); break;
      default  : ERROR();
      }
   }

void multiply(matrix *M)
   {
   switch (output)
      {
      case 'X' : XWinmultiply(M); break;
      case 'O' : OGLmultiply(M); break;
      default  : ERROR();
      }
   }

void push()
   {
   switch (output)
      {
      case 'X' : XWinpush(); break;
      case 'O' : OGLpush(); break;
      default  : ERROR();
      }
   }

void pop()
   {
   switch (output)
      {
      case 'X' : XWinpop(); break;
      case 'O' : OGLpop(); break;
      default  : ERROR();
      }
   }

void beginlist(int id,int pickable)
   {
   switch (output)
      {
      case 'X' : XWinbeginlist(id,pickable); break;
      case 'O' : OGLbeginlist(id,pickable); break;
      default  : ERROR();
      }
   }

void endlist()
   {
   switch (output)
      {
      case 'X' : XWinendlist(); break;
      case 'O' : OGLendlist(); break;
      default  : ERROR();
      }
   }

void uselist(int id)
   {
   switch (output)
      {
      case 'X' : XWinuselist(id); break;
      case 'O' : OGLuselist(id); break;
      default  : ERROR();
      }
   }

void beginpicking(int x,int y)
   {
   switch (output)
      {
      case 'X' : XWinbeginpicking(x,y); break;
      case 'O' : OGLbeginpicking(x,y); break;
      default  : ERROR();
      }
   }

int endpicking()
   {
   switch (output)
      {
      case 'X' : return(XWinendpicking());
      case 'O' : return(OGLendpicking());
      default  : ERROR();
      }
   }

void light(float dx,float dy,float dz)
   {
   switch (output)
      {
      case 'X' : break;
      case 'O' : OGLlight(dx,dy,dz); break;
      default  : ERROR();
      }
   }

void setcolor(float red,float green,float blue)
   {
   switch (output)
      {
      case 'X' : XWinsetcolor(red,green,blue); break;
      case 'O' : OGLsetcolor(red,green,blue); break;
      default  : ERROR();
      }
   }

void loadtexmap(char *filename,int width,int height,float scale,float offset)
   {
   switch (output)
      {
      case 'X' : XWinloadtexmap(filename,width,height,scale,offset); break;
      case 'O' : OGLloadtexmap(filename,width,height,scale,offset); break;
      default  : ERROR();
      }
   }

void drawline(float x1,float y1,float z1,
              float x2,float y2,float z2)
   {
   switch (output)
      {
      case 'X' : XWindrawline(x1,y1,z1,x2,y2,z2); break;
      case 'O' : OGLdrawline(x1,y1,z1,x2,y2,z2); break;
      default  : ERROR();
      }
   }

void drawtriangle(float x1,float y1,float z1,
                  float x2,float y2,float z2,
                  float x3,float y3,float z3)
   {
   drawline(x1,y1,z1,x2,y2,z2);
   drawline(x2,y2,z2,x3,y3,z3);
   drawline(x3,y3,z3,x1,y1,z1);
   }

void filltriangle(float x1,float y1,float z1,
                  float nx1,float ny1,float nz1,
                  float r1,float g1,float b1,
                  float x2,float y2,float z2,
                  float nx2,float ny2,float nz2,
                  float r2,float g2,float b2,
                  float x3,float y3,float z3,
                  float nx3,float ny3,float nz3,
                  float r3,float g3,float b3)
   {
   switch (output)
      {
      case 'X' : drawtriangle(x1,y1,z1,x2,y2,z2,x3,y3,z3); break;
      case 'O' : OGLfilltriangle(x1,y1,z1,nx1,ny1,nz1,r1,g1,b1,
                                 x2,y2,z2,nx2,ny2,nz2,r2,g2,b2,
                                 x3,y3,z3,nx3,ny3,nz3,r3,g3,b3); break;
      default  : ERROR();
      }
   }

void texturetriangle(float x1,float y1,float z1,
                     float x2,float y2,float z2,
                     float x3,float y3,float z3)
   {
   switch (output)
      {
      case 'X' : drawtriangle(x1,y1,z1,x2,y2,z2,x3,y3,z3); break;
      case 'O' : OGLtexturetriangle(x1,y1,z1,x2,y2,z2,x3,y3,z3); break;
      default  : ERROR();
      }
   }

void drawrectangle(float x,float y,float z,
                   float dx1,float dy1,float dz1,
                   float dx2,float dy2,float dz2)
   {
   drawline(x,y,z,x+dx1,y+dy1,z+dz1);
   drawline(x+dx1,y+dy1,z+dz1,x+dx1+dx2,y+dy1+dy2,z+dz1+dz2);
   drawline(x+dx1+dx2,y+dy1+dy2,z+dz1+dz2,x+dx2,y+dy2,z+dz2);
   drawline(x+dx2,y+dy2,z+dz2,x,y,z);
   }

void fillrectangle(float x,float y,float z,
                   float dx1,float dy1,float dz1,
                   float dx2,float dy2,float dz2)
   {
   switch (output)
      {
      case 'X' : drawrectangle(x,y,z,dx1,dy1,dz1,dx2,dy2,dz2); break;
      case 'O' : OGLfillrectangle(x,y,z,dx1,dy1,dz1,dx2,dy2,dz2); break;
      default  : ERROR();
      }
   }

void texturerectangle(float x1,float y1,float z1,
                      float x2,float y2,float z2,
                      float x3,float y3,float z3,
                      float x4,float y4,float z4)
   {
   switch (output)
      {
      case 'X' : XWindrawline(x1,y1,z1,x2,y2,z2);
                 XWindrawline(x2,y2,z2,x3,y3,z3);
                 XWindrawline(x3,y3,z3,x4,y4,z4);
                 XWindrawline(x4,y4,z4,x1,y1,z1);
                 break;
      case 'O' : OGLtexturerectangle(x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4); break;
      default  : ERROR();
      }
   }

void beginfan()
   {
   switch (output)
      {
      case 'X' : fanstate=0; break;
      case 'O' : OGLbeginfan(); break;
      default  : ERROR();
      }
   }

void fanvertex(float x,float y,float z)
   {
   switch (output)
      {
      case 'X' : switch (++fanstate)
                    {
                    case 1  : mx=x; my=y; mz=z; break;
                    case 2  : lx=x; ly=y; lz=z; break;
                    default : drawtriangle(mx,my,mz,lx,ly,lz,x,y,z);
                              lx=x; ly=y; lz=z;
                              break;
                    }
                 break;
      case 'O' : OGLfanvertex(x,y,z); break;
      default  : ERROR();
      }
   }

/* compute v:=M*v */
void multiplyMbyv(matrix *M,vertex *v)
   {
   int j,k;

   vertex w=v_0;

   /* matrix by vertex multiplication */
   for (j=0; j<4; j++)
      for (k=0; k<4; k++) w.v[j]+=M->M[k][j]*v->v[k];

   *v=w;
   }

/* compute M:=M*N (M_RIGHT) or M:=N*M (M_LEFT) */
void multiplyMbyN(matrix *M,matrix *N,int where)
   {
   int i,j,k;

   matrix O=M_0;

   /* matrix by matrix multiplication */
   if (where==M_RIGHT)
      for (i=0; i<4; i++)
         for (j=0; j<4; j++)
            for (k=0; k<4; k++) O.M[i][j]+=M->M[k][j]*N->M[i][k];
   else
      for (i=0; i<4; i++)
         for (j=0; j<4; j++)
            for (k=0; k<4; k++) O.M[i][j]+=N->M[k][j]*M->M[i][k];

   *M=O;
   }
