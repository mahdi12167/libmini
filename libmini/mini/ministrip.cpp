// (c) by Stefan Roettger

#include "minibase.h"

#include "minibrick.h"

#include "ministrip.h"

int ministrip::INSTANCES=0;

ministrip::SHADER_TYPE ministrip::SHADER[SHADERMAX];

ministrip::SNIPPET_TYPE ministrip::SNIPPET[SNIPPETMAX];
int ministrip::SNIPPETS=0;

// initialize shader snippets
void ministrip::initsnippets()
   {
   // vertex program snippets:

   addsnippet(MINI_SNIPPET_VTX_BEGIN,"!!ARBvp1.0\n");

   addsnippet(MINI_SNIPPET_VTX_HEADER,"\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      PARAM c2=program.env[2]; \n\
      PARAM c3=program.env[3]; \n\
      PARAM c4=program.env[4]; \n\
      PARAM c5=program.env[5]; \n\
      PARAM c6=program.env[6]; \n\
      PARAM c7=program.env[7]; \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM matrix[4]={state.matrix.modelview}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      PARAM texmat[4]={state.matrix.texture[0]}; \n\
      TEMP vtx,col,pos; \n");

   addsnippet(MINI_SNIPPET_VTX_BASIC,"\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position; \n\
      MOV col,vertex.color; \n\
      ### transform vertex with combined modelview \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      ### write transformed vertex \n\
      MOV result.position,pos; \n");

   addsnippet(MINI_SNIPPET_VTX_VIEWPOS,"\
      ### transform vertex with modelview \n\
      DP4 pos.x,matrix[0],vtx; \n\
      DP4 pos.y,matrix[1],vtx; \n\
      DP4 pos.z,matrix[2],vtx; \n\
      DP4 pos.w,matrix[3],vtx; \n\
      ### write view position to tex coords \n\
      MOV result.texcoord[1],pos; \n");

   addsnippet(MINI_SNIPPET_VTX_NORMAL,"\
      ### fetch actual normal \n\
      TEMP nrm; \n\
      MOV nrm,vertex.normal; \n\
      ### transform normal with inverse transpose \n\
      TEMP vec; \n\
      DP4 vec.x,invtra[0],nrm; \n\
      DP4 vec.y,invtra[1],nrm; \n\
      DP4 vec.z,invtra[2],nrm; \n\
      DP4 vec.w,invtra[3],nrm; \n\
      ### write normal to tex coords \n\
      MOV result.texcoord[2],vec; \n");

   addsnippet(MINI_SNIPPET_VTX_NORMAL_DIRECT,"\
      ### directly write normal to tex coords\n\
      MOV result.texcoord[2],vertex.normal; \n");

   addsnippet(MINI_SNIPPET_VTX_TEX,"\
      ### fetch actual tex coords \n\
      TEMP texcrd; \n\
      MOV texcrd,vertex.texcoord[0]; \n\
      ### transform tex coords with texture matrix \n\
      TEMP crd; \n\
      DP4 crd.x,texmat[0],texcrd; \n\
      DP4 crd.y,texmat[1],texcrd; \n\
      DP4 crd.z,texmat[2],texcrd; \n\
      DP4 crd.w,texmat[3],texcrd; \n\
      ### write resulting tex coords \n\
      MOV result.texcoord[0],crd; \n");

   addsnippet(MINI_SNIPPET_VTX_FOG,"\
      ### calculate spherical fog coord \n\
      DP3 result.fogcoord.x,pos,pos; \n");

   addsnippet(MINI_SNIPPET_VTX_FOOTER,"\
      ### write resulting color \n\
      MOV result.color,col; \n");

   addsnippet(MINI_SNIPPET_VTX_END,"END\n");

   // fragment program snippets:

   addsnippet(MINI_SNIPPET_FRG_BEGIN,"!!ARBfp1.0\n");

   addsnippet(MINI_SNIPPET_FRG_HEADER,"\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      PARAM c2=program.env[2]; \n\
      PARAM c3=program.env[3]; \n\
      PARAM c4=program.env[4]; \n\
      PARAM c5=program.env[5]; \n\
      PARAM c6=program.env[6]; \n\
      PARAM c7=program.env[7]; \n\
      TEMP col; \n");

   addsnippet(MINI_SNIPPET_FRG_BASIC,"\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n");

   addsnippet(MINI_SNIPPET_FRG_TEX,"\
      ### fetch actual texel \n\
      TEMP tex; \n\
      TEX tex,fragment.texcoord[0],texture[0],2D; \n\
      ### modulate with texture color \n\
      MUL col,col,tex; \n");

   addsnippet(MINI_SNIPPET_FRG_TEX_MASK,"\
      ### fetch actual texel \n\
      TEMP tex,crd; \n\
      TEX tex,fragment.texcoord[0],texture[0],2D; \n\
      ### check coordinate range \n\
      MOV crd,fragment.texcoord[0]; \n\
      CMP tex.a,crd.x,0.0,tex.a; \n\
      CMP tex.a,crd.y,0.0,tex.a; \n\
      SUB crd,1.0,crd; \n\
      CMP tex.a,crd.x,0.0,tex.a; \n\
      CMP tex.a,crd.y,0.0,tex.a; \n\
      ### modulate with texture color \n\
      MUL col,col,tex; \n");

   addsnippet(MINI_SNIPPET_FRG_TEX2_DIRECT,"\
      ### fetch two texels \n\
      TEMP tex1,tex2; \n\
      TEX tex1,fragment.texcoord[0],texture[0],2D; \n\
      TEX tex2,fragment.texcoord[0],texture[1],2D; \n\
      ### fetch fragment normal \n\
      TEMP nrm; \n\
      MOV nrm,fragment.texcoord[2]; \n\
      ### calculate directional light \n\
      TEMP len; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 nrm.z,nrm,c5; \n\
      ### blend between texels \n\
      MAD nrm.z,nrm.z,c4.x,c4.y; \n\
      MAD_SAT nrm.z,nrm.z,c4.w,c4.w; \n\
      MUL nrm.z,nrm.z,c5.w; \n\
      SIN nrm.z,nrm.z; \n\
      LRP tex1,nrm.z,tex1,tex2; \n\
      ### modulate with texture color \n\
      MUL col,col,tex1; \n");

   addsnippet(MINI_SNIPPET_FRG_SHADE,"\
      ### fetch view position \n\
      TEMP pos; \n\
      MOV pos,fragment.texcoord[1]; \n\
      ### fetch fragment normal \n\
      TEMP nrm; \n\
      MOV nrm,fragment.texcoord[2]; \n\
      ### calculate head light \n\
      TEMP len; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 len.x,pos,pos; \n\
      RSQ len.x,len.x; \n\
      MUL pos,pos,len.x; \n\
      DP3 nrm.z,nrm,pos; \n\
      ### modulate fragment color \n\
      ABS nrm.z,nrm.z; \n\
      MUL col.xyz,col,nrm.z; \n");

   addsnippet(MINI_SNIPPET_FRG_SHADE_ALPHA,"\
      ### fetch view position \n\
      TEMP pos; \n\
      MOV pos,fragment.texcoord[1]; \n\
      ### fetch fragment normal \n\
      TEMP nrm; \n\
      MOV nrm,fragment.texcoord[2]; \n\
      ### calculate head light \n\
      TEMP len; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 len.x,pos,pos; \n\
      RSQ len.x,len.x; \n\
      MUL pos,pos,len.x; \n\
      DP3 nrm.z,nrm,pos; \n\
      ### modulate fragment alpha \n\
      ABS nrm.z,nrm.z; \n\
      MUL col.a,col.a,nrm.z; \n");

   addsnippet(MINI_SNIPPET_FRG_SHADE_ALPHA2,"\
      ### fetch view position \n\
      TEMP pos; \n\
      MOV pos,fragment.texcoord[1]; \n\
      ### fetch fragment normal \n\
      TEMP nrm; \n\
      MOV nrm,fragment.texcoord[2]; \n\
      ### calculate head light \n\
      TEMP len; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3 len.x,pos,pos; \n\
      RSQ len.x,len.x; \n\
      MUL pos,pos,len.x; \n\
      DP3 nrm.z,nrm,pos; \n\
      ### modulate fragment alpha \n\
      MUL nrm.z,nrm.z,1.5; \n\
      MUL nrm.z,nrm.z,nrm.z; \n\
      MUL col.a,col.a,nrm.z; \n");

   addsnippet(MINI_SNIPPET_FRG_SHADE_DIRECT,"\
      ### fetch fragment normal \n\
      TEMP nrm; \n\
      MOV nrm,fragment.texcoord[2]; \n\
      ### calculate directional light \n\
      TEMP len; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3_SAT nrm.z,nrm,c5; \n\
      ### modulate fragment color \n\
      MAD nrm.z,nrm.z,c4.x,c4.y; \n\
      MUL col.xyz,col,nrm.z; \n");

   addsnippet(MINI_SNIPPET_FRG_FOG,"\
      ### fetch fog coord \n\
      TEMP fog; \n\
      MOV fog.x,fragment.fogcoord.x; \n\
      ### modulate with spherical fog \n\
      MAD_SAT fog.x,fog.x,c6.x,c6.y; \n\
      POW fog.x,fog.x,c6.z; \n\
      LRP col.xyz,fog.x,c7,col; \n");

   addsnippet(MINI_SNIPPET_FRG_FOOTER,"\
      ### write resulting pixel \n\
      MOV result.color,col; \n");

   addsnippet(MINI_SNIPPET_FRG_END,"END\n");
   }

// free shader snippets
void ministrip::freesnippets()
   {
   int n;

   for (n=0; n<SNIPPETS; n++)
      {
      if (SNIPPET[n].snippetname!=NULL) free(SNIPPET[n].snippetname);
      if (SNIPPET[n].snippet!=NULL) free(SNIPPET[n].snippet);
      }

   SNIPPETS=0;
   }

// add shader snippet
void ministrip::addsnippet(const char *snippetname,const char *snippet)
   {
   if (SNIPPETS>=SNIPPETMAX) ERRORMSG();

   SNIPPET[SNIPPETS].snippetname=strdup(snippetname);
   SNIPPET[SNIPPETS].snippet=strdup(snippet);

   SNIPPETS++;
   }

// get free shader slot
int ministrip::getfreeslot()
   {
   int n;

   for (n=0; n<SHADERMAX; n++)
      if (SHADER[n].occupied==0)
         {
         SHADER[n].occupied=1;
         return(n);
         }

   return(-1);
   }

// initialize default shader
void ministrip::initshader()
   {
   int slot;

   slot=getfreeslot();

   concatvtxshader(slot,MINI_SNIPPET_VTX_BEGIN);
   concatvtxshader(slot,MINI_SNIPPET_VTX_HEADER);
   concatvtxshader(slot,MINI_SNIPPET_VTX_BASIC);
   concatvtxshader(slot,MINI_SNIPPET_VTX_VIEWPOS);
   concatvtxshader(slot,MINI_SNIPPET_VTX_NORMAL);
   concatvtxshader(slot,MINI_SNIPPET_VTX_FOOTER);
   concatvtxshader(slot,MINI_SNIPPET_VTX_END);

   concatpixshader(slot,MINI_SNIPPET_FRG_BEGIN);
   concatpixshader(slot,MINI_SNIPPET_FRG_HEADER);
   concatpixshader(slot,MINI_SNIPPET_FRG_BASIC);
   concatpixshader(slot,MINI_SNIPPET_FRG_SHADE);
   concatpixshader(slot,MINI_SNIPPET_FRG_FOOTER);
   concatpixshader(slot,MINI_SNIPPET_FRG_END);
   }

// default constructor
ministrip::ministrip(int colcomps,int nrmcomps,int texcomps)
   {
   int i,j;

   if (colcomps!=0 && colcomps!=3 && colcomps!=4) ERRORMSG();
   if (nrmcomps!=0 && nrmcomps!=3) ERRORMSG();
   if (texcomps<0 || texcomps>4) ERRORMSG();

   COLCOMPS=colcomps;
   NRMCOMPS=nrmcomps;
   TEXCOMPS=texcomps;

   MAXSIZE=1;

   if ((VTXARRAY=(float *)malloc(3*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

   if (COLCOMPS==0) COLARRAY=NULL;
   else
      if ((COLARRAY=(float *)malloc(COLCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

   if (NRMCOMPS==0) NRMARRAY=NULL;
   else
      if ((NRMARRAY=(float *)malloc(NRMCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

   if (TEXCOMPS==0) TEXARRAY=NULL;
   else
      if ((TEXARRAY=(float *)malloc(TEXCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

   SIZE=0;

   COLR=1.0f;
   COLG=1.0f;
   COLB=1.0f;
   COLA=1.0f;

   NRMX=0.0f;
   NRMY=1.0f;
   NRMZ=0.0f;

   TEXX=0.0f;
   TEXY=0.0f;
   TEXZ=0.0f;
   TEXW=1.0f;

   COPYVTX=0;

   SCALE=1.0f;

   for (i=0; i<16; i++) MTX[i]=0.0;
   for (i=0; i<4; i++) MTX[i+4*i]=1.0;
   MTXSET=FALSE;

   for (i=0; i<16; i++) TEXMTX[i]=0.0;
   for (i=0; i<4; i++) TEXMTX[i+4*i]=1.0;
   TEXMTXSET=FALSE;

   ZSCALE=1.0f;

   USESHADER=-1;

   if (INSTANCES==0)
      {
      for (i=0; i<SHADERMAX; i++)
         {
         SHADER[i].occupied=0;

         SHADER[i].vtxprog=NULL;
         SHADER[i].vtxprogid=0;
         SHADER[i].vtxdirty=0;

         SHADER[i].frgprog=NULL;
         SHADER[i].frgprogid=0;
         SHADER[i].frgdirty=0;

         for (j=0; j<SHADERVTXPRMMAX; j++)
            {
            SHADER[i].vtxshaderpar1[j]=0.0f;
            SHADER[i].vtxshaderpar2[j]=0.0f;
            SHADER[i].vtxshaderpar3[j]=0.0f;
            SHADER[i].vtxshaderpar4[j]=0.0f;
            }

         for (j=0; j<SHADERFRGPRMMAX; j++)
            {
            SHADER[i].pixshaderpar1[j]=0.0f;
            SHADER[i].pixshaderpar2[j]=0.0f;
            SHADER[i].pixshaderpar3[j]=0.0f;
            SHADER[i].pixshaderpar4[j]=0.0f;
            }

         for (j=0; j<SHADERFRGTEXMAX; j++)
            {
            SHADER[i].pixshadertexid[j]=0;
            SHADER[i].pixshadertexw[j]=0;
            SHADER[i].pixshadertexh[j]=0;
            SHADER[i].pixshadertexcl[j]=0;
            SHADER[i].pixshadertexmm[j]=0;
            SHADER[i].pixshadertexnofree[j]=0;
            }
         }

      initsnippets();
      initshader();
      }

   INSTANCES++;
   }

// destructor
ministrip::~ministrip()
   {
   int i,j;

   free(VTXARRAY);

   if (COLARRAY!=NULL) free(COLARRAY);
   if (NRMARRAY!=NULL) free(NRMARRAY);
   if (TEXARRAY!=NULL) free(TEXARRAY);

   INSTANCES--;

   if (INSTANCES==0)
      {
      for (i=0; i<SHADERMAX; i++)
         {
         SHADER[i].occupied=0;

         if (SHADER[i].vtxprog!=NULL)
            {
            free(SHADER[i].vtxprog);
            SHADER[i].vtxprog=NULL;
            if (SHADER[i].vtxprogid!=0) deletevtxprog(SHADER[i].vtxprogid);
            SHADER[i].vtxdirty=0;
            }

         if (SHADER[i].frgprog!=NULL)
            {
            free(SHADER[i].frgprog);
            SHADER[i].frgprog=NULL;
            if (SHADER[i].frgprogid!=0) deletefrgprog(SHADER[i].frgprogid);
            SHADER[i].frgdirty=0;
            }

         for (j=0; j<SHADERVTXPRMMAX; j++)
            {
            SHADER[i].vtxshaderpar1[j]=0.0f;
            SHADER[i].vtxshaderpar2[j]=0.0f;
            SHADER[i].vtxshaderpar3[j]=0.0f;
            SHADER[i].vtxshaderpar4[j]=0.0f;
            }

         for (j=0; j<SHADERFRGPRMMAX; j++)
            {
            SHADER[i].pixshaderpar1[j]=0.0f;
            SHADER[i].pixshaderpar2[j]=0.0f;
            SHADER[i].pixshaderpar3[j]=0.0f;
            SHADER[i].pixshaderpar4[j]=0.0f;
            }

         for (j=0; j<SHADERFRGTEXMAX; j++)
            if (SHADER[i].pixshadertexid[j]!=0)
               {
               deletetexmap(SHADER[i].pixshadertexid[j]);
               SHADER[i].pixshadertexid[j]=0;

               SHADER[i].pixshadertexw[j]=0;
               SHADER[i].pixshadertexh[j]=0;
               SHADER[i].pixshadertexcl[j]=0;
               SHADER[i].pixshadertexmm[j]=0;
               SHADER[i].pixshadertexnofree[j]=0;
               }
         }

      freesnippets();
      }
   }

// clear strip
void ministrip::clear()
   {SIZE=0;}

// begin next triangle strip
void ministrip::beginstrip()
   {
   if (SIZE>0)
      {
      addvtx();
      COPYVTX=1;
      }
   }

// set color of next vertex
void ministrip::setcol(const float r,const float g,const float b,float a)
   {
   COLR=r;
   COLG=g;
   COLB=b;
   COLA=a;
   }

// set normal of next vertex
void ministrip::setnrm(const float nx,const float ny,const float nz)
   {
   NRMX=nx;
   NRMY=ny;
   NRMZ=nz;
   }

// set tex coords of next vertex
void ministrip::settex(const float tx,const float ty,const float tz,float tw)
   {
   TEXX=tx;
   TEXY=ty;
   TEXZ=tz;
   TEXW=tw;
   }

// add one vertex
void ministrip::addvtx(const float x,const float y,const float z)
   {
   VTXX=x;
   VTXY=y;
   VTXZ=z;

   addvtx();

   if (COPYVTX!=0)
      {
      addvtx();
      COPYVTX=0;
      }
   }

// add one vertex
void ministrip::addvtx()
   {
   float *ptr;

   if (SIZE>=MAXSIZE)
      {
      MAXSIZE*=2;

      if ((VTXARRAY=(float *)realloc(VTXARRAY,3*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

      if (COLARRAY!=NULL)
         if ((COLARRAY=(float *)realloc(COLARRAY,COLCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

      if (NRMARRAY!=NULL)
         if ((NRMARRAY=(float *)realloc(NRMARRAY,NRMCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();

      if (TEXARRAY!=NULL)
         if ((TEXARRAY=(float *)realloc(TEXARRAY,TEXCOMPS*MAXSIZE*sizeof(float)))==NULL) MEMERROR();
      }

   ptr=&VTXARRAY[3*SIZE];

   *ptr++=VTXX;
   *ptr++=VTXY;
   *ptr++=VTXZ;

   if (COLARRAY!=NULL)
      {
      ptr=&COLARRAY[COLCOMPS*SIZE];

      *ptr++=COLR;
      *ptr++=COLG;
      *ptr++=COLB;

      if (COLCOMPS==4) *ptr=COLA;
      }

   if (NRMARRAY!=NULL)
      {
      ptr=&NRMARRAY[NRMCOMPS*SIZE];

      *ptr++=NRMX;
      *ptr++=NRMY;
      *ptr++=NRMZ;
      }

   if (TEXARRAY!=NULL)
      {
      ptr=&TEXARRAY[TEXCOMPS*SIZE];

      *ptr++=TEXX;

      if (TEXCOMPS>1) *ptr++=TEXY;
      if (TEXCOMPS>2) *ptr++=TEXZ;
      if (TEXCOMPS>3) *ptr=TEXW;
      }

   SIZE++;
   }

// set scale
void ministrip::setscale(float scale)
   {SCALE=scale;}

// set matrix
void ministrip::setmatrix(double mtx[16])
   {
   int i;

   for (i=0; i<16; i++) MTX[i]=mtx[i];
   MTXSET=TRUE;
   }

// set texture matrix
void ministrip::settexmatrix(double texmtx[16])
   {
   int i;

   for (i=0; i<16; i++) TEXMTX[i]=texmtx[i];
   TEXMTXSET=TRUE;
   }

// set Z-scaling
void ministrip::setZscale(float zscale)
   {ZSCALE=zscale;}

// set vertex shader
void ministrip::setvtxshader(int num,char *vtxprog)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();

   if (SHADER[num].vtxprog!=NULL)
      {
      if (strcmp(vtxprog,SHADER[num].vtxprog)==0) return;
      free(SHADER[num].vtxprog);
      }

   SHADER[num].vtxprog=strdup(vtxprog);
   SHADER[num].vtxdirty=1;
   }

// concatenate vertex shader from snippets
void ministrip::concatvtxshader(int num,const char *snippetname)
   {
   int n;

   char *vtxprog;

   if (num<0 || num>=SHADERMAX) ERRORMSG();

   for (n=0; n<SNIPPETS; n++)
      if (strcmp(snippetname,SNIPPET[n].snippetname)==0)
         {
         vtxprog=strdup2(SHADER[num].vtxprog,SNIPPET[n].snippet);
         if (SHADER[num].vtxprog!=NULL) free(SHADER[num].vtxprog);
         SHADER[num].vtxprog=vtxprog;
         SHADER[num].vtxdirty=1;

         break;
         }
   }

// set vertex shader parameter vector
void ministrip::setvtxshaderparams(int num,float p1,float p2,float p3,float p4,int n)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();
   if (n<0 || n>=SHADERVTXPRMMAX) ERRORMSG();

   SHADER[num].vtxshaderpar1[n]=p1;
   SHADER[num].vtxshaderpar2[n]=p2;
   SHADER[num].vtxshaderpar3[n]=p3;
   SHADER[num].vtxshaderpar4[n]=p4;
   }

// enable vertex shader
void ministrip::enablevtxshader(int num)
   {
   int i;

   if (num<0 || num>=SHADERMAX) ERRORMSG();

   if (SHADER[num].vtxprog!=NULL)
      {
      if (SHADER[num].vtxdirty!=0)
         {
         if (SHADER[num].vtxprogid!=0) deletevtxprog(SHADER[num].vtxprogid);

         SHADER[num].vtxprogid=buildvtxprog(SHADER[num].vtxprog);
         SHADER[num].vtxdirty=0;
         }

      if (SHADER[num].vtxprogid!=0)
         {
         bindvtxprog(SHADER[num].vtxprogid);

         for (i=0; i<SHADERVTXPRMMAX; i++)
            setvtxprogpar(i,SHADER[num].vtxshaderpar1[i],SHADER[num].vtxshaderpar2[i],SHADER[num].vtxshaderpar3[i],SHADER[num].vtxshaderpar4[i]);
         }
      }
   }

// disable vertex shader
void ministrip::disablevtxshader(int num)
   {
   if (SHADER[num].vtxprog!=NULL)
      if (SHADER[num].vtxprogid!=0) bindvtxprog(0);
   }

// set pixel shader
void ministrip::setpixshader(int num,char *frgprog)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();

   if (SHADER[num].frgprog!=NULL)
      {
      if (strcmp(frgprog,SHADER[num].frgprog)==0) return;
      free(SHADER[num].frgprog);
      }

   SHADER[num].frgprog=strdup(frgprog);
   SHADER[num].frgdirty=1;
   }

// concatenate pixel shader from snippets
void ministrip::concatpixshader(int num,const char *snippetname)
   {
   int n;

   char *frgprog;

   if (num<0 || num>=SHADERMAX) ERRORMSG();

   for (n=0; n<SNIPPETS; n++)
      if (strcmp(snippetname,SNIPPET[n].snippetname)==0)
         {
         frgprog=strdup2(SHADER[num].frgprog,SNIPPET[n].snippet);
         if (SHADER[num].frgprog!=NULL) free(SHADER[num].frgprog);
         SHADER[num].frgprog=frgprog;
         SHADER[num].frgdirty=1;

         break;
         }
   }

// set pixel shader parameter vector
void ministrip::setpixshaderparams(int num,float p1,float p2,float p3,float p4,int n)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();
   if (n<0 || n>=SHADERFRGPRMMAX) ERRORMSG();

   SHADER[num].pixshaderpar1[n]=p1;
   SHADER[num].pixshaderpar2[n]=p2;
   SHADER[num].pixshaderpar3[n]=p3;
   SHADER[num].pixshaderpar4[n]=p4;
   }

// set pixel shader RGB texture map
void ministrip::setpixshadertexRGB(int num,unsigned char *image,int width,int height,int clamp,int mipmaps,int n)
   {setpixshadertex(num,image,width,height,3,clamp,mipmaps,n);}

// set pixel shader RGBA texture map
void ministrip::setpixshadertexRGBA(int num,unsigned char *image,int width,int height,int clamp,int mipmaps,int n)
   {setpixshadertex(num,image,width,height,4,clamp,mipmaps,n);}

// set pixel shader RGB[A] texture map
void ministrip::setpixshadertex(int num,unsigned char *image,int width,int height,int components,int clamp,int mipmaps,int n)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();
   if (n<0 || n>=SHADERFRGTEXMAX) ERRORMSG();

   if (width<2 || height<2) ERRORMSG();

   if (SHADER[num].pixshadertexid[n]!=0)
      {
      if (SHADER[num].pixshadertexnofree[n]==0)  deletetexmap(SHADER[num].pixshadertexid[n]);
      SHADER[num].pixshadertexid[n]=0;

      SHADER[num].pixshadertexw[n]=0;
      SHADER[num].pixshadertexh[n]=0;
      SHADER[num].pixshadertexcl[n]=0;
      SHADER[num].pixshadertexmm[n]=0;
      SHADER[num].pixshadertexnofree[n]=0;
      }

   if (image!=NULL)
      {
      if (components==1) SHADER[num].pixshadertexid[n]=buildLtexmap(image,&width,&height,mipmaps);
      else if (components==3) SHADER[num].pixshadertexid[n]=buildRGBtexmap(image,&width,&height,mipmaps);
      else if (components==4) SHADER[num].pixshadertexid[n]=buildRGBAtexmap(image,&width,&height,mipmaps);
      else ERRORMSG();

      SHADER[num].pixshadertexw[n]=width;
      SHADER[num].pixshadertexh[n]=height;
      SHADER[num].pixshadertexcl[n]=clamp;
      SHADER[num].pixshadertexmm[n]=mipmaps;
      SHADER[num].pixshadertexnofree[n]=0;
      }
   }

// set pixel shader RGB[A] texture map from texture id
void ministrip::setpixshadertexid(int num,int texid,int width,int height,int clamp,int mipmaps,int n)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();
   if (n<0 || n>=SHADERFRGTEXMAX) ERRORMSG();

   if (width<2 || height<2) ERRORMSG();

   if (SHADER[num].pixshadertexid[n]!=0)
      {
      if (SHADER[num].pixshadertexnofree[n]==0) deletetexmap(SHADER[num].pixshadertexid[n]);
      SHADER[num].pixshadertexid[n]=0;

      SHADER[num].pixshadertexw[n]=0;
      SHADER[num].pixshadertexh[n]=0;
      SHADER[num].pixshadertexcl[n]=0;
      SHADER[num].pixshadertexmm[n]=0;
      SHADER[num].pixshadertexnofree[n]=0;
      }

   SHADER[num].pixshadertexid[n]=texid;
   SHADER[num].pixshadertexw[n]=width;
   SHADER[num].pixshadertexh[n]=height;
   SHADER[num].pixshadertexcl[n]=clamp;
   SHADER[num].pixshadertexmm[n]=mipmaps;
   SHADER[num].pixshadertexnofree[n]=1;
   }

// set pixel shader RGB[A] texture map from image buffer
void ministrip::setpixshadertexbuf(int num,databuf *buf,int clamp,int mipmaps,int n)
   {
   int texid;
   int width,height;

   if (num<0 || num>=SHADERMAX) ERRORMSG();
   if (n<0 || n>=SHADERFRGTEXMAX) ERRORMSG();

   if (buf->missing()) ERRORMSG();

   if (buf->xsize<2 || buf->ysize<2 ||
       buf->zsize>1 || buf->tsteps>1) ERRORMSG();

   width=buf->xsize;
   height=buf->ysize;

   if (SHADER[num].pixshadertexid[n]!=0)
      {
      deletetexmap(SHADER[num].pixshadertexid[n]);
      SHADER[num].pixshadertexid[n]=0;

      SHADER[num].pixshadertexw[n]=0;
      SHADER[num].pixshadertexh[n]=0;
      SHADER[num].pixshadertexcl[n]=0;
      SHADER[num].pixshadertexmm[n]=0;
      SHADER[num].pixshadertexnofree[n]=0;
      }

   texid=db2texid(buf,&width,&height,&mipmaps);

   SHADER[num].pixshadertexid[n]=texid;
   SHADER[num].pixshadertexw[n]=width;
   SHADER[num].pixshadertexh[n]=height;
   SHADER[num].pixshadertexcl[n]=clamp;
   SHADER[num].pixshadertexmm[n]=mipmaps;
   SHADER[num].pixshadertexnofree[n]=0;
   }

// enable pixel shader
void ministrip::enablepixshader(int num)
   {
   int i;

   if (num<0 || num>=SHADERMAX) ERRORMSG();

   if (SHADER[num].frgprog!=NULL)
      {
      if (SHADER[num].frgdirty!=0)
         {
         if (SHADER[num].frgprogid!=0) deletefrgprog(SHADER[num].frgprogid);

         SHADER[num].frgprogid=buildfrgprog(SHADER[num].frgprog);
         SHADER[num].frgdirty=0;
         }

      if (SHADER[num].frgprogid!=0)
         {
         bindfrgprog(SHADER[num].frgprogid);

         for (i=0; i<SHADERFRGPRMMAX; i++)
            setfrgprogpar(i,SHADER[num].pixshaderpar1[i],SHADER[num].pixshaderpar2[i],SHADER[num].pixshaderpar3[i],SHADER[num].pixshaderpar4[i]);

         for (i=0; i<SHADERFRGTEXMAX; i++)
            if (SHADER[num].pixshadertexid[i]!=0)
               {
               texunit(i);

               if (SHADER[num].pixshadertexcl[i]==0) bindtexmap(SHADER[num].pixshadertexid[i],0,0,0,SHADER[num].pixshadertexmm[i]);
               else bindtexmap(SHADER[num].pixshadertexid[i],SHADER[num].pixshadertexw[i],SHADER[num].pixshadertexh[i],0,SHADER[num].pixshadertexmm[i]);
               }

         texunit(0);
         }
      }
   }

// disable pixel shader
void ministrip::disablepixshader(int num)
   {
   int i;

   if (SHADER[num].frgprog!=NULL)
      {
      if (SHADER[num].frgprogid!=0) bindfrgprog(0);

      for (i=0; i<SHADERFRGTEXMAX; i++)
         if (SHADER[num].pixshadertexid[i]!=0)
            {
            texunit(i);
            bindtexmap(0,0,0,0,0);
            }

      texunit(0);
      }
   }

// set direct shading parameters
void ministrip::setshadedirectparams(int num,
                                     float lightdir[3],
                                     float lightbias,float lightoffset)
   {
   setpixshaderparams(num,lightdir[0],lightdir[1],lightdir[2],0.0f,5);
   setpixshaderparams(num,lightbias,lightoffset,0.0f,0.0f,4);
   }

// set direct texturing parameters
void ministrip::settexturedirectparams(int num,
                                       float lightdir[3],
                                       float transbias,float transoffset)
   {
   setpixshaderparams(num,lightdir[0],lightdir[1],lightdir[2],PI/2,5);
   setpixshaderparams(num,transbias,transoffset,0.0f,0.5f,4);
   }

// set fog parameters
void ministrip::setfogparams(int num,
                             float fogstart,float fogend,
                             float fogdensity,
                             float *fogcolor)
   {
   float fog_a,fog_b,fog_c;

   // calculate the fog parameters
   if (fogstart<fogend && fogdensity>0.0f)
      {
      fog_a=fsqr(1.0f/fogend);
      fog_b=0.0f;
      fog_c=log(fmin(0.5f*fogdensity,1.0f))/(2.0f*log(fogstart/fogend/2.0f+0.5f));
      }
   else
      {
      fog_a=0.0f;
      fog_b=0.0f;
      fog_c=1.0f;
      }

   // pass the fog parameters
   setpixshaderparams(num,fog_a,fog_b,fog_c,0.0f,6);
   setpixshaderparams(num,fogcolor[0],fogcolor[1],fogcolor[2],0.0f,7);
   }

// set actual shader
void ministrip::useshader(int num)
   {USESHADER=num;}

// get actual shader
int ministrip::getshader()
   {return(USESHADER);}

// render triangle strips
void ministrip::render()
   {
   if (COLARRAY==NULL) color(COLR,COLG,COLB,COLA);
   if (NRMARRAY==NULL) normal(NRMX,NRMY,NRMZ);

   if (MTXSET || SCALE!=1.0f)
      {
      mtxpush();
      mtxmult(MTX);
      mtxscale(SCALE,SCALE,SCALE);
      }

   if (ZSCALE!=1.0f)
      {
      mtxproj();
      mtxpush();
      mtxscale(ZSCALE,ZSCALE,ZSCALE);
      mtxmodel();
      }

   if (USESHADER>=0)
      {
      enablevtxshader(USESHADER);
      enablepixshader(USESHADER);
      }

   vertexarray(VTXARRAY);
   colorarray(COLARRAY,COLCOMPS);
   normalarray(NRMARRAY);

   if (TEXARRAY!=NULL)
      {
      if (TEXMTXSET)
         {
         mtxtex();
         mtxpush();
         mtxmult(TEXMTX);
         mtxmodel();
         }

      texclientunit(0);
      texcoordarray(TEXARRAY,TEXCOMPS);
      }
   else texcoordarray(NULL);

   renderstrip(0,SIZE);

   vertexarray(NULL);
   colorarray(NULL);
   normalarray(NULL);

   if (TEXARRAY!=NULL)
      {
      texcoordarray(NULL);

      if (TEXMTXSET)
         {
         mtxtex();
         mtxpop();
         mtxmodel();
         }
      }

   if (USESHADER>=0)
      {
      disablevtxshader(USESHADER);
      disablepixshader(USESHADER);
      }

   if (ZSCALE!=1.0f)
      {
      mtxproj();
      mtxpop();
      mtxmodel();
      }

   if (MTXSET || SCALE!=1.0f) mtxpop();
   }

// render triangle strips with multi-pass method for unordered semi-transparent geometry
void ministrip::rendermulti(int passes,
                            float ambient,
                            float bordercontrol,float centercontrol,float colorcontrol,
                            float bordercontrol2,float centercontrol2,float colorcontrol2,
                            float stripewidth,float stripeoffset,
                            float stripedx,float stripedy,float stripedz,
                            int correctz)
   {
   int i;

   static minisurf surf;

   int shader;

   int dorender;

   if (passes<1 || passes>4) ERRORMSG();

   // disable regular shader
   shader=getshader();
   useshader(-1);

   // multi-pass rendering
   for (i=minisurf::FIRST_RENDER_PHASE; i<=minisurf::LAST_RENDER_PHASE; i++)
      {
      // enable external multi-pass shader
      dorender=surf.setextstate(1,
                                i,passes,
                                ambient,
                                bordercontrol,centercontrol,colorcontrol,
                                bordercontrol2,centercontrol2,colorcontrol2,
                                stripewidth,stripeoffset,stripedx,stripedy,stripedz,
                                correctz);

      // render strips with external multi-pass shader
      if (dorender!=0) render();

      // disable external multi-pass shader
      surf.setextstate(0,
                       i,passes,
                       ambient,
                       bordercontrol,centercontrol,colorcontrol,
                       bordercontrol2,centercontrol2,colorcontrol2,
                       stripewidth,stripeoffset,stripedx,stripedy,stripedz,
                       correctz);
      }

   // enable previous shader
   useshader(shader);
   }

// get vertex shader
char *ministrip::getvtxshader(int num)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();

   return(SHADER[num].vtxprog);
   }

// get pixel shader
char *ministrip::getpixshader(int num)
   {
   if (num<0 || num>=SHADERMAX) ERRORMSG();

   return(SHADER[num].frgprog);
   }
