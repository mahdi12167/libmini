// (c) by Stefan Roettger

#undef PTHREADS // enable this if pthreads are installed

#define DEMO_REMOTEID "FraenkischeTileset/"
#define DEMO_ELEVTILESETFILE "elev.tileset.sav"
#define DEMO_IMAGTILESETFILE "imag.tileset.sav"
#define DEMO_STARTUPFILE "startup.sav"

#include "mini/minibase.h"

#include "mini/miniOGL.h"
#include "mini/minitime.h"
#include "mini/miniio.h"
#include "mini/miniload.h"
#include "mini/minicache.h"
#include "mini/minitree.h"
#include "mini/minipoint.h"
#include "mini/minitext.h"
#include "mini/minisky.h"

#include "mini/pnmbase.h"
#include "mini/pnmsample.h"

#include "mini/database.h"
#include "mini/datacloud.h"
#include "mini/datacache.h"

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#ifdef PTHREADS

#include <pthread.h>

static const int numthreads=10;

pthread_t pthread[numthreads];
pthread_mutex_t mutex;
pthread_attr_t attr;

void threadinit()
   {
   pthread_mutex_init(&mutex,NULL);

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
   }

void threadexit()
   {
   pthread_mutex_destroy(&mutex);
   pthread_attr_destroy(&attr);
   }

void startthread(void *(*thread)(void *background),backarrayelem *background,void *data)
   {pthread_create(&pthread[background->background-1],&attr,thread,background);}

void jointhread(backarrayelem *background,void *data)
   {
   void *status;
   pthread_join(pthread[background->background-1],&status);
   }

void lock_cs(void *data)
   {pthread_mutex_lock(&mutex);}

void unlock_cs(void *data)
   {pthread_mutex_unlock(&mutex);}

#endif

static int winwidth,winheight,winid;

// SRTM
static char *srtmmaps[]={"FraenkischeTileset/tiles/SRTM-DEM-BBox.demo.pgm",
                         "GTOPO30/World.W020N90.map.pgm", // fill SRTM holes
                         "SRTM/N47E010.pgm",
                         "SRTM/N47E011.pgm",
                         "SRTM/N47E012.pgm",
                         "SRTM/N47E013.pgm",
                         "SRTM/N48E010.pgm",
                         "SRTM/N48E011.pgm",
                         "SRTM/N48E012.pgm",
                         "SRTM/N48E013.pgm",
                         "SRTM/N49E010.pgm",
                         "SRTM/N49E011.pgm",
                         "SRTM/N49E012.pgm",
                         "SRTM/N49E013.pgm",
                         "SRTM/N50E010.pgm",
                         "SRTM/N50E011.pgm",
                         "SRTM/N50E012.pgm",
                         "SRTM/N50E013.pgm"};

// LandSat ETM and MSS orthophotos
static char *landmaps[]={"FraenkischeTileset/landsat/LandSat-ETM-BBox.demo.ppm",
                         "LandSat-ETM/p193r025.ppm",
                         "LandSat-ETM/p193r026.ppm",
                         "LandSat-ETM/p194r025.ppm", // cloudy -> poor quality
                         "LandSat-MSS/p208r25_1m19730528.ppm",
                         "LandSat-MSS/p208r26_1m19730528.ppm",
                         "LandSat-MSS/p209r25_3m19790516.ppm",
                         "LandSat-MSS/p209r26_2m19760609.ppm", // cloudy -> poor quality
                         "LandSat-MSS/p208r27_1m19720813.ppm", // snowy -> very poor quality
                         "LandSat-MSS/p209r27_1m19750606.ppm"}; // snowy -> very poor quality

// Tree maps
static char *treemaps[]={"FraenkischeTileset/trees/LandSat-NDVI-BBox.demo.pgm",
                         "LandSat-NDVI/p193r026-trees.pgm"};

// base path to the generated tiles and textures
static char basepath1[]="tiles";
static char basepath2[]="landsat";
static char basepath3[]="trees";

// number of tiles
static const int tiles=30;

// number of downsampling levels
static const int down=0;

// maximum size of tiles
static const int maxsize=2000;
static const int treesize=100;

// reduced scale
static const float scale=50.0f; // 1:50

// vertical exaggeration
static const float exaggeration=2.0f; // exaggeration=200%
static const float exaggertrees=3.0f; // exaggeration=300%

// tree parameters
static const float treescale=0.1f;
static const float treebase=0.5f;
static const float treeheight=12.5f;
static const float treestep=1.7f;
static const float treevalue=1.5f;

// tree rendering mode
static const int treemode1=3;
static const int treemode2=8;
static int treemode=treemode1;

// fog and sky color
static GLfloat fogcolor[]={0.85f,0.9f,1.0f,1.0f};

// number of columns and rows
static int cols,rows;

// resampling output parameters
static float outparams[5];

// the terrain and its cache
static miniload terrain;
static minicache cache;

// the tile cache
static datacache *tilecache;

// the trees
static minitree *trees;

// the waypoints
static minipoint points;

// the sky dome
static minisky skydome;

// frames per second
static const float fps=30.0f;
static float winfps=fps;
static const int fpu=100;
static int upd=fpu;

// global parameters
static const float res=2.0E6f;
static const float minres=25.0f;
static const float bsafety=0.05f;

// view frustum
static const float fovy=60.0f,nearplane=50.0f,farplane=50000.0f;

// view point and direction
static float ex,ey,ez,ay,angle,turn,pitch,incline;

// stereo base
static const float sbase=0.2f;

// gliding parameters
static const float minspeed=5.0f,maxspeed=500.0f,accel=0.1f,gravity=10.0f,eyeheight=1.81f,damping=200.0f;

// steering parameters
static const float oneturn=5.0f,minturn=0.1f,oneincline=10.0f,minincline=0.1f;

// flying parameters
static const float boost=0.3f,maxelev=11000.0f;

// viewing height and range
static float height=eyeheight,nearp=nearplane,farp=farplane;

// gliding speed
static float speed,topspeed;

// wire frame flag
static int wireframe=0;

// performance statistics
static float peak=0.0f,avg=0.0f,load=0.0f;
static int frames=0;

// initial viewpoint on the Baumleite at Walkersbrunn
static const float viewx=11.22097f; // easting
static const float viewy=49.64927f; // northing
static const float viewa=265.0f; // angle
static const float viewp=0.0f; // pitch

// height and range of waypoint markers
static const float markerheight=100.0f;
static const float markerrange=0.05f;

// approximate value of one arc-second in meters
static float arcsec[3];

// command line switches
static int sw_quit=0;
static int sw_stereo=0;
static int sw_anaglyph=0;
static int sw_trees=0;
static int sw_record=0;
static int sw_replay=0;
static int sw_full=0;
static int sw_exagger=1;
static int sw_dropwp=1;
static int sw_demo=1;

// output switch
static int output=0;

// record file
static FILE *record=NULL;
static char recordfile[]="Record.txt";
static int actframe=0,newframe=0;
static char key='\0';

void initview(float x,float y,float a,float p)
   {
   float elev;
   BOOLINT addfog=FALSE;

   if (trees!=NULL) addfog=TRUE;

   ex=(x-viewx)*3600*arcsec[0];
   ez=(y-viewy)*3600*arcsec[1];

   elev=terrain.getheight(ex,-ez)+(addfog?terrain.getfogheight(ex,-ez):0.0f);
   ey=elev+exaggeration*height;

#ifdef PTHREADS

   terrain.restrictroi(ex,-ez,farp/scale/3);

   terrain.updateroi(res,
                     ex,ey,-ez,
                     ex,-ez,farp/scale);

#endif

   ay=0.0f;

   angle=turn=a;
   pitch=incline=p;

   speed=topspeed=0.0f;

   upd=1;
   }

void request_callback(char *file,int istexture,databuf *buf,void *data)
   {buf->loadPNMdata(file);}

void reshapefunc(int width,int height)
   {
   if (sw_full==0)
      {
      winwidth=width;
      winheight=height;
      }
   else
      {
      winwidth=glutGameModeGet(GLUT_GAME_MODE_WIDTH);
      winheight=glutGameModeGet(GLUT_GAME_MODE_HEIGHT);
      winfps=glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE);

      if (winwidth<=0 || winheight<=0 || winfps<=0.0f)
         {
         winwidth=width;
         winheight=height;
         winfps=fps;
         }
      }

   if (winwidth<1) winwidth=1;
   if (winheight<1) winheight=1;

   glViewport(0,0,winwidth,winheight);

   tilecache->setloader(request_callback,NULL,1,1.25f*farp/scale,terrain.calcrange(30.0f/scale,winheight,fovy),128,3,100,1000);
   }

void keypressed(unsigned char key)
   {
   float elev,coef;

   BOOLINT addfog=FALSE;

   if (trees!=NULL)
      if (trees->getmode()<3) addfog=TRUE;

   if (sw_record!=0)
      fprintf(record,"frame=%d key=%d\n",actframe,key);

   if (key=='0') initview(viewx,viewy,viewa,viewp);
   else if (key=='w' || key=='W'|| key==' ')
      if (topspeed==0.0f) topspeed=maxspeed/scale;
      else topspeed=0.0f;
   else if (key=='a' || key=='A')
      {
      elev=terrain.getheight(ex,-ez)+(addfog?terrain.getfogheight(ex,-ez):0.0f);

      coef=(ey-elev-exaggeration*height)/(exaggeration*height);
      if (coef<1.0f) coef=1.0f;
      else if (coef>3.0f) coef=3.0f;

      angle-=oneturn/coef;

      if (angle<0.0f)
         {
         angle+=360.0f;
         turn+=360.0f;
         }
      }
   else if (key=='d' || key=='D')
      {
      elev=terrain.getheight(ex,-ez)+(addfog?terrain.getfogheight(ex,-ez):0.0f);

      coef=(ey-elev-exaggeration*height)/(exaggeration*height);
      if (coef<1.0f) coef=1.0f;
      else if (coef>3.0f) coef=3.0f;

      angle+=oneturn/coef;

      if (angle>360.0f)
         {
         angle-=360.0f;
         turn-=360.0f;
         }
      }
   else if (key=='s'|| key=='S')
      if (topspeed==0.0f) topspeed=-maxspeed/scale;
      else topspeed=0.0f;
   else if (key=='<')
      {
      pitch+=oneincline;
      if (pitch>80.0f) pitch=80.0f;
      }
   else if (key=='>')
      {
      pitch-=oneincline;
      if (pitch<-80.0f) pitch=-80.0f;
      }
   else if (key=='j') ay=exaggeration*height*fps;
   else if (key=='e')
      {
      sw_exagger=1-sw_exagger;
      if (sw_exagger!=0) terrain.setrelscale(1.0f);
      else terrain.setrelscale(1.0f/exaggeration);
      upd=1;
      }
   else if (key=='p') sw_dropwp=1-sw_dropwp;
   else if (key=='t')
      {
      if (treemode==treemode1) treemode=treemode2;
      else treemode=treemode1;
      trees->setmode(treemode);
      upd=1;
      }
   else if (key=='g')
      {
      treemode--;
      if (treemode<-2) treemode=-2;
      else
         {
         trees->setmode(treemode);
         upd=1;
         }
      }
   else if (key=='h')
      {
      treemode++;
      if (treemode>12) treemode=12;
      else
         {
         trees->setmode(treemode);
         upd=1;
         }
      }
   else if (key=='l')
      {
      wireframe=1-wireframe;
      if (wireframe!=0) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      }
   else if (key=='o') output=1-output;
   else if ((key=='q' && (sw_replay==0 || sw_record!=0)) || key==27)
      {
      delete tilecache;

#ifdef PTHREADS
      threadexit();
#endif

      if (trees!=NULL) delete trees;

      if (sw_replay!=0) fclose(record);

      if (sw_full==0) glutDestroyWindow(winid);
      else glutLeaveGameMode();

      if (frames>0)
         printf("performance: peak=%gMVtx/s avg=%gMVtx/s load=%d%%\n",
                peak/1.0E6f,avg/frames/1.0E6f,(int)(100.0f*load/frames));

      exit(0);
      }
   }

void keyboardfunc(unsigned char key,int x,int y)
   {
   if (sw_replay!=0 && sw_record==0)
      if (key!='l' && key!=27) return;

   keypressed(key);
   }

void drawpoints()
   {
   int i;

   const int maxinfo=1000;
   static char info[maxinfo];

   if (sw_dropwp!=0) return;

   points.calcvdata();

   minipointdata **vpoint=points.getvdata();
   minipointdata *nearest=points.getnearest(ex,ez,ey);

   if (vpoint==NULL) return;

   initstate();
   glLineWidth(2);
   glEnable(GL_LINE_SMOOTH);
   glBegin(GL_LINES);

   // mark all waypoints
   for (i=0; i<points.getvnum(); i++,vpoint++)
      {
      if (*vpoint==nearest) glColor3f(1.0f,0.0f,0.0f);
      else glColor3f(0.0f,0.0f,1.0f);
      glVertex3f((*vpoint)->x,(*vpoint)->height,-(*vpoint)->y);
      glVertex3f((*vpoint)->x,(*vpoint)->height+markerheight/scale,-(*vpoint)->y);
      }

   glEnd();

   // compile label information of nearest waypoint
   snprintf(info,maxinfo,"\n %s \n\n %s Lat=%s Lon=%s \n Elev=%s \n",
            nearest->desc,
            nearest->system,nearest->latitude,nearest->longitude,
            nearest->elevation);

   // label nearest waypoint
   glLineWidth(1);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);
   glPushMatrix();
   glTranslatef(0.0f,markerheight/scale,0.0f);
   glTranslatef(nearest->x,nearest->height,-nearest->y);
   glRotatef(-turn,0.0f,1.0f,0.0f);
   glScalef(2.0f*markerheight/scale,2.0f*markerheight/scale,2.0f*markerheight/scale);
   glTranslatef(-0.5f,0.0f,0.0f);
   minitext::drawstring(1.0f,240.0f,0.5f,0.5f,1.0f,info,1.0f,0.75f);
   glPopMatrix();

   vpoint=points.getvdata();
   for (i=0; i<points.getvnum(); i++,vpoint++)
      if (*vpoint!=nearest)
         if (points.getdistance2(ex,ez,ey,*vpoint)<fsqr(markerrange*farp/scale))
            {
            // compile label information of waypoint within range
            snprintf(info,maxinfo,"\n %s \n",(*vpoint)->desc);

            // label waypoint within range
            glPushMatrix();
            glTranslatef(0.0f,markerheight/scale,0.0f);
            glTranslatef((*vpoint)->x,(*vpoint)->height,-(*vpoint)->y);
            glRotatef(-turn,0.0f,1.0f,0.0f);
            glScalef(2.0f*markerheight/scale,2.0f*markerheight/scale,2.0f*markerheight/scale);
            glTranslatef(-0.5f,0.0f,0.0f);
            minitext::drawstring(1.0f,240.0f,0.5f,0.5f,1.0f,info,1.0f,0.75f);
            glPopMatrix();
            }

   glDisable(GL_LINE_SMOOTH);
   exitstate();
   }

void displayfunc()
   {
   double time,delta,idle;

   float sina,cosa,tanp;
   float elev,coef;
   float rx,rz;

   int vtx;
   float perf;

   BOOLINT addfog=FALSE;

   int value;

   if (trees!=NULL)
      if (trees->getmode()<3) addfog=TRUE;

   if (sw_replay!=0 && sw_record==0)
      while (newframe<=actframe)
         {
         keypressed(key);

         if (fscanf(record,"frame=%d key=%d\n",&newframe,&value)==2) key=(char)value;
         else
            {
            fclose(record);
            if ((record=fopen(recordfile,"rb"))==NULL) ERRORMSG();

            initview(viewx,viewy,viewa,viewp);
            actframe=-1;
            newframe=0;
            key='\0';
            break;
            }
         }

   actframe++;

   time=minigettime();

   sina=sin(2.0f*PI/360.0f*turn);
   cosa=cos(2.0f*PI/360.0f*turn);

   tanp=tan(2.0f*PI/360.0f*incline);

   ex+=sina*speed/fps;
   ez+=cosa*speed/fps;

   speed+=(fabs(topspeed-speed)>minspeed/scale?accel:1.0f)*(topspeed-speed);

   elev=terrain.getheight(ex,-ez)+(addfog?terrain.getfogheight(ex,-ez):0.0f);

   coef=(ey-elev-exaggeration*height)/(exaggeration*height);

   if (coef>1.0f) coef=1.0f;
   else if (coef<0.05f) coef=0.0f;

   ay-=coef*gravity/fps;
   ay*=pow(1.0f/damping,1.0f/fps);
   ey+=ay/fps;

   ey+=(boost-tanp)*speed*fmax(1.0f-ey/(maxelev/scale),0.0f)/fps;

   if (ey<elev+exaggeration*height)
      {
      ay=(elev+exaggeration*height-ey)*fps;
      ey=elev+exaggeration*height;
      }

   turn+=(fabs(angle-turn)>minturn?accel:1.0f)*(angle-turn);

   incline+=(fabs(pitch-incline)>minincline?accel:1.0f)*(pitch-incline);

   glClearColor(fogcolor[0],fogcolor[1],fogcolor[2],fogcolor[3]);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glFogfv(GL_FOG_COLOR,fogcolor);

   glFogi(GL_FOG_MODE,GL_LINEAR);
   glFogf(GL_FOG_START,0.5f*farp/scale);
   glFogf(GL_FOG_END,farp/scale);

   glEnable(GL_FOG);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fovy,(float)winwidth/winheight,nearp/scale,farp/scale);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(ex,ey,-ez,ex+sina,ey-tanp,-ez-cosa,0.0f,1.0f,0.0f);

   // update vertex arrays
   terrain.draw(res,
                ex,ey,-ez,
                sina,-tanp,-cosa,
                0.0f,1.0f,0.0f,
                fovy,(float)winwidth/winheight,
                nearp/scale,farp/scale,
                upd);

   upd=fpu;

   skydome.setpos(ex,0.0f,-ez,
                  2.0f*farp/scale,0.5f);

   if (sw_stereo==0)
      {
      // draw sky dome
      skydome.drawskydome();

      // render vertex arrays
      vtx=cache.rendercache();

      // draw waypoints
      drawpoints();
      }
   else
      {
      rx=cosa*sbase;
      rz=-sina*sbase;

      // left channel:

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(ex-rx,ey,-ez+rz,ex+sina-rx,ey-tanp,-ez-cosa+rz,0.0f,1.0f,0.0f);

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK_LEFT);
      else glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE);

      skydome.drawskydome();

      vtx=cache.rendercache();

      drawpoints();

      // right channel:

      glClear(GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(ex+rx,ey,-ez-rz,ex+sina+rx,ey-tanp,-ez-cosa-rz,0.0f,1.0f,0.0f);

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK_RIGHT);
      else glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_FALSE);

      skydome.drawskydome();

      vtx+=cache.rendercache();

      drawpoints();

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK);
      else glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
      }

   glutSwapBuffers();

   if (output)
      {
      printf("position: lat=%.5f lon=%.5f direction=%g elevation=%gm visible=[%d,%d]x[%d,%d] mem=%gMB texmem=%gMB fancnt=%d vtxcnt=%d\n",
             ez/arcsec[1]/3600+viewy,ex/arcsec[0]/3600+viewx,angle,elev*scale/exaggeration,
             terrain.getminitile()->getvisibleleft(),terrain.getminitile()->getvisibleright(),
             terrain.getminitile()->getvisiblebottom(),terrain.getminitile()->getvisibletop(),
             terrain.getmem(),terrain.gettexmem(),
             cache.getfancnt(),cache.getvtxcnt());

      datacloud *cloud=tilecache->getcloud();

      printf("streaming: pending=%d mem=%gMB\n",
             cloud->getpending(), // total number of pending tiles
             cloud->getmem()); // total memory foot print

      if (trees!=NULL)
         printf("trees: #%d of %gM\n",
                trees->gettrees(),0.5f*fsqr(2.0f*farp/50.0f)/1E6f);
      }

   delta=minigettime()-time;

   if (delta>0.0)
      {
      perf=vtx/delta;

      if (perf>peak) peak=perf;
      avg+=perf;

      load+=delta*fps;

      frames++;
      }

   idle=1.0f/winfps-delta;
   if (idle>0.0) miniwaitfor(idle);
   }

int main(int argc,char *argv[])
   {
   int i;

   // process command line options
   for (i=1; i<argc; i++)
      if (strcmp(argv[i],"-q")==0) sw_quit=1;
      else if (strcmp(argv[i],"-s")==0) sw_stereo=1;
      else if (strcmp(argv[i],"-a")==0) sw_anaglyph=1;
      else if (strcmp(argv[i],"-t")==0) sw_trees=1;
      else if (strcmp(argv[i],"-r")==0) sw_replay=1;
      else if (strcmp(argv[i],"-f")==0) sw_full=1;
      else if (strcmp(argv[i],"-e")==0) sw_exagger=0;
      else if (strcmp(argv[i],"-d")==0) sw_dropwp=0;
      else if (strcmp(argv[i],"-D")==0) sw_demo=0;

   // adjust options for demo mode
   if (sw_demo!=0)
      {
      sw_trees=1;
      sw_dropwp=0;
      sw_replay=1;
      treemode=3;
      }

   // check for replay option
   if (sw_replay!=0)
      {
      sw_record=(checkfile(recordfile)==0);
      if (sw_record!=0) record=fopen(recordfile,"wb");
      else record=fopen(recordfile,"rb");
      }

   // turn off supersampling
   configure_supersampling(0);

   // enable startup file
   configure_startupfile(1);

   // define tile set path
   configure_tilesetpath(DEMO_REMOTEID);

   // resample LandSat orthophotos
   resample(9,landmaps,
            tiles,down,maxsize,basepath2);

   // resample tree maps
   if (sw_trees!=0)
      resample(2,treemaps,
               tiles,down,treesize,basepath3);

   // resample SRTM
   resample(18,srtmmaps,
            tiles,down,maxsize,basepath1,
            NULL,NULL,NULL,NULL,NULL,-9999,
            &cols,&rows,outparams);

   // check for exit option
   if (sw_quit!=0) return(0);

   // open window with GLUT:

   winwidth=1024;
   winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   if (sw_stereo!=0 && sw_anaglyph==0) glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_STEREO);
   else glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   if (sw_full==0) winid=glutCreateWindow("Fraenkische Schweiz Demo");
   else glutEnterGameMode();

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

   // turn off on-the-fly texture compression
   miniOGL::configure_compression(0);

   // create tile cache
   tilecache=new datacache(&terrain);
   tilecache->setelevtilesetfile(DEMO_ELEVTILESETFILE);
   tilecache->setimagtilesetfile(DEMO_IMAGTILESETFILE);
   tilecache->setstartupfile(DEMO_STARTUPFILE);
   tilecache->setloader(request_callback,NULL);
   tilecache->getcloud()->setschedule(0.02,5.0); // upload for 20ms and keep for 5min
   tilecache->getcloud()->setmaxsize(256.0); // allow 256 MB tile cache size
   tilecache->setremoteid(DEMO_REMOTEID);
#ifdef PTHREADS
   tilecache->getcloud()->setthread(startthread,NULL,jointhread,lock_cs,unlock_cs);
   tilecache->configure_netthreads(numthreads);
   threadinit();
#endif

   // load persistent startup file
   tilecache->load();

   // get tile set info
   cols=tilecache->getelevinfo_tilesx();
   rows=tilecache->getelevinfo_tilesy();

   // load resampled tiles
   terrain.load(cols,rows, // number of columns and rows
                basepath1,basepath2,(sw_trees!=0)?basepath3:NULL, // directory for tiles, textures and fogmaps
                -viewx*3600,-viewy*3600, // horizontal offset in arc-seconds
                0.0f, // no vertical offset
                exaggeration,scale, // vertical exaggeration and global scale
                treescale*exaggertrees,treevalue, // fog parameters
                minres, // absolute minimum of global resolution
                bsafety, // absolute minimum of base offset safety
                outparams, // geometric output parameters
                arcsec); // one arcsec in meters

   // enable exaggeration
   if (sw_exagger!=0) terrain.setrelscale(1.0f);
   else terrain.setrelscale(1.0f/exaggeration);

   // tell pager about initial window size
   reshapefunc(winwidth,winheight);

   // define resolution reduction of invisible tiles
   terrain.getminitile()->setreduction(2.0f,3.0f);

   // attach vertex array cache
   cache.attach(terrain.getminitile());

   // create sky dome
   skydome.loadskydome("SkyDome.ppm",
                       outparams[2],0.0f,-outparams[3],
                       cols*outparams[0],rows*outparams[1]/(cols*outparams[0]));

   // load way points
   if (sw_demo!=0) points.load("Splash.txt",-viewy*3600,-viewx*3600,arcsec[0],arcsec[1],exaggeration/scale,terrain.getminitile());
   else
      {
      points.load("Waypoints.txt",-viewy*3600,-viewx*3600,arcsec[0],arcsec[1],exaggeration/scale,terrain.getminitile());
      points.load("ETC/Cities.txt",-viewy*3600,-viewx*3600,arcsec[0],arcsec[1],exaggeration/scale,terrain.getminitile());
      }

   // initialize trees
   if (sw_trees!=0)
      {
      trees=new minitree(&cache);

      trees->setmode_mx(treebase,0.0f,0.25f,0.05f,0.9f);
      trees->setmode_m2("Forest.ppm",250.0f/scale);
      trees->setmode_x(50.0f,5.0f,4,50.0f/scale,treeheight*exaggertrees*exaggeration/scale,70.0f/scale,60.0f/scale,0.0f,0.25f,0.05f);
      trees->setmode_3(0.1f);
      trees->setmode_4("TreeRGB.ppm","TreeA.pgm",0.0f,0.5f);
      trees->setmode_6(0.4f,0.7f);
      trees->setmode_7("TreesRGB.ppm","TreesA.pgm",4);
      trees->setmode_8(treestep*exaggertrees*exaggeration/scale,0.5f);
      trees->setmode_9("Soil.pvm",30.0f/scale,0.5f*exaggertrees*exaggeration/scale,0.005f,0.01f,1.25f,0.75f,5.0f/scale,8,1.0f);
      trees->setmode_10(250.0f/scale,1024,16,0.5f,0.0f,0.5f);
      trees->setmode_12(0.3f,0.6f,0.1f,0.25f,1.0f);
      trees->setmode(treemode);

      if (trees->getmode()>=3)
         {
         height/=4.0f;
         nearp/=4.0f;
         }

      if (trees->getmode()>=4)
         {
         height/=2.0f;
         nearp/=2.0f;
         }
      }
   else
      {
      trees=NULL;

      height/=8.0f;
      nearp/=8.0f;
      }

   // initialize view point
   initview(viewx,viewy,viewa,viewp);

   // enter event loop
   glutMainLoop();

   // never reached
   return(0);
   }
