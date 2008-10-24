// (c) by Stefan Roettger

#include "miniio.h"

#include "miniOGL.h"
#include "minishader.h"

#include "miniutm.h"
#include "minicoord.h"
#include "miniwarp.h"

#include "miniearth.h"

// default constructor
miniearth::miniearth()
   {
   // configurable parameters:

   EPARAMS.warpmode=4;    // warp mode: linear=0 flat=1 flat_ref=2 affine=3 affine_ref=4

   EPARAMS.fps=25.0f;     // frames per second (target frame rate)

   EPARAMS.fovy=60.0f;    // field of view (degrees)
   EPARAMS.nearp=10.0f;   // near plane (meters)
   EPARAMS.farp=10000.0f; // far plane (meters)

   // feature switches:

   EPARAMS.usefog=FALSE;
   EPARAMS.useshaders=FALSE;
   EPARAMS.usediffuse=FALSE;
   EPARAMS.usevisshader=FALSE;
   EPARAMS.usebathymap=FALSE;
   EPARAMS.usecontours=FALSE;
   EPARAMS.usenprshader=FALSE;
   EPARAMS.useskydome=FALSE;
   EPARAMS.usewaypoints=FALSE;
   EPARAMS.usebricks=FALSE;
   EPARAMS.useearth=TRUE;
   EPARAMS.useflat=FALSE;

   // optional spherical fog:

   EPARAMS.fogcolor[0]=0.65f;
   EPARAMS.fogcolor[1]=0.7f;
   EPARAMS.fogcolor[2]=0.7f;

   EPARAMS.fogstart=0.5f;   // start of fog relative to far plane
   EPARAMS.fogdensity=0.5f; // relative fog density

   // optional void display:

   EPARAMS.voidstart=0.0f;    // end of atmosphere

   EPARAMS.voidcolor[0]=0.0f; // void color
   EPARAMS.voidcolor[1]=0.0f; // void color
   EPARAMS.voidcolor[2]=0.0f; // void color

   // optional sky-dome:

   EPARAMS.skydome="SkyDome.ppm"; // skydome file

   // optional earth globe:

   EPARAMS.lightdir=miniv3d(0.0,0.0,1.0); // directional light

   EPARAMS.lightbias=0.75f;   // lighting bias
   EPARAMS.lightoffset=0.25f; // lighting offset

   EPARAMS.transbias=4.0f;    // transition bias between night and day
   EPARAMS.transoffset=0.01f; // transition offset between night and day

   EPARAMS.shrinkZpass=FALSE; // shrink earth radius during Z rendering pass

   EPARAMS.frontname="EarthDay.ppm";  // file name of front earth PPM texture
   EPARAMS.backname="EarthNight.ppm"; // file name of back earth PPM texture

   EPARAMS.frontbuf=NULL; // front earth texture image buffer (databuf object)
   EPARAMS.backbuf=NULL;  // back earth texture image buffer (databuf object)

   EPARAMS.frontbufname="EarthDay.db";  // file name of front earth DB texture
   EPARAMS.backbufname="EarthNight.db"; // file name of back earth DB texture

   // initialize state:

   TERRAIN=new miniterrain();

   SKYDOME=new minisky();
   EARTH=new miniglobe();

   LOADED=FALSE;

   CLEAR=FALSE;

   FREEZE=FALSE;
   GRABBED=FALSE;

   RGBBUF=NULL;
   ZBUF=NULL;
   RGBTEXID=ZTEXID=0;

   DATAGRID=NULL;
   SORT=FALSE;

   initOGL();
   }

// destructor
miniearth::~miniearth()
   {
   delete TERRAIN;

   delete SKYDOME;
   delete EARTH;

   if (EPARAMS.frontbuf!=NULL)
      {
      EPARAMS.frontbuf->release();
      delete EPARAMS.frontbuf;
      }

   if (EPARAMS.backbuf!=NULL)
      {
      EPARAMS.backbuf->release();
      delete EPARAMS.backbuf;
      }

   if (GRABBED) freebuffers();
   }

// get parameters
void miniearth::get(MINIEARTH_PARAMS &eparams)
   {eparams=EPARAMS;}

// set parameters
void miniearth::set(MINIEARTH_PARAMS &eparams)
   {
   miniterrain::MINITERRAIN_PARAMS tparams;

   // set new state
   EPARAMS=eparams;

   // get the actual terrain state
   TERRAIN->get(tparams);

   // update the terrain state:

   tparams.fps=EPARAMS.fps;

   tparams.fovy=EPARAMS.fovy;
   tparams.nearp=EPARAMS.nearp;
   tparams.farp=EPARAMS.farp;

   tparams.usefog=EPARAMS.usefog;
   tparams.useshaders=EPARAMS.useshaders;
   tparams.usediffuse=EPARAMS.usediffuse;
   tparams.usevisshader=EPARAMS.usevisshader;
   tparams.usebathymap=EPARAMS.usebathymap;
   tparams.usecontours=EPARAMS.usecontours;
   tparams.usenprshader=EPARAMS.usenprshader;
   tparams.usewaypoints=EPARAMS.usewaypoints;
   tparams.usebricks=EPARAMS.usebricks;

   tparams.lightdir=EPARAMS.lightdir;

   tparams.lightbias=EPARAMS.lightbias;
   tparams.lightoffset=EPARAMS.lightoffset;

   tparams.fogcolor[0]=EPARAMS.fogcolor[0];
   tparams.fogcolor[1]=EPARAMS.fogcolor[1];
   tparams.fogcolor[2]=EPARAMS.fogcolor[2];

   tparams.fogstart=EPARAMS.fogstart;
   tparams.fogdensity=EPARAMS.fogdensity;

   if (EPARAMS.useflat)
      if (EPARAMS.warpmode==4) tparams.warpmode=2;
      else if (EPARAMS.warpmode==3) tparams.warpmode=1;
      else tparams.warpmode=0;
   else tparams.warpmode=EPARAMS.warpmode;

   // finally pass the updated terrain state
   TERRAIN->set(tparams);
   }

// propagate parameters
void miniearth::propagate()
   {set(EPARAMS);}

// initialize the OpenGL wrapper
void miniearth::initOGL()
   {
   // turn on hw-accelerated OpenGL mipmap generation
   miniOGL::configure_generatemm(1);

   // turn off on-the-fly OpenGL texture compression
   miniOGL::configure_compression(0);
   }

// load tileset (short version)
BOOLINT miniearth::load(const char *url,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(url,loadopts,reset));
   }

// load tileset (long version)
BOOLINT miniearth::load(const char *baseurl,const char *baseid,const char *basepath1,const char *basepath2,
                         BOOLINT loadopts,BOOLINT reset)
   {
   // propagate the parameters
   propagate();

   // load the tileset layer
   return(TERRAIN->load(baseurl,baseid,basepath1,basepath2,loadopts,reset));
   }

// load optional features
void miniearth::loadopts()
   {
   minilayer *ref;
   minilayer::MINILAYER_PARAMS lparams;

   if (LOADED) return;

   ref=TERRAIN->getlayer(TERRAIN->getreference());

   if (ref==NULL) return;

   ref->get(lparams);

   // load skydome:

   char *skyname=NULL;

   if (ref->getcache()!=NULL) skyname=ref->getcache()->getfile(EPARAMS.skydome,lparams.altpath);
   else
      {
      skyname=getfile(EPARAMS.skydome,lparams.altpath);
      if (skyname==NULL) skyname=getfile(EPARAMS.skydome,lparams.instpath);
      }

   if (skyname!=NULL)
      {
      SKYDOME->loadskydome(skyname);
      free(skyname);
      }

   // load earth textures:

   char *ename1=NULL;

   if (ref->getcache()!=NULL) ename1=ref->getcache()->getfile(EPARAMS.frontname,lparams.altpath);
   else
      {
      ename1=getfile(EPARAMS.frontname,lparams.altpath);
      if (ename1==NULL) ename1=getfile(EPARAMS.frontname,lparams.instpath);
      }

   if (ename1!=NULL)
      {
      EARTH->configure_frontname(ename1);
      free(ename1);
      }

   char *ename2=NULL;

   if (ref->getcache()!=NULL) ename2=ref->getcache()->getfile(EPARAMS.backname,lparams.altpath);
   else
      {
      ename2=getfile(EPARAMS.backname,lparams.altpath);
      if (ename2==NULL) ename2=getfile(EPARAMS.backname,lparams.instpath);
      }

   if (ename2!=NULL)
      {
      EARTH->configure_backname(ename2);
      free(ename2);
      }

   // load earth image buffers:

   char *ebname1=NULL;

   if (EPARAMS.frontbuf==NULL)
      {
      if (ref->getcache()!=NULL) ebname1=ref->getcache()->getfile(EPARAMS.frontbufname,lparams.altpath);
      else
         {
         ebname1=getfile(EPARAMS.frontbufname,lparams.altpath);
         if (ebname1==NULL) ebname1=getfile(EPARAMS.frontbufname,lparams.instpath);
         }

      if (ebname1!=NULL)
         {
         EPARAMS.frontbuf=new databuf;

         if (EPARAMS.frontbuf->loaddata(ebname1)!=0) EARTH->configure_frontbuf(EPARAMS.frontbuf);
         else
            {
            delete EPARAMS.frontbuf;
            EPARAMS.frontbuf=NULL;
            }

         free(ebname1);
         }
      }
   else EARTH->configure_frontbuf(EPARAMS.frontbuf);

   char *ebname2=NULL;

   if (EPARAMS.backbuf==NULL)
      {
      if (ref->getcache()!=NULL) ebname2=ref->getcache()->getfile(EPARAMS.backbufname,lparams.altpath);
      else
         {
         ebname2=getfile(EPARAMS.backbufname,lparams.altpath);
         if (ebname2==NULL) ebname2=getfile(EPARAMS.backbufname,lparams.instpath);
         }

      if (ebname2!=NULL)
         {
         EPARAMS.backbuf=new databuf;

         if (EPARAMS.backbuf->loaddata(ebname2)!=0) EARTH->configure_backbuf(EPARAMS.backbuf);
         else
            {
            delete EPARAMS.backbuf;
            EPARAMS.backbuf=NULL;
            }

         free(ebname2);
         }
      }
   else EARTH->configure_backbuf(EPARAMS.backbuf);

   LOADED=TRUE;
   }

// get file
char *miniearth::getfile(const char *src_file,const char *altpath)
   {
   char *file;

   if (checkfile(src_file)) return(strdup(src_file));

   file=strcct(altpath,src_file);

   if (file!=NULL)
      if (checkfile(file)) return(file);
      else free(file);

   return(NULL);
   }

// get initial view point
minicoord miniearth::getinitial()
   {return(TERRAIN->getinitial());}

// set initial eye point
void miniearth::initeyepoint(const minicoord &e)
   {TERRAIN->initeyepoint(e);}

// clear scene
void miniearth::clear()
   {CLEAR=TRUE;}

// generate and cache scene for a particular eye point and time step
void miniearth::cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect,double time)
   {if (!FREEZE) TERRAIN->cache(e,d,u,aspect,time);}

// render cached scene
void miniearth::rendercache()
   {
   minilayer *ref;
   minilayer::MINILAYER_PARAMS lparams;

   minicoord egl;

   double alt,altf,fogf;

   miniwarp warp;

   miniv4d mtx[3];
   double oglmtx[16];

   miniv3d lgl;
   float light[3];

   miniterrain::MINITERRAIN_PARAMS tparams;
   int seamode;

   ref=getreference();

   if (ref!=NULL)
      {
      ref->get(lparams);

      egl=ref->map_g2o(lparams.eye);

      // calculate void display factor
      if (EPARAMS.voidstart<=0.0f) altf=0.0;
      else
         {
         if (ref->get()->warpmode==1 || ref->get()->warpmode==2)
            {
            alt=miniv3d((lparams.eye-ref->getcenter()).vec)*ref->getnormal();

            altf=alt/EPARAMS.voidstart;
            }
         else
            {
            alt=miniv3d((lparams.eye-getearth()->getcenter()).vec).getlength();
            alt-=miniutm::EARTH_radius;

            altf=alt/EPARAMS.voidstart;
            }

         if (altf<0.0) altf=0.0;
         else if (altf>1.0) altf=1.0;

         altf=altf*altf;
         }

      // clear back buffer
      if (CLEAR)
         {
         clearwindow((1.0-altf)*EPARAMS.fogcolor[0]+altf*EPARAMS.voidcolor[0],
                     (1.0-altf)*EPARAMS.fogcolor[1]+altf*EPARAMS.voidcolor[1],
                     (1.0-altf)*EPARAMS.fogcolor[2]+altf*EPARAMS.voidcolor[2]);

         CLEAR=FALSE;
         }

      // enable fog
      if (EPARAMS.usefog)
         {
         fogf=(1.0-altf)*EPARAMS.fogstart+altf;

         enablefog(fogf*ref->len_g2o(EPARAMS.farp),
                   ref->len_g2o(EPARAMS.farp),
                   EPARAMS.fogcolor[0],
                   EPARAMS.fogcolor[1],
                   EPARAMS.fogcolor[2]);
         }

      // draw skydome
      if (EPARAMS.useskydome || EPARAMS.voidstart==0.0f)
         if (ref->get()->warpmode==0 || ref->get()->warpmode==2)
            {
            SKYDOME->setpos(egl.vec.x,egl.vec.y,egl.vec.z,
                            1.9*ref->len_g2o(EPARAMS.farp));

            SKYDOME->drawskydome();
            }

      // render earth globe (without Z writing)
      if (EPARAMS.useearth)
         if (ref->get()->warpmode!=0)
            {
            EARTH->setscale(ref->len_o2g(1.0));
            EARTH->setdynscale(1.0);

            warp=*getearth()->getwarp();
            warp.setwarp(miniwarp::MINIWARP_INTERNAL,miniwarp::MINIWARP_FINAL);
            warp.getwarp(mtx);

            oglmtx[0]=mtx[0].x;
            oglmtx[1]=mtx[1].x;
            oglmtx[2]=mtx[2].x;
            oglmtx[3]=0.0;

            oglmtx[4]=mtx[0].y;
            oglmtx[5]=mtx[1].y;
            oglmtx[6]=mtx[2].y;
            oglmtx[7]=0.0;

            oglmtx[8]=mtx[0].z;
            oglmtx[9]=mtx[1].z;
            oglmtx[10]=mtx[2].z;
            oglmtx[11]=0.0;

            oglmtx[12]=mtx[0].w;
            oglmtx[13]=mtx[1].w;
            oglmtx[14]=mtx[2].w;
            oglmtx[15]=1.0;

            EARTH->setmatrix(oglmtx);

            lgl=getnull()->rot_g2i(EPARAMS.lightdir,getnull()->getcenter());

            light[0]=lgl.x;
            light[1]=lgl.y;
            light[2]=lgl.z;

            if (EPARAMS.usediffuse)
               {
               EARTH->setshadedirectparams(light,EPARAMS.lightbias,EPARAMS.lightoffset);
               EARTH->settexturedirectparams(light,EPARAMS.transbias,EPARAMS.transbias*EPARAMS.transoffset);
               }
            else
               {
               EARTH->setshadedirectparams(light,0.0f,1.0f);
               EARTH->settexturedirectparams(light,0.0f,1.0f);
               }

            fogf=(1.0-altf)*EPARAMS.fogstart/2.0f+altf;

            EARTH->setfogparams((EPARAMS.usefog)?fogf*ref->len_g2o(EPARAMS.farp):0.0f,
                                (EPARAMS.usefog)?ref->len_g2o(EPARAMS.farp):0.0f,
                                EPARAMS.fogdensity,
                                EPARAMS.fogcolor);

            EARTH->render(MINIGLOBE_FIRST_RENDER_PHASE);
            }

      // check data grid for underwater volumes
      if (DATAGRID!=NULL)
         {
         // get the actual terrain state
         TERRAIN->get(tparams);

         // use stippling if there are underwater volumes
         if (DATAGRID->isbelowsealevel()) seamode=1;
         else seamode=0;

         // pass the updated terrain state
         if (seamode!=tparams.seamode)
            {
            tparams.seamode=seamode;
            TERRAIN->set(tparams);
            }
         }

      // render terrain
      TERRAIN->render();

      // render earth globe (without RGB writing)
      if (EPARAMS.useearth)
         if (ref->get()->warpmode!=0 && ref->get()->warpmode!=1 && ref->get()->warpmode!=2)
            {
            if (EPARAMS.shrinkZpass)
               EARTH->setdynscale(1.0-ref->get()->maxelev/miniutm::EARTH_radius);

            EARTH->render(MINIGLOBE_LAST_RENDER_PHASE);
            }

      // disable fog
      if (EPARAMS.usefog) disablefog();
      }
   }

// render data grid
void miniearth::renderdgrid()
   {
   minilayer *ref;
   minilayer::MINILAYER_PARAMS lparams;

   miniwarp warp;
   miniv4d mtx[3];

   ref=getreference();

   if (ref!=NULL)
      {
      ref->get(lparams);

      // trigger data grid
      if (DATAGRID!=NULL)
         {
         // construct mesh
         DATAGRID->construct();

         // check if constructed mesh is empty
         if (!DATAGRID->isempty())
            {
            DATAGRID->usemtxpost(FALSE);

            // set post matrix (world to rendering coordinates)
            if (ref->getwarp()!=NULL)
               {
               warp=*ref->getwarp();
               warp.setwarp(miniwarp::MINIWARP_METRIC,miniwarp::MINIWARP_FINAL);
               warp.getwarp(mtx);

               DATAGRID->specmtxpost(mtx);
               DATAGRID->usemtxpost(TRUE);
               }

            // push either sorted or unsorted mesh
            if (!SORT) DATAGRID->trigger(lparams.time);
            else DATAGRID->trigger(lparams.time,lparams.eye.vec,lparams.dir,lparams.nearp,lparams.farp,lparams.fovy,lparams.aspect,MAXFLOAT,ZTEXID);
            }
         }
      }
   }

// grab scene
void miniearth::grabbuffers()
   {
   int startx,starty;

   if (!EPARAMS.useshaders)
      {
      // get viewport dimensions
      getviewport(&startx,&starty,&BUFWIDTH,&BUFHEIGHT);

      // read rgb and z channels
      RGBBUF=readRGBpixels(startx,starty,BUFWIDTH,BUFHEIGHT);
      ZBUF=readZpixels(startx,starty,BUFWIDTH,BUFHEIGHT);
      }
   else
      {
      // copy rgb and z channels
      RGBTEXID=copytexrect();
      ZTEXID=copytexrect(1);
      }
   }

// draw scene
void miniearth::drawbuffers()
   {
   int startx,starty;
   int width,height;

   if (!EPARAMS.useshaders)
      {
      // get viewport dimensions
      getviewport(&startx,&starty,&width,&height);

      // write rgb and z channels
      writeRGBpixels(RGBBUF,BUFWIDTH,BUFHEIGHT,width,height,startx,starty);
      writeZpixels(ZBUF,BUFWIDTH,BUFHEIGHT,width,height,startx,starty);
      }
   else
      {
      // paint rgb and z channels
      painttexrect(RGBTEXID);
      painttexrect(ZTEXID,1);
      }
   }

// free scene
void miniearth::freebuffers()
   {
   if (!EPARAMS.useshaders)
      {
      free(RGBBUF);
      free(ZBUF);

      RGBBUF=NULL;
      ZBUF=NULL;
      }
   else
      {
      deletetexrect(RGBTEXID);
      deletetexrect(ZTEXID);

      RGBTEXID=ZTEXID=0;
      }
   }

// render scene
void miniearth::render()
   {
   // render cached scene
   if (!FREEZE || !GRABBED) rendercache();

   if (FREEZE)
      if (GRABBED) drawbuffers(); // draw buffers
      else
         {
         // grab buffers
         grabbuffers();
         GRABBED=TRUE;
         }
   else
      {
      // free buffers
      if (GRABBED) freebuffers();
      GRABBED=FALSE;
      }

   // render data grid
   renderdgrid();
   }

// freeze scene
void miniearth::freeze(BOOLINT flag)
   {FREEZE=flag;}

// add datagrid object
void miniearth::addgrid(datagrid *obj,BOOLINT sort)
   {
   DATAGRID=obj;
   SORT=sort;
   }

// shoot a ray at the scene
double miniearth::shoot(const minicoord &o,const miniv3d &d,double hitdist)
   {
   double t;

   minilayer *ref;

   ref=getreference();

   // check for hit with terrain
   t=TERRAIN->shoot(o,d,hitdist);

   // check for hit with earth ellipsoid
   if (t==MAXFLOAT)
      if (EPARAMS.useearth)
         if (ref->get()->warpmode!=0)
            if (ref->get()->warpmode!=1 && ref->get()->warpmode!=2)
               t=intersect_ellipsoid(miniv3d(o.vec),d,
                                     miniv3d(0.0,0.0,0.0),miniutm::WGS84_r_major,miniutm::WGS84_r_major,miniutm::WGS84_r_minor);
            else
               t=intersect_plane(miniv3d(o.vec),d,
                                 miniv3d(ref->getcenter().vec),ref->getnormal());

   return(t);
   }

// ray/sphere intersection
double miniearth::intersect_unitsphere(miniv3d p,miniv3d d)
   {
   double a,b,c;
   double s,r;

   double t1,t2;

   a=2*d*d;
   b=2*p*d;
   c=2*(p*p-1.0);

   r=b*b-a*c;

   if (r<0.0) return(MAXFLOAT);

   s=sqrt(r);

   t1=(-b+s)/a;
   t2=(-b-s)/a;

   if (t1<0.0)
      if (t2<0.0) return(MAXFLOAT);
      else return(t2);
   else
      if (t2<0.0) return(t1);
      else if (t1<t2) return(t1);
      else return(t2);
   }

// ray/ellipsoid intersection
double miniearth::intersect_ellipsoid(miniv3d p,miniv3d d,
                                      miniv3d o,double r1,double r2,double r3)
   {
   p-=o;

   p.x/=r1;
   p.y/=r2;
   p.z/=r3;

   d.x/=r1;
   d.y/=r2;
   d.z/=r3;

   return(intersect_unitsphere(p,d));
   }

// ray/plane intersection
double miniearth::intersect_plane(miniv3d p,miniv3d d,
                                  miniv3d o,miniv3d n)
   {
   double c;

   c=d*n;

   if (c>=0.0) return(MAXFLOAT);

   return(n*(o-p)/c);
   }

// extract triangles that [possibly] intersect a plane
minidyna<miniv3d> miniearth::extract(const minicoord &p,const miniv3d &v,double radius)
   {return(TERRAIN->extract(p,v,radius));}

// set locking callbacks
void miniearth::setraycallbacks(void (*lock)(void *data),void *data,
                                void (*unlock)(void *data))
   {miniray::setcallbacks(lock,data,unlock);}
