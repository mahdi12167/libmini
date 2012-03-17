// (c) by Stefan Roettger

#include "minibase.h"
#include "minimath.h"

#include "minibrick.h"

#include "ministrip.h"

int ministrip::INSTANCES=0;

int ministrip::USEGLOBALSHADER=-1;

ministrip::SHADER_TYPE ministrip::SHADER[SHADERMAX];

ministrip::SNIPPET_TYPE ministrip::SNIPPET[SNIPPETMAX];
int ministrip::SNIPPETS=0;

int ministrip::global_shader[64]={-1};

BOOLINT ministrip::global_texgen=FALSE;
BOOLINT ministrip::global_shade=FALSE,ministrip::global_shade_direct=FALSE;
BOOLINT ministrip::global_tex=FALSE,ministrip::global_tex3=FALSE;
BOOLINT ministrip::global_fog=FALSE;

float ministrip::global_fogstart=0.0f,ministrip::global_fogend=0.0f;
float ministrip::global_fogdensity=0.0f,ministrip::global_fogcolor[3]={0,0,0};

float ministrip::global_lightdir[3]={0,0,0};
float ministrip::global_lightbias=0.5f,ministrip::global_lightoffset=0.5f;
float ministrip::global_transbias=4.0f,ministrip::global_transoffset=0.0f;

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

   addsnippet(MINI_SNIPPET_VTX_TEXGEN,"\
      ### generate tex coords from vertex position \n\
      TEMP crd; \n\
      DP4 crd.x,texmat[0],vtx; \n\
      DP4 crd.y,texmat[1],vtx; \n\
      DP4 crd.z,texmat[2],vtx; \n\
      DP4 crd.w,texmat[3],vtx; \n\
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

   addsnippet(MINI_SNIPPET_FRG_TEX3,"\
      ### fetch actual voxel \n\
      TEMP tex; \n\
      TEX tex,fragment.texcoord[0],texture[0],3D; \n\
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

   for (slot=0; slot<64; slot++)
      global_shader[slot]=createshader(slot&1,
                                       slot&2,slot&4,
                                       slot&8,slot&16,
                                       slot&32);
   }

// create basic shader
int ministrip::createshader(BOOLINT texgen,
                            BOOLINT shade,BOOLINT shade_direct,
                            BOOLINT tex,BOOLINT tex3,
                            BOOLINT fog)
   {
   int slot=getfreeslot();

   concatvtxshader(slot,MINI_SNIPPET_VTX_BEGIN);
   concatvtxshader(slot,MINI_SNIPPET_VTX_HEADER);
   concatvtxshader(slot,MINI_SNIPPET_VTX_BASIC);
   concatvtxshader(slot,MINI_SNIPPET_VTX_VIEWPOS);
   concatvtxshader(slot,MINI_SNIPPET_VTX_NORMAL);
   if (tex || tex3)
      if (texgen)
         concatvtxshader(slot,MINI_SNIPPET_VTX_TEXGEN);
      else
         concatvtxshader(slot,MINI_SNIPPET_VTX_TEX);
   if (fog)
      concatvtxshader(slot,MINI_SNIPPET_VTX_FOG);
   concatvtxshader(slot,MINI_SNIPPET_VTX_FOOTER);
   concatvtxshader(slot,MINI_SNIPPET_VTX_END);

   concatpixshader(slot,MINI_SNIPPET_FRG_BEGIN);
   concatpixshader(slot,MINI_SNIPPET_FRG_HEADER);
   concatpixshader(slot,MINI_SNIPPET_FRG_BASIC);
   if (tex)
      concatpixshader(slot,MINI_SNIPPET_FRG_TEX);
   else if (tex3)
      concatpixshader(slot,MINI_SNIPPET_FRG_TEX3);
   if (shade)
      concatpixshader(slot,MINI_SNIPPET_FRG_SHADE);
   else if (shade_direct)
      concatpixshader(slot,MINI_SNIPPET_FRG_SHADE_DIRECT);
   if (fog)
      concatpixshader(slot,MINI_SNIPPET_FRG_FOG);
   concatpixshader(slot,MINI_SNIPPET_FRG_FOOTER);
   concatpixshader(slot,MINI_SNIPPET_FRG_END);

   return(slot);
   }

// enable global shader
void ministrip::enableglobalshader(BOOLINT texgen,
                                   BOOLINT shade,BOOLINT shade_direct,
                                   BOOLINT tex,BOOLINT tex3,
                                   BOOLINT fog)
   {
   useglobalshader(global_shader[texgen+
                                 2*shade+
                                 4*shade_direct+
                                 8*tex+
                                 16*tex3+
                                 32*fog]);
   }

// enable global shader
void ministrip::enableglobalshader()
   {
   enableglobalshader(global_texgen,
                      global_shade,global_shade_direct,
                      global_tex,global_tex3,
                      global_fog);

   settexturedirectparams(getglobalshader(),
                          global_lightdir,
                          global_transbias,global_transoffset);

   setshadedirectparams(getglobalshader(),
                        global_lightdir,
                        global_lightbias,global_lightoffset);

   setfogparams(getglobalshader(),
                global_fogstart,global_fogend,
                global_fogdensity,
                global_fogcolor);
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

   BBOXMIN=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
   BBOXMAX=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

   COLR=1.0f;
   COLG=1.0f;
   COLB=1.0f;
   COLA=1.0f;

   COLAMIN=1.0f;
   COLAMAX=0.0f;

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
   {
   SIZE=0;

   BBOXMIN=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
   BBOXMAX=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

   COLAMIN=1.0f;
   COLAMAX=0.0f;

   COPYVTX=0;
   }

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

   if (a<COLAMIN) COLAMIN=a;
   if (a>COLAMAX) COLAMAX=a;
   }

void ministrip::setcol(const miniv3d &rgb)
   {setcol(rgb.x,rgb.y,rgb.z,1.0);}

void ministrip::setcol(const miniv4d &rgba)
   {setcol(rgba.x,rgba.y,rgba.z,rgba.w);}

// set normal of next vertex
void ministrip::setnrm(const float nx,const float ny,const float nz)
   {
   NRMX=nx;
   NRMY=ny;
   NRMZ=nz;
   }

void ministrip::setnrm(const miniv3d &n)
   {setnrm(n.x,n.y,n.z);}

// set normal from three points
void ministrip::setnrm(const miniv3d &v1,const miniv3d &v2,const miniv3d &v3)
   {setnrm((v2-v1)/(v3-v1));}

// set tex coords of next vertex
void ministrip::settex(const float tx,const float ty,const float tz,float tw)
   {
   TEXX=tx;
   TEXY=ty;
   TEXZ=tz;
   TEXW=tw;
   }

void ministrip::settex(const miniv3d &t)
   {settex(t.x,t.y,t.z);}

void ministrip::settex(const miniv4d &t)
   {settex(t.x,t.y,t.z,t.w);}

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

void ministrip::addvtx(const miniv3d &v)
   {addvtx(v.x,v.y,v.z);}

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

   if (VTXX<BBOXMIN.x) BBOXMIN.x=VTXX;
   if (VTXX>BBOXMAX.x) BBOXMAX.x=VTXX;

   if (VTXY<BBOXMIN.y) BBOXMIN.y=VTXY;
   if (VTXY>BBOXMAX.y) BBOXMAX.y=VTXY;

   if (VTXZ<BBOXMIN.z) BBOXMIN.z=VTXZ;
   if (VTXZ>BBOXMAX.z) BBOXMAX.z=VTXZ;

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

   if (buf->missing())
      {
      WARNMSG();
      return;
      }

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

// set global direct shading parameters
void ministrip::setglobalshadedirectparams(float lightdir[3],
                                           float lightbias,float lightoffset)
   {
   global_lightdir[0]=lightdir[0];
   global_lightdir[1]=lightdir[1];
   global_lightdir[2]=lightdir[2];

   global_lightbias=lightbias;
   global_lightoffset=lightoffset;
   }

// set direct texturing parameters
void ministrip::settexturedirectparams(int num,
                                       float lightdir[3],
                                       float transbias,float transoffset)
   {
   setpixshaderparams(num,lightdir[0],lightdir[1],lightdir[2],PI/2,5);
   setpixshaderparams(num,transbias,transoffset,0.0f,0.5f,4);
   }

// set global direct texturing parameters
void ministrip::setglobaltexturedirectparams(float lightdir[3],
                                             float transbias,float transoffset)
   {
   global_lightdir[0]=lightdir[0];
   global_lightdir[1]=lightdir[1];
   global_lightdir[2]=lightdir[2];

   global_transbias=transbias;
   global_transoffset=transoffset;
   }

// set fog parameters
void ministrip::setfogparams(int num,
                             float fogstart,float fogend,
                             float fogdensity,
                             float fogcolor[3])
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

// set global fog parameters
void ministrip::setglobalfogparams(float fogstart,float fogend,
                                   float fogdensity,
                                   float fogcolor[3])
   {
   global_fogstart=fogstart;
   global_fogend=fogend;

   global_fogdensity=fogdensity;

   global_fogcolor[0]=fogcolor[0];
   global_fogcolor[1]=fogcolor[1];
   global_fogcolor[2]=fogcolor[2];
   }

// set global shader
void ministrip::useglobalshader(int num)
   {USEGLOBALSHADER=num;}

// get global shader
int ministrip::getglobalshader()
   {return(USEGLOBALSHADER);}

// set actual shader
void ministrip::useshader(int num)
   {USESHADER=num;}

// get actual shader
int ministrip::getshader()
   {return(USESHADER);}

// check whether or not the geometry is fully opaque
BOOLINT ministrip::isopaque()
   {
   if (COLCOMPS<4) return(TRUE);

   return(COLAMIN>=1.0f);
   }

// render triangle strips
void ministrip::render(int wocolor,int wonrm,int wotex)
   {
   int shader;

   shader=USESHADER;
   if (shader<0) shader=USEGLOBALSHADER;

   if (wocolor==0)
      if (COLARRAY==NULL) color(COLR,COLG,COLB,COLA);

   if (wonrm==0)
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

   if (shader>=0)
      {
      enablevtxshader(shader);
      enablepixshader(shader);
      }

   vertexarray(VTXARRAY);
   if (wocolor==0) colorarray(COLARRAY,COLCOMPS);
   if (wonrm==0) normalarray(NRMARRAY);

   if (wotex==0)
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
   if (wocolor==0) colorarray(NULL);
   if (wonrm==0) normalarray(NULL);

   if (wotex==0)
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

   if (shader>=0)
      {
      disablevtxshader(shader);
      disablepixshader(shader);
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
void ministrip::rendermultipass(int pass,
                                int passes,
                                float ambient,
                                float bordercontrol,float centercontrol,float colorcontrol,
                                float bordercontrol2,float centercontrol2,float colorcontrol2,
                                float stripewidth,float stripeoffset,
                                float stripedx,float stripedy,float stripedz,
                                int correctz)
   {
   static minisurf surf;

   int dorender;

   if (passes<1 || passes>4) ERRORMSG();

   // enable external multi-pass shader
   dorender=surf.setextstate(1,
                             pass,passes,
                             ambient,
                             bordercontrol,centercontrol,colorcontrol,
                             bordercontrol2,centercontrol2,colorcontrol2,
                             stripewidth,stripeoffset,stripedx,stripedy,stripedz,
                             correctz);

   // render strips with external multi-pass shader
   if (dorender!=0) render();

   // disable external multi-pass shader
   surf.setextstate(0,
                    pass,passes,
                    ambient,
                    bordercontrol,centercontrol,colorcontrol,
                    bordercontrol2,centercontrol2,colorcontrol2,
                    stripewidth,stripeoffset,stripedx,stripedy,stripedz,
                    correctz);
   }

// render triangle strip with multi-pass method
void ministrip::rendermulti(int passes,
                            float ambient,
                            float bordercontrol,float centercontrol,float colorcontrol,
                            float bordercontrol2,float centercontrol2,float colorcontrol2,
                            float stripewidth,float stripeoffset,
                            float stripedx,float stripedy,float stripedz,
                            int correctz)
   {
   int i;

   int globalshader,shader;

   // disable regular shader
   globalshader=getglobalshader();
   useglobalshader(-1);
   shader=getshader();
   useshader(-1);

   // multi-pass rendering
   for (i=minisurf::FIRST_RENDER_PHASE; i<=minisurf::LAST_RENDER_PHASE; i++)
      rendermultipass(i,passes,
                      ambient,
                      bordercontrol,centercontrol,colorcontrol,
                      bordercontrol2,centercontrol2,colorcontrol2,
                      stripewidth,stripeoffset,
                      stripedx,stripedy,stripedz,
                      correctz);

   // enable previous shader
   useglobalshader(globalshader);
   useshader(shader);
   }

// render bbox
void ministrip::renderbbox()
   {
   renderline(miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMIN.z),miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMIN.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMIN.z),miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMIN.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMAX.z),miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMAX.z),miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMIN.z),miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMIN.z));
   renderline(miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMIN.z),miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMIN.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMAX.z),miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMAX.z),miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMIN.z),miniv3d(BBOXMIN.x,BBOXMIN.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMIN.z),miniv3d(BBOXMAX.x,BBOXMIN.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMIN.z),miniv3d(BBOXMIN.x,BBOXMAX.y,BBOXMAX.z));
   renderline(miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMIN.z),miniv3d(BBOXMAX.x,BBOXMAX.y,BBOXMAX.z));
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

// get bounding box
void ministrip::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
   {
   bboxmin=BBOXMIN;
   bboxmax=BBOXMAX;
   }

// get bounding sphere
void ministrip::getbsphere(miniv3d &center,double &radius2) const
   {
   center=0.5*(BBOXMIN+BBOXMAX);
   radius2=(0.5*(BBOXMAX-BBOXMIN)).getlength2();
   }

// shoot a ray and return the distance to the closest triangle
double ministrip::shoot(const miniv3d &o,const miniv3d &d,double mindist) const
   {
   int i;

   float *ptr;
   miniv3f v1,v2,v3;

   double dist,result;

   result=MAXFLOAT;

   if (!MTXSET)
      if (itest_ray_bbox(o,d,0.5*(BBOXMIN+BBOXMAX),0.5*(BBOXMAX-BBOXMIN)))
         {
         ptr=VTXARRAY;

         for (i=0; i<SIZE; i++)
            {
            v1.x=*ptr++;
            v1.y=*ptr++;
            v1.z=*ptr++;

            if (i>=2)
               {
               dist=ray_triangle_dist(o,d,v1,v2,v3);
               if (dist!=MAXFLOAT)
                  if (dist>=mindist)
                     if (dist<result) result=dist;

               v3=v2;
               v2=v1;
               }
            else if (i>=1)
               {
               v3=v2;
               v2=v1;
               }
            else v2=v1;
            }
         }

   return(result);
   }
