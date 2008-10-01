// (c) by Stefan Roettger

#include "minibase.h"

#include "minishader.h"

unsigned char *minishader::VISBATHYMAP=NULL;
int minishader::VISBATHYWIDTH=0,minishader::VISBATHYHEIGHT=0,minishader::VISBATHYCOMPS=0;
int minishader::VISBATHYMOD=0;

unsigned char *minishader::NPRBATHYMAP=NULL;
int minishader::NPRBATHYWIDTH=0,minishader::NPRBATHYHEIGHT=0,minishader::NPRBATHYCOMPS=0;
int minishader::NPRBATHYMOD=0;

int minishader::CONTOURMODE=0;

int minishader::DETAILTEXMODE=0;
float minishader::DETAILTEXALPHA=0.0f;
int minishader::DETAILTEXMASK=0;

// enable vertex and pixel shader for VIS purposes
void minishader::setVISshader(minicache *cache,
                              float scale,float exaggeration,
                              float fogstart,float fogend,
                              float fogdensity,
                              float fogcolor[3],
                              float bathystart,float bathyend,
                              float contours,
                              float sealevel,float seabottom,
                              float seacolor[3],
                              float seatrans,float bottomtrans,
                              float bottomcolor[3],
                              float seamodulate)
   {
   char *frgprog;

   BOOLINT usefog,usemap,usecnt,usesea;

   float fog_a,fog_b,fog_c;
   float bathy_a,bathy_b,bathy_c;
   float cnt_a,cnt_b,cnt_c,cnt_d;
   float sea_a,sea_b;

   // fragment program for the terrain (initializer snippet, load color)
   static const char *frgprog1_i="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM l=program.env[2]; \n\
      PARAM p=program.env[3]; \n\
      PARAM o=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      PARAM c5=program.env[10]; \n\
      TEMP col,colt,colb,nrm,vtx,len,crd,opa,fog; \n\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n";

   // fragment program for the terrain (main snippet #1, load texture)
   static const char *frgprog1_s1="\
      ### fetch texture color \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      ### modulate with fragment color \n\
      MUL col,col,colt; \n";

   // fragment program for the terrain (main snippet #2, color mapping)
   static const char *frgprog1_s2="\
      ### blend with color map \n\
      SUB vtx.z,fragment.texcoord[0].z,c5.x; \n\
      MUL vtx.z,-vtx.z,c5.y; \n\
      MOV vtx.y,c5.z; \n\
      MAD vtx.x,vtx.z,t.x,t.y; \n\
      TEX colt,vtx,texture[1],2D; \n\
      LRP colt.xyz,colt.w,colt,c5.w; \n\
      MUL colt.xyz,colt,col; \n\
      CMP col.xyz,-vtx.z,colt,col; \n";

   // fragment program for the terrain (main snippet #3, fade sea)
   static const char *frgprog1_s3="\
      ### fade-out at sea bottom \n\
      SUB vtx.y,fragment.texcoord[0].z,c0.x; \n\
      MUL_SAT vtx.w,-vtx.y,c0.y; \n\
      SUB vtx.w,c0.w,vtx.w; \n\
      MAD vtx.w,vtx.w,-vtx.w,c0.w; \n\
      MUL vtx.w,vtx.w,c1.w; \n\
      LRP colb.xyz,vtx.w,c1,col; \n\
      CMP col.xyz,vtx.y,colb,col; \n";

   // fragment program for the terrain (main snippet #4, contour lines)
   static const char *frgprog1_s4="\
      ### modulate with contour lines \n\
      MUL vtx.y,fragment.texcoord[0].z,-c2.x; \n\
      FRC vtx.y,vtx.y; \n\
      MAD vtx.y,vtx.y,c2.z,-c2.w; \n\
      ABS vtx.y,vtx.y; \n\
      SUB vtx.y,c4.w,vtx.y; \n\
      MUL_SAT vtx.y,vtx.y,c2.y; \n\
      MUL col.xyz,col,vtx.y; \n";

   // fragment program for the terrain (main snippet #4, bathymetry contour lines)
   static const char *frgprog1_s4b="\
      ### modulate with contour lines \n\
      MUL vtx.x,fragment.texcoord[0].z,-c2.x; \n\
      FRC vtx.y,vtx.x; \n\
      MAD vtx.y,vtx.y,c2.z,-c2.w; \n\
      ABS vtx.y,vtx.y; \n\
      SUB vtx.y,c4.w,vtx.y; \n\
      MUL_SAT vtx.y,vtx.y,c2.y; \n\
      CMP vtx.y,vtx.x,c4.w,vtx.y; \n\
      MUL col.xyz,col,vtx.y; \n";

   // fragment program for the terrain (main snippet #5, load detail)
   static const char *frgprog1_s5l="\
      ## load detail texture \n\
      TEX colt,fragment.texcoord[2],texture[2],2D; \n";

   // fragment program for the terrain (main snippet #5, load+mask detail)
   static const char *frgprog1_s5lm="\
      ## load detail texture \n\
      TEX colt,fragment.texcoord[2],texture[2],2D; \n\
      ### check coordinate range \n\
      MOV crd,fragment.texcoord[2]; \n\
      CMP colt.a,crd.x,0.0,colt.a; \n\
      CMP colt.a,crd.y,0.0,colt.a; \n\
      SUB crd,1.0,crd; \n\
      CMP colt.a,crd.x,0.0,colt.a; \n\
      CMP colt.a,crd.y,0.0,colt.a; \n";

   // fragment program for the terrain (main snippet #6, overlay mode)
   static const char *frgprog1_s6o="\
      ## blend in detail texture \n\
      MUL opa.a,colt.a,o.a; \n\
      MUL opa.a,opa.a,c3.a; \n\
      LRP col,opa.a,colt,col; \n";

   // fragment program for the terrain (main snippet #6, modulate mode)
   static const char *frgprog1_s6m="\
      ### blend in detail texture \n\
      MUL opa.a,colt.a,o.a; \n\
      MUL opa.a,opa.a,c3.a; \n\
      SUB opa.x,c4.a,opa.a; \n\
      MAD colt,colt,opa.a,opa.x; \n\
      MUL col,col,colt; \n";

   // fragment program for the terrain (terminator snippet #1, directional shading)
   static const char *frgprog_t1="\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3_SAT nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n";

   // fragment program for the terrain (terminator snippet #2, spherical fog)
   static const char *frgprog_t2="\
      ### modulate with spherical fog \n\
      MOV fog.x,fragment.fogcoord.x; \n\
      MAD_SAT fog.x,fog.x,c3.x,c3.y; \n\
      POW fog.x,fog.x,c3.z; \n\
      LRP col.xyz,fog.x,c4,col; \n";

   // fragment program for the terrain (terminator snippet #3, write color)
   static const char *frgprog_t3="\
      ### write resulting color \n\
      MOV result.color,col; \n\
      END \n";

   // fragment program for the sea surface (initializer snippet, load color)
   static const char *frgprog2_i="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM l=program.env[2]; \n\
      PARAM p=program.env[3]; \n\
      PARAM o=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      TEMP col,colt,nrm,len,fog; \n\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n";

   // fragment program for the sea surface (main snippet, load texture)
   static const char *frgprog2_s="\
      ### fetch texture color \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      ### interpolate with fragment color \n\
      LRP col.xyz,c0.x,colt,col; \n";

   // set primary sea color
   cache->setseacolor(seacolor[0],seacolor[1],seacolor[2],seatrans);

   // use default vertex shader plugin
   cache->setvtxshader();
   cache->usevtxshader(1);

   // check parameters
   usefog=(fogstart<fogend);
   usemap=(exaggeration*(bathystart-bathyend)!=0.0f && VISBATHYMAP!=NULL);
   usecnt=(exaggeration*contours!=0.0f);
   usesea=(sealevel!=-MAXFLOAT && exaggeration*seabottom!=0.0f);

   // calculate the fog parameters
   if (usefog)
      {
      fog_a=fsqr(scale/fogend);
      fog_b=0.0f;
      fog_c=log(fmin(0.5f*fogdensity,1.0f))/(2.0f*log(fogstart/fogend/2.0f+0.5f));
      }
   else
      {
      fog_a=0.0f;
      fog_b=0.0f;
      fog_c=1.0f;
      }

   // calculate the bathymetry parameters
   if (usemap)
      {
      bathy_a=bathystart*exaggeration/scale;
      bathy_b=scale/(exaggeration*(bathystart-bathyend));
      bathy_c=0.5f;
      }
   else
      {
      bathy_a=0.0f;
      bathy_b=0.0f;
      bathy_c=0.0f;
      }

   // calculate the contour parameters
   if (usecnt)
      {
      cnt_a=scale/(exaggeration*fabs(contours));
      cnt_b=3.0f;
      cnt_c=2.0f;
      cnt_d=1.0f;
      }
   else
      {
      cnt_a=0.0f;
      cnt_b=1.0f;
      cnt_c=0.0f;
      cnt_d=0.0f;
      }

   // calculate the sea parameters
   if (usesea)
      {
      sea_a=sealevel*exaggeration/scale;
      sea_b=scale/(exaggeration*fabs(seabottom));
      }
   else
      {
      sea_a=0.0f;
      sea_b=0.0f;
      }

   // concatenate pixel shader
   if (!usemap)
      if (DETAILTEXMODE==1) frgprog=concatprog(frgprog1_i,frgprog1_s1,NULL,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6o,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, with detail texture overlay
      else if (DETAILTEXMODE==2) frgprog=concatprog(frgprog1_i,frgprog1_s1,NULL,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6m,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, with detail texture modulation
      else frgprog=concatprog(frgprog1_i,frgprog1_s1,NULL,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, without detail texture
   else if (seabottom<0.0f)
      if (DETAILTEXMODE==1) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6o,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping before the fade-out, with detail texture overlay
      else if (DETAILTEXMODE==2) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6m,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping before the fade-out, with detail texture modulation
      else frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,usesea?frgprog1_s3:NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping before the fade-out, without detail texture
   else
      if (DETAILTEXMODE==1) frgprog=concatprog(frgprog1_i,frgprog1_s1,usesea?frgprog1_s3:NULL,frgprog1_s2,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6o,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping after the fade-out, with detail texture overlay
      else if (DETAILTEXMODE==2) frgprog=concatprog(frgprog1_i,frgprog1_s1,usesea?frgprog1_s3:NULL,frgprog1_s2,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6m,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping after the fade-out, with detail texture modulation
      else frgprog=concatprog(frgprog1_i,frgprog1_s1,usesea?frgprog1_s3:NULL,frgprog1_s2,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping after the fade-out, without detail texture

   // use pixel shader plugin
   cache->setpixshader(frgprog);
   cache->setpixshaderparams(sea_a,sea_b,0.0f,1.0f);
   cache->setpixshaderparams(bottomcolor[0],bottomcolor[1],bottomcolor[2],bottomtrans,1);
   cache->setpixshaderparams(cnt_a,cnt_b,cnt_c,cnt_d,2);
   cache->setpixshaderparams(fog_a,fog_b,fog_c,DETAILTEXALPHA,3);
   cache->setpixshaderparams(fogcolor[0],fogcolor[1],fogcolor[2],1.0f,4);
   cache->setpixshaderparams(bathy_a,bathy_b,bathy_c,1.0f,5);
   cache->usepixshader(1);
   free(frgprog);

   // concatenate sea shader
   frgprog=concatprog(frgprog2_i,frgprog2_s,NULL,NULL,NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3);

   // use sea shader plugin
   cache->setseashader(frgprog);
   cache->setseashaderparams(seamodulate,0.0f,0.0f,0.0f);
   cache->setseashaderparams(fog_a,fog_b,fog_c,0.0f,3);
   cache->setseashaderparams(fogcolor[0],fogcolor[1],fogcolor[2],0.0f,4);
   cache->useseashader(1);
   free(frgprog);

   // set bathymetry color map
   if (VISBATHYMAP!=NULL && VISBATHYMOD!=0)
      {
      cache->setpixshadertex(VISBATHYMAP,VISBATHYWIDTH,VISBATHYHEIGHT,VISBATHYCOMPS);

      VISBATHYMOD=0;
      NPRBATHYMOD=1;
      }

   // check for sea texture binding
   if (seamodulate==0.0f) cache->configure_seaenabletex(0);
   else cache->configure_seaenabletex(1);
   }

// set bathymetry color map for VIS shader
void minishader::setVISbathymap(unsigned char *bathymap,
                                int bathywidth,int bathyheight,int bathycomps)
   {
   VISBATHYMAP=bathymap;

   VISBATHYWIDTH=bathywidth;
   VISBATHYHEIGHT=bathyheight;
   VISBATHYCOMPS=bathycomps;

   VISBATHYMOD=1;
   }

// enable vertex and pixel shader for NPR purposes
void minishader::setNPRshader(minicache *cache,
                              float scale,float exaggeration,
                              float fogstart,float fogend,
                              float fogdensity,
                              float fogcolor[3],
                              float bathystart,float bathyend,
                              float contours,
                              float fadefactor,
                              float sealevel,
                              float seacolor[3],float seatrans,
                              float seagray)
   {
   char *frgprog;

   BOOLINT usefog,usemap,usecnt;

   float fog_a,fog_b,fog_c;
   float bathy_a,bathy_b,bathy_c;
   float cnt_a,cnt_b,cnt_c,cnt_d;
   float npr_a,npr_b,npr_c,npr_d;

   // fragment program for the terrain (initializer snippet, load color)
   static const char *frgprog1_i="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM l=program.env[2]; \n\
      PARAM p=program.env[3]; \n\
      PARAM o=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      PARAM c5=program.env[10]; \n\
      TEMP col,colt,nrm,vtx,len,crd,opa,fog; \n\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n";

   // fragment program for the terrain (main snippet #1, load texture)
   static const char *frgprog1_s1="\
      ### fetch texture color \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      ### modulate with fragment color \n\
      MUL col,col,colt; \n";

   // fragment program for the terrain (main snippet #2, bleach color)
   static const char *frgprog1_s2="\
      ### bleach fragment color \n\
      DP3 col.xyz,col,c0; \n\
      ADD col.xyz,col,c0.w; \n\
      ### replace bathymetry \n\
      SUB vtx.y,fragment.texcoord[0].z,c1.w; \n\
      CMP col.xyz,vtx.y,c1,col; \n";

   // fragment program for the terrain (main snippet #3, color mapping)
   static const char *frgprog1_s3="\
      ### blend with color map \n\
      SUB vtx.z,fragment.texcoord[0].z,c5.x; \n\
      MUL vtx.z,-vtx.z,c5.y; \n\
      MOV vtx.y,c5.z; \n\
      MAD vtx.x,vtx.z,t.x,t.y; \n\
      TEX colt,vtx,texture[1],2D; \n\
      LRP colt.xyz,colt.w,colt,c5.w; \n\
      MUL colt.xyz,colt,col; \n\
      CMP col.xyz,-vtx.z,colt,col; \n";

   // fragment program for the terrain (main snippet #4, contour lines)
   static const char *frgprog1_s4="\
      ### modulate with contour lines \n\
      MUL vtx.y,fragment.texcoord[0].z,-c2.x; \n\
      FRC vtx.y,vtx.y; \n\
      MAD vtx.y,vtx.y,c2.z,-c2.w; \n\
      ABS vtx.y,vtx.y; \n\
      SUB vtx.y,c4.w,vtx.y; \n\
      MUL_SAT vtx.y,vtx.y,c2.y; \n\
      MUL col.xyz,col,vtx.y; \n";

   // fragment program for the terrain (main snippet #4, bathymetry contour lines)
   static const char *frgprog1_s4b="\
      ### modulate with contour lines \n\
      MUL vtx.x,fragment.texcoord[0].z,-c2.x; \n\
      FRC vtx.y,vtx.x; \n\
      MAD vtx.y,vtx.y,c2.z,-c2.w; \n\
      ABS vtx.y,vtx.y; \n\
      SUB vtx.y,c4.w,vtx.y; \n\
      MUL_SAT vtx.y,vtx.y,c2.y; \n\
      CMP vtx.y,vtx.x,c4.w,vtx.y; \n\
      MUL col.xyz,col,vtx.y; \n";

   // fragment program for the terrain (main snippet #5, load detail)
   static const char *frgprog1_s5l="\
      ## load detail texture \n\
      TEX colt,fragment.texcoord[2],texture[2],2D; \n";

   // fragment program for the terrain (main snippet #5, load+mask detail)
   static const char *frgprog1_s5lm="\
      ## load detail texture \n\
      TEX colt,fragment.texcoord[2],texture[2],2D; \n\
      ### check coordinate range \n\
      MOV crd,fragment.texcoord[2]; \n\
      CMP colt.a,crd.x,0.0,colt.a; \n\
      CMP colt.a,crd.y,0.0,colt.a; \n\
      SUB crd,1.0,crd; \n\
      CMP colt.a,crd.x,0.0,colt.a; \n\
      CMP colt.a,crd.y,0.0,colt.a; \n";

   // fragment program for the terrain (main snippet #6, overlay mode)
   static const char *frgprog1_s6o="\
      ## blend in detail texture \n\
      MUL opa.a,colt.a,o.a; \n\
      MUL opa.a,opa.a,c3.a; \n\
      LRP col,opa.a,colt,col; \n";

   // fragment program for the terrain (main snippet #6, modulate mode)
   static const char *frgprog1_s6m="\
      ### blend in detail texture \n\
      MUL opa.a,colt.a,o.a; \n\
      MUL opa.a,opa.a,c3.a; \n\
      SUB opa.x,c4.a,opa.a; \n\
      MAD colt,colt,opa.a,opa.x; \n\
      MUL col,col,colt; \n";

   // fragment program for the terrain (terminator snippet #1, directional shading)
   static const char *frgprog_t1="\
      ### modulate with directional light \n\
      MOV nrm,fragment.texcoord[1]; \n\
      DP3 len.x,nrm,nrm; \n\
      RSQ len.x,len.x; \n\
      MUL nrm,nrm,len.x; \n\
      DP3_SAT nrm.z,nrm,l; \n\
      MAD nrm.z,nrm.z,p.x,p.y; \n\
      MUL_SAT col.xyz,col,nrm.z; \n";

   // fragment program for the terrain (terminator snippet #2, spherical fog)
   static const char *frgprog_t2="\
      ### modulate with spherical fog \n\
      MOV fog.x,fragment.fogcoord.x; \n\
      MAD_SAT fog.x,fog.x,c3.x,c3.y; \n\
      POW fog.x,fog.x,c3.z; \n\
      LRP col.xyz,fog.x,c4,col; \n";

   // fragment program for the terrain (terminator snippet #3, write color)
   static const char *frgprog_t3="\
      ### write resulting color \n\
      MOV result.color,col; \n\
      END \n";

   // fragment program for the sea surface (initializer snippet, load color)
   static const char *frgprog2_i="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM l=program.env[2]; \n\
      PARAM p=program.env[3]; \n\
      PARAM o=program.env[4]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      PARAM c3=program.env[8]; \n\
      PARAM c4=program.env[9]; \n\
      TEMP col,colt,nrm,len,fog; \n\
      ### fetch fragment color \n\
      MOV col,fragment.color; \n";

   // fragment program for the sea surface (main snippet, load texture)
   static const char *frgprog2_s="\
      ### fetch texture color \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      ### interpolate with fragment color \n\
      LRP col.xyz,c0.x,colt,col; \n";

   // set primary sea color
   cache->setseacolor(seacolor[0],seacolor[1],seacolor[2],seatrans);

   // use default vertex shader plugin
   cache->setvtxshader();
   cache->usevtxshader(1);

   // check parameters
   usefog=(fogstart<fogend);
   usemap=(exaggeration*(bathystart-bathyend)!=0.0f && NPRBATHYMAP!=NULL);
   usecnt=(exaggeration*contours!=0.0f);

   // calculate the fog parameters
   if (usefog)
      {
      fog_a=fsqr(scale/fogend);
      fog_b=0.0f;
      fog_c=log(fmin(0.5f*fogdensity,1.0f))/(2.0f*log(fogstart/fogend/2.0f+0.5f));
      }
   else
      {
      fog_a=0.0f;
      fog_b=0.0f;
      fog_c=1.0f;
      }

   // calculate the bathymetry parameters
   if (usemap)
      {
      bathy_a=bathystart*exaggeration/scale;
      bathy_b=scale/(exaggeration*(bathystart-bathyend));
      bathy_c=0.5f;
      }
   else
      {
      bathy_a=0.0f;
      bathy_b=0.0f;
      bathy_c=0.0f;
      }

   // calculate the contour parameters
   if (usecnt)
      {
      cnt_a=scale/(exaggeration*fabs(contours));
      cnt_b=5.0f;
      cnt_c=2.0f;
      cnt_d=1.0f;
      }
   else
      {
      cnt_a=0.0f;
      cnt_b=1.0f;
      cnt_c=0.0f;
      cnt_d=0.0f;
      }

   // calculate the NPR parameters
   if (fadefactor<0.0f)
      {
      npr_a=0.3f*(1.0f+fadefactor);
      npr_b=0.6f*(1.0f+fadefactor);
      npr_c=0.1f*(1.0f+fadefactor);
      npr_d=0.0f;
      }
   else
      {
      npr_a=0.3f*(1.0f-fadefactor);
      npr_b=0.6f*(1.0f-fadefactor);
      npr_c=0.1f*(1.0f-fadefactor);
      npr_d=fadefactor;
      }

   // concatenate pixel shader
   if (!usemap)
      if (DETAILTEXMODE==1) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6o,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, with detail texture overlay
      else if (DETAILTEXMODE==2) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6m,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, with detail texture modulation
      else frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,NULL,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // without color mapping, without detail texture
   else
      if (DETAILTEXMODE==1) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,frgprog1_s3,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6o,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping, with detail texture overlay
      else if (DETAILTEXMODE==2) frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,frgprog1_s3,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,(DETAILTEXMASK==0)?frgprog1_s5l:frgprog1_s5lm,frgprog1_s6m,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping, with detail texture modulation
      else frgprog=concatprog(frgprog1_i,frgprog1_s1,frgprog1_s2,frgprog1_s3,usecnt?(CONTOURMODE==0)?frgprog1_s4:frgprog1_s4b:NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3); // with color mapping, without detail texture

   // use pixel shader plugin
   cache->setpixshader(frgprog);
   cache->setpixshaderparams(npr_a,npr_b,npr_c,npr_d);
   cache->setpixshaderparams(seagray,seagray,seagray,sealevel/scale,1);
   cache->setpixshaderparams(cnt_a,cnt_b,cnt_c,cnt_d,2);
   cache->setpixshaderparams(fog_a,fog_b,fog_c,DETAILTEXALPHA,3);
   cache->setpixshaderparams(fogcolor[0],fogcolor[1],fogcolor[2],1.0f,4);
   cache->setpixshaderparams(bathy_a,bathy_b,bathy_c,1.0f,5);
   cache->usepixshader(1);
   free(frgprog);

   // concatenate sea shader
   frgprog=concatprog(frgprog2_i,frgprog2_s,NULL,NULL,NULL,NULL,NULL,frgprog_t1,usefog?frgprog_t2:NULL,frgprog_t3);

   // use sea shader plugin
   cache->setseashader(frgprog);
   cache->setseashaderparams(0.0f,0.0f,0.0f,0.0f);
   cache->setseashaderparams(fog_a,fog_b,fog_c,0.0f,3);
   cache->setseashaderparams(fogcolor[0],fogcolor[1],fogcolor[2],0.0f,4);
   cache->useseashader(1);
   free(frgprog);

   // set bathymetry color map
   if (NPRBATHYMAP!=NULL && NPRBATHYMOD!=0)
      {
      cache->setpixshadertex(NPRBATHYMAP,NPRBATHYWIDTH,NPRBATHYHEIGHT,NPRBATHYCOMPS);

      NPRBATHYMOD=0;
      VISBATHYMOD=1;
      }
   }

// set bathymetry color map for NPR shader
void minishader::setNPRbathymap(unsigned char *bathymap,
                                int bathywidth,int bathyheight,int bathycomps)
   {
   NPRBATHYMAP=bathymap;

   NPRBATHYWIDTH=bathywidth;
   NPRBATHYHEIGHT=bathyheight;
   NPRBATHYCOMPS=bathycomps;

   NPRBATHYMOD=1;
   }

// set contour line mode (0=full 1=bathy)
void minishader::setcontourmode(int mode)
   {CONTOURMODE=mode;}

// set detail texturing mode (0=off 1=overlay 2=modulate)
void minishader::setdetailtexmode(int mode,float alpha,int mask)
   {
   DETAILTEXMODE=mode;
   DETAILTEXALPHA=alpha;
   DETAILTEXMASK=mask;
   }

// disable vertex and pixel shaders
void minishader::unsetshaders(minicache *cache)
   {
   cache->usevtxshader(0);
   cache->usepixshader(0);
   cache->useseashader(0);
   }

// concatenate shader program from snippets
char *minishader::concatprog(const char *s1,const char *s2,const char *s3,const char *s4,const char *s5,const char *s6,const char *s7,const char *s8,const char *s9,const char *s10)
   {
   char *prog1,*prog2;

   prog1=strcct(s1,s2);
   prog2=strcct(prog1,s3);
   free(prog1);
   prog1=strcct(prog2,s4);
   free(prog2);
   prog2=strcct(prog1,s5);
   free(prog1);
   prog1=strcct(prog2,s6);
   free(prog2);
   prog2=strcct(prog1,s7);
   free(prog1);
   prog1=strcct(prog2,s8);
   free(prog2);
   prog2=strcct(prog1,s9);
   free(prog1);
   prog1=strcct(prog2,s10);
   free(prog2);

   return(prog1);
   }
