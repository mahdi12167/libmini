// (c) by Stefan Roettger

#undef PTHREADS // enable this if pthreads are installed

#include <mini/minibase.h>

#include <mini/miniOGL.h>
#include <mini/minitime.h>
#include <mini/miniio.h>
#include <mini/minihsv.h>
#include <mini/minicrs.h>
#include <mini/miniload.h>
#include <mini/minicache.h>
#include <mini/minibrick.h>
#include <mini/minipoint.h>
#include <mini/minitext.h>
#include <mini/minisky.h>

#include <mini/pnmbase.h>
#include <mini/pnmsample.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#ifdef PTHREADS

#include <pthread.h>

const int pthreads=2;

pthread_t pthread[pthreads];
pthread_mutex_t mutex1[pthreads],mutex2[pthreads];
pthread_attr_t attr;

int pdata[pthreads];

void threadinit()
   {
   int i;

   for (i=0; i<pthreads; i++)
      {
      pthread_mutex_init(&mutex1[i],NULL);
      pthread_mutex_init(&mutex2[i],NULL);

      pdata[i]=i;
      }

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
   }

void threadexit()
   {
   int i;

   for (i=0; i<pthreads; i++)
      {
      pthread_mutex_destroy(&mutex1[i]);
      pthread_mutex_destroy(&mutex2[i]);
      }

   pthread_attr_destroy(&attr);
   }

void startthread(void *(*thread)(void *brick),void *brick,void *data)
   {
   int pnum=*((int *)data);
   pthread_create(&pthread[pnum],&attr,thread,brick);
   }

void jointhread(void *data)
   {
   int pnum=*((int *)data);
   void *status;
   pthread_join(pthread[pnum],&status);
   }

void lock_cs1(void *data)
   {
   int pnum=*((int *)data);
   pthread_mutex_lock(&mutex1[pnum]);
   }

void unlock_cs1(void *data)
   {
   int pnum=*((int *)data);
   pthread_mutex_unlock(&mutex1[pnum]);
   }

void lock_cs2(void *data)
   {
   int pnum=*((int *)data);
   pthread_mutex_lock(&mutex2[pnum]);
   }

void unlock_cs2(void *data)
   {
   int pnum=*((int *)data);
   pthread_mutex_unlock(&mutex2[pnum]);
   }

#endif

static int winwidth,winheight,winid;

// USGS 1:250,000 and 1:24,000 DEM quads
static const char *usgsmaps[]={"data/HawaiiTileset/tiles/USGS-DEM-BBox.pgm",
                               // lores bathymetry
                               "Bathymetry-DEM/bathymetry.pgm",
                               // lores coverage
                               "USGS-DEM-250000/HAWAII-C.pgm",
                               "USGS-DEM-250000/HAWAII-E.pgm",
                               "USGS-DEM-250000/HAWAII-N.pgm",
                               "USGS-DEM-250000/HAWAII-S.pgm",
                               "USGS-DEM-250000/HAWAII-W.pgm",
                               "USGS-DEM-250000/KAUAI-NE.pgm",
                               "USGS-DEM-250000/KAUAI-NW.pgm",
                               "USGS-DEM-250000/KAUAI-SE.pgm",
                               "USGS-DEM-250000/KAUAI-SW.pgm",
                               "USGS-DEM-250000/MAUI-NE.pgm",
                               "USGS-DEM-250000/MAUI-NW.pgm",
                               "USGS-DEM-250000/MAUI-SE.pgm",
                               "USGS-DEM-250000/MAUI-SW.pgm",
                               "USGS-DEM-250000/OAHU.pgm",
                               // Big Island
                               "USGS-DEM-24000/AHUMOA.pgm",
                               "USGS-DEM-24000/AKAKA-FALLS.pgm",
                               "USGS-DEM-24000/ALIKA-CONE.pgm",
                               "USGS-DEM-24000/ANAEHOOMALU.pgm",
                               "USGS-DEM-24000/HAWI-OE-N.pgm",
                               "USGS-DEM-24000/HAWI.pgm",
                               "USGS-DEM-24000/HILO.pgm",
                               "USGS-DEM-24000/HONAUNAU.pgm",
                               "USGS-DEM-24000/HONOKAA.pgm",
                               "USGS-DEM-24000/HONOKANE.pgm",
                               "USGS-DEM-24000/HUALALAI.pgm",
                               "USGS-DEM-24000/KA-LAE-OE-E.pgm",
                               "USGS-DEM-24000/KA-LAE.pgm",
                               "USGS-DEM-24000/KAHUKU-RANCH.pgm",
                               "USGS-DEM-24000/KAILUA.pgm",
                               "USGS-DEM-24000/KALALUA.pgm",
                               "USGS-DEM-24000/KALAPANA-OE-E.pgm",
                               "USGS-DEM-24000/KALAPANA.pgm",
                               "USGS-DEM-24000/KAMUELA.pgm",
                               "USGS-DEM-24000/KAPOHO-OE-N.pgm",
                               "USGS-DEM-24000/KAPOHO.pgm",
                               "USGS-DEM-24000/KAU-DESERT.pgm",
                               "USGS-DEM-24000/KAULUOA-POINT.pgm",
                               "USGS-DEM-24000/KAUNENE.pgm",
                               "USGS-DEM-24000/KAWAIHAE.pgm",
                               "USGS-DEM-24000/KEAAU-RANCH.pgm",
                               "USGS-DEM-24000/KEAHOLE-POINT.pgm",
                               "USGS-DEM-24000/KEAIWA-RESERVOIR.pgm",
                               "USGS-DEM-24000/KEALAKEKUA.pgm",
                               "USGS-DEM-24000/KEAMUKU.pgm",
                               "USGS-DEM-24000/KEANAKOLU.pgm",
                               "USGS-DEM-24000/KEAWANUI-BAY.pgm",
                               "USGS-DEM-24000/KIHOLO.pgm",
                               "USGS-DEM-24000/KILAUEA-CRATER.pgm",
                               "USGS-DEM-24000/KIPUKA-PAKEKAKE.pgm",
                               "USGS-DEM-24000/KOKOOLAU.pgm",
                               "USGS-DEM-24000/KUKAIAU.pgm",
                               "USGS-DEM-24000/KUKUIHAELE-OE-N.pgm",
                               "USGS-DEM-24000/KUKUIHAELE.pgm",
                               "USGS-DEM-24000/KULANI.pgm",
                               "USGS-DEM-24000/MAHUKONA-OE-N.pgm",
                               "USGS-DEM-24000/MAHUKONA.pgm",
                               "USGS-DEM-24000/MAKAHALAU.pgm",
                               "USGS-DEM-24000/MAKALAWENA.pgm",
                               "USGS-DEM-24000/MAKAOPUHI-CRATER.pgm",
                               "USGS-DEM-24000/MANUKA-BAY.pgm",
                               "USGS-DEM-24000/MAUNA-KEA.pgm",
                               "USGS-DEM-24000/MAUNA-LOA.pgm",
                               "USGS-DEM-24000/MILOLII.pgm",
                               "USGS-DEM-24000/MOUNTAIN-VIEW.pgm",
                               "USGS-DEM-24000/NAALEHU.pgm",
                               "USGS-DEM-24000/NALIIKAKANI-POINT.pgm",
                               "USGS-DEM-24000/NAOHUELEELUA.pgm",
                               "USGS-DEM-24000/NOHONAOHAE.pgm",
                               "USGS-DEM-24000/PAHALA.pgm",
                               "USGS-DEM-24000/PAHOA-NORTH.pgm",
                               "USGS-DEM-24000/PAHOA-SOUTH.pgm",
                               "USGS-DEM-24000/PAPA.pgm",
                               "USGS-DEM-24000/PAPAALOA.pgm",
                               "USGS-DEM-24000/PAPAIKOU-OE-N.pgm",
                               "USGS-DEM-24000/PAPAIKOU.pgm",
                               "USGS-DEM-24000/PIIHONUA.pgm",
                               "USGS-DEM-24000/POHUE-BAY.pgm",
                               "USGS-DEM-24000/PUA-AKALA.pgm",
                               "USGS-DEM-24000/PUNALUU.pgm",
                               "USGS-DEM-24000/PUU-ANAHULU.pgm",
                               "USGS-DEM-24000/PUU-HINAI.pgm",
                               "USGS-DEM-24000/PUU-HOU.pgm",
                               "USGS-DEM-24000/PUU-KOLI.pgm",
                               "USGS-DEM-24000/PUU-LEHUA.pgm",
                               "USGS-DEM-24000/PUU-MAKAALA.pgm",
                               "USGS-DEM-24000/PUU-O-KEOKEO.pgm",
                               "USGS-DEM-24000/PUU-O-UO.pgm",
                               "USGS-DEM-24000/PUU-OO.pgm",
                               "USGS-DEM-24000/PUU-POHAKULOA.pgm",
                               "USGS-DEM-24000/PUU-ULAULA.pgm",
                               "USGS-DEM-24000/SULPHUR-CONE.pgm",
                               "USGS-DEM-24000/UMIKOA.pgm",
                               "USGS-DEM-24000/UPPER-PIIHONUA.pgm",
                               "USGS-DEM-24000/VOLCANO.pgm",
                               "USGS-DEM-24000/WOOD-VALLEY.pgm",
                               // Oahu
                               "USGS-DEM-24000/EWA.pgm",
                               "USGS-DEM-24000/HALEIWA.pgm",
                               "USGS-DEM-24000/HAUULA.pgm",
                               "USGS-DEM-24000/HONOLULU.pgm",
                               "USGS-DEM-24000/KAENA-OE-W.pgm",
                               "USGS-DEM-24000/KAENA.pgm",
                               "USGS-DEM-24000/KAHANA.pgm",
                               "USGS-DEM-24000/KAHUKU.pgm",
                               "USGS-DEM-24000/KANEOHE.pgm",
                               "USGS-DEM-24000/KOKO-HEAD.pgm",
                               "USGS-DEM-24000/MOKAPU.pgm",
                               "USGS-DEM-24000/PEARL-HARBOR.pgm",
                               "USGS-DEM-24000/SCHOFIELD-BARRACKS.pgm",
                               "USGS-DEM-24000/WAIANAE.pgm",
                               "USGS-DEM-24000/WAIMEA.pgm",
                               "USGS-DEM-24000/WAIPAHU.pgm"};

// LandSat ETM orthophotos
static const char *landmaps[]={"data/HawaiiTileset/landsat/LandSat-ETM-BBox.ppm",
                               "LandSat-ETM/hawaii.ppm",
                               "LandSat-ETM/kahoolawe.ppm",
                               "LandSat-ETM/kauai.ppm",
                               "LandSat-ETM/lanai.ppm",
                               "LandSat-ETM/maui.ppm",
                               "LandSat-ETM/molokai.ppm",
                               "LandSat-ETM/nihau.ppm",
                               "LandSat-ETM/oahu.ppm",
                               "data/HawaiiTileset/bathymetry/tile.1-1.ppm"};

// NOAA bathymetric DEM
static const char *bathymaps[]={"data/HawaiiTileset/bathymetry/Bathymetry-DEM-BBox.pgm",
                                "Bathymetry-DEM/bathymetry.pgm"};

// derived normal maps
static const char *normaps[]={"data/HawaiiTileset/normalized/Normalization-BBox.ppm",
                              // lores background
                              "data/HawaiiTileset/normalized/normalized.USGS-DEM-BBox.pgm",
                              // lores bathymetry
                              "data/HawaiiTileset/normalized/normalized.bathymetry.pgm",
                              // lores coverage
                              "data/HawaiiTileset/normalized/normalized.HAWAII-C.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWAII-E.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWAII-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWAII-S.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWAII-W.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAUAI-NE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAUAI-NW.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAUAI-SE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAUAI-SW.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUI-NE.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUI-NW.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUI-SE.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUI-SW.pgm",
                              "data/HawaiiTileset/normalized/normalized.OAHU.pgm",
                              // Big Island
                              "data/HawaiiTileset/normalized/normalized.AHUMOA.pgm",
                              "data/HawaiiTileset/normalized/normalized.AKAKA-FALLS.pgm",
                              "data/HawaiiTileset/normalized/normalized.ALIKA-CONE.pgm",
                              "data/HawaiiTileset/normalized/normalized.ANAEHOOMALU.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWI-OE-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAWI.pgm",
                              "data/HawaiiTileset/normalized/normalized.HILO.pgm",
                              "data/HawaiiTileset/normalized/normalized.HONAUNAU.pgm",
                              "data/HawaiiTileset/normalized/normalized.HONOKAA.pgm",
                              "data/HawaiiTileset/normalized/normalized.HONOKANE.pgm",
                              "data/HawaiiTileset/normalized/normalized.HUALALAI.pgm",
                              "data/HawaiiTileset/normalized/normalized.KA-LAE-OE-E.pgm",
                              "data/HawaiiTileset/normalized/normalized.KA-LAE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAHUKU-RANCH.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAILUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KALALUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KALAPANA-OE-E.pgm",
                              "data/HawaiiTileset/normalized/normalized.KALAPANA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAMUELA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAPOHO-OE-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAPOHO.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAU-DESERT.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAULUOA-POINT.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAUNENE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAWAIHAE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEAAU-RANCH.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEAHOLE-POINT.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEAIWA-RESERVOIR.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEALAKEKUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEAMUKU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEANAKOLU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KEAWANUI-BAY.pgm",
                              "data/HawaiiTileset/normalized/normalized.KIHOLO.pgm",
                              "data/HawaiiTileset/normalized/normalized.KILAUEA-CRATER.pgm",
                              "data/HawaiiTileset/normalized/normalized.KIPUKA-PAKEKAKE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KOKOOLAU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KUKAIAU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KUKUIHAELE-OE-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.KUKUIHAELE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KULANI.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAHUKONA-OE-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAHUKONA.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAKAHALAU.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAKALAWENA.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAKAOPUHI-CRATER.pgm",
                              "data/HawaiiTileset/normalized/normalized.MANUKA-BAY.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUNA-KEA.pgm",
                              "data/HawaiiTileset/normalized/normalized.MAUNA-LOA.pgm",
                              "data/HawaiiTileset/normalized/normalized.MILOLII.pgm",
                              "data/HawaiiTileset/normalized/normalized.MOUNTAIN-VIEW.pgm",
                              "data/HawaiiTileset/normalized/normalized.NAALEHU.pgm",
                              "data/HawaiiTileset/normalized/normalized.NALIIKAKANI-POINT.pgm",
                              "data/HawaiiTileset/normalized/normalized.NAOHUELEELUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.NOHONAOHAE.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAHALA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAHOA-NORTH.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAHOA-SOUTH.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAPA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAPAALOA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAPAIKOU-OE-N.pgm",
                              "data/HawaiiTileset/normalized/normalized.PAPAIKOU.pgm",
                              "data/HawaiiTileset/normalized/normalized.PIIHONUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.POHUE-BAY.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUA-AKALA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUNALUU.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-ANAHULU.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-HINAI.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-HOU.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-KOLI.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-LEHUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-MAKAALA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-O-KEOKEO.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-O-UO.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-OO.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-POHAKULOA.pgm",
                              "data/HawaiiTileset/normalized/normalized.PUU-ULAULA.pgm",
                              "data/HawaiiTileset/normalized/normalized.SULPHUR-CONE.pgm",
                              "data/HawaiiTileset/normalized/normalized.UMIKOA.pgm",
                              "data/HawaiiTileset/normalized/normalized.UPPER-PIIHONUA.pgm",
                              "data/HawaiiTileset/normalized/normalized.VOLCANO.pgm",
                              "data/HawaiiTileset/normalized/normalized.WOOD-VALLEY.pgm",
                              // Oahu
                              "data/HawaiiTileset/normalized/normalized.EWA.pgm",
                              "data/HawaiiTileset/normalized/normalized.HALEIWA.pgm",
                              "data/HawaiiTileset/normalized/normalized.HAUULA.pgm",
                              "data/HawaiiTileset/normalized/normalized.HONOLULU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAENA-OE-W.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAENA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAHANA.pgm",
                              "data/HawaiiTileset/normalized/normalized.KAHUKU.pgm",
                              "data/HawaiiTileset/normalized/normalized.KANEOHE.pgm",
                              "data/HawaiiTileset/normalized/normalized.KOKO-HEAD.pgm",
                              "data/HawaiiTileset/normalized/normalized.MOKAPU.pgm",
                              "data/HawaiiTileset/normalized/normalized.PEARL-HARBOR.pgm",
                              "data/HawaiiTileset/normalized/normalized.SCHOFIELD-BARRACKS.pgm",
                              "data/HawaiiTileset/normalized/normalized.WAIANAE.pgm",
                              "data/HawaiiTileset/normalized/normalized.WAIMEA.pgm",
                              "data/HawaiiTileset/normalized/normalized.WAIPAHU.pgm"};

// relative base path to the generated tiles and textures
static const char rbasepath1[]="tiles";
static const char rbasepath2[]="landsat";
static const char rbasepath3[]="bathymetry";
static const char rbasepath4[]="normalized";

// absolute base path to the generated tiles and textures
static const char abasepath1[]="data/HawaiiTileset/tiles";
static const char abasepath2[]="data/HawaiiTileset/landsat";
static const char abasepath3[]="data/HawaiiTileset/bathymetry";
static const char abasepath4[]="data/HawaiiTileset/normalized";

// number of tiles
static const int tilesL=16;
static const int tilesH=32;

// number of downsampling levels
static const int hdownL=3;
static const int tdownL=2;
static const int hdownH=1;
static const int tdownH=1;

// maximum size of tiles
static const int maxsize=2000;

// reduced scale
static const float scale=100.0f; // 1:100

// vertical exaggeration
static const float exaggeration=1.5f; // exaggeration=150%

// sea level and snow line
static const float sealevel=0.0f;
static const float snowline=3000.0f;

// light direction
static const float lx=-1.0f,ly=1.0f,lz=-0.5f;

// shading parameters
static const float ddirps=20.0f;
static float shadedir=0.0f;
static float shadeagl=45.0f;

// number of columns and rows
static int cols,rows;

// resampling output parameters
static float outparams[5];

// the terrain and its cache
static miniload terrain;
static minicache cache;

// the waypoints
static minipoint points;

// the sea level brick
static const int sea_m=5;
static const int sea_n=5;
static minibrick sea(sea_m,sea_n);

// the volume brick
static minibrick brick;

// the test volume bricks
static minibrick test1;
static minibrick test2;

// the sky dome
static minisky skydome;

// frames per second
static const float fps=25.0f;
static const int fpu=5;
static int upd=fpu;

// global resolution
static const float res=1.0E4f;

// sea level brick configuration
static const float sea_ext=10000.0f;
static const float srad=1000.0f;

// brick volume configuration
static const float brad=5000.0f;

// test brick configuration
static const float trad1=7000.0f;
static const float ttimef1=2*60;
static const float trad2=15000.0f;
static const float ttimef2=20*60;

// view frustum
static const float fovy=70.0f,nearp=50.0f,farp=1000000.0f;

// view point and direction
static float ex,ey,ez,ay,angle,turn,pitch,incline;

// stereo base
static const float sbase=0.5f;

// gliding parameters
static const float minspeed=10.0f,maxspeed=5000.0f,accel=0.1f,gravity=10.0f,height=1.81f,damping=200.0f;

// steering parameters
static const float oneturn=5.0f,minturn=0.1f,oneincline=10.0f,minincline=0.1f;

// gliding speed
static float speed,topspeed;

// wire frame flag
static int wireframe=0;

// performance statistics
static float peak=0.0f,avg=0.0f,load=0.0f;
static int frames=0,tsteps=0;

// initial uplift of the viewpoint
static const float uplift=1000.0f;

// initial viewpoint at Waipi'o Valley in geographic coordinates
static const float viewx=-560309.0f;
static const float viewy=72234.0f;
static const float viewa=25.0f;
static const float viewp=0.0f;

// Ben Discoe's house in UTM zone 5 NAD83 coordinates
static const float benx=237257.0f;
static const float beny=2219644.0f;
static const float bena=-65.0f;
static const float benp=0.0f;

// Green Flamingo Nursery in UTM zone 4 WGS84 coordinates
static const float gfnx=631433.0f;
static const float gfny=2361214.0f;
static const float gfna=-60.0f;
static const float gfnp=0.0f;

// Makai Ocean Engineering in UTM zone 4 WGS84 coordinates
static const float moex=638128.0f;
static const float moey=2358101.0f;
static const float moea=-75.0f;
static const float moep=0.0f;

// height and range of waypoint markers
static const float markerheight=200.0f;
static const float markerrange=0.05f;

// approximate value of one arc-second in meters
static float arcsec[3];

// command line switches
static int sw_quit=0;
static int sw_hires=0;
static int sw_stereo=0;
static int sw_anaglyph=0;
static int sw_contours=0;
static int sw_normals=0;
static int sw_dropwp=1;
static int sw_sea=0;
static int sw_brick=0;
static int sw_test1=0;
static int sw_test2=0;

// stopmotion switch
static int stopmotion=0;

// output switch
static int output=0;

inline void blendcolor(float *color,float value,float red,float green,float blue,float light)
   {
   if (value==0.0f) return;

   if (value==1.0f)
      {
      color[0]=red*light;
      color[1]=green*light;
      color[2]=blue*light;
      }
   else
      {
      color[0]=(1.0f-value)*color[0]+value*red*light;
      color[1]=(1.0f-value)*color[1]+value*green*light;
      color[2]=(1.0f-value)*color[2]+value*blue*light;
      }
   }

void shader(float nx,float ny,float nz,float elev,float *rgb)
   {
   static const float norm=sqrt(lx*lx+ly*ly+lz*lz);

   float slope,light;
   float sea,acre,grass,trees,rock,snow;

   slope=sqrt(nx*nx+nz*nz)/ny;

   light=(lx*nx+ly*ny+lz*nz)/norm;
   if (light<0.0f) light=0.0f;
   light=0.4f+0.6f*light;

   sea=acre=grass=trees=rock=snow=0.0f;

   if (elev==0.0f) sea=1.0f;
   if (elev>0.0f && elev<1.0f/3.0f && slope<1.0f/5.0f) acre=(1.0f-3.0f*elev)*(1.0f-5.0f*slope);
   if (elev>0.0f && elev<2.0f/3.0f && slope>1.0f/10.0f && slope<1.0f/2.0f) grass=(1.0f-3.0f/2.0f*elev)*(1.0f-2.0f*slope);
   if (elev>0.0f && elev<1.0f/2.0f && slope>1.0f/4.0f && slope<1.0f) trees=(1.0f-2.0f*elev)*slope;
   if (elev>0.0f && slope>1.0f/3.0f) rock=1.0f-1.0f/3.0f/slope;
   if (elev>2.0f/3.0f) snow=(elev-2.0f/3.0f)/(1.0f-2.0f/3.0f);

   blendcolor(rgb,1.0f,1.0f,1.0f,1.0f,light);
   blendcolor(rgb,sea,0.5f,0.75f,1.0f,light);
   blendcolor(rgb,acre,0.75f,0.75f,0.1f,light);
   blendcolor(rgb,grass,0.0f,1.0f,0.0f,light);
   blendcolor(rgb,trees,0.3f,0.5f,0.1f,light);
   blendcolor(rgb,rock,0.5f,0.5f,0.25f,light);
   blendcolor(rgb,snow,0.9f,0.9f,1.0f,light);
   }

void initview(float x,float y,float a,float p)
   {
   ex=(x-viewx)*arcsec[0];
   ez=(y-viewy)*arcsec[1];

   ey=terrain.getheight(ex,-ez)+exaggeration*height;
   if (frames==0) ey+=uplift/scale;

   ay=0.0f;

   angle=turn=a;
   pitch=incline=p;

   speed=topspeed=0.0f;

   upd=1;
   }

int request_callback(int col,int row,const unsigned char *mapfile,int hlod,const unsigned char *texfile,int tlod,const unsigned char *fogfile,void *data,databuf *hfield,databuf *texture,databuf *fogmap)
   {
   int present;

   if (col<0 || row<0 || hlod<0 || tlod<0 || data!=NULL) ERRORMSG();

   if (hfield!=NULL && texture!=NULL && fogmap!=NULL)
      {
      if (mapfile!=NULL) hfield->loadPNMdata((char *)mapfile);
      if (texfile!=NULL)
         if (sw_normals==0) texture->loadPPMcompressed((char *)texfile);
         else texture->loadPPMnormalized((char *)texfile,abasepath4);
      if (fogfile!=NULL) fogmap->loadPNMdata((char *)fogfile);
      }
   else
      {
      present=1;

      if (mapfile!=NULL) present&=checkfile((char *)mapfile);
      if (texfile!=NULL) present&=checkfile((char *)texfile);
      if (fogfile!=NULL) present&=checkfile((char *)fogfile);

      return(present);
      }

   return(1);
   }

int SEAavailable_callback(int col,int row,int lod,void *data)
   {
   if (col<0 || row<0 || data!=NULL) ERRORMSG();

   return(lod==0);
   }

void SEAload_callback(int col,int row,int lod,databuf *volume,void *data)
   {
   float px,py;

   if (col<0 || row<0 || lod<0 || data!=NULL) ERRORMSG();

   minicrs::UTM2LL(moex,moey,4,3,&py,&px);

   px+=(col-sea_m/2.0f)*sea_ext;
   py+=(row-sea_n/2.0f)*sea_ext;

   volume->generateplane(8,px,py,0.0f,0.0f,0.0f,1.0f,sea_ext,sea_ext,sea_ext);
   }

const char PVMfilename[]="data/Storm/Storm.pvm";

int PVMavailable_callback(int col,int row,int lod,void *data)
   {
   if (col<0 || row<0 || data!=NULL) ERRORMSG();

   return(lod<=1);
   }

void PVMload_callback(int col,int row,int lod,databuf *volume,void *data)
   {
   int i;

   float px,py;

   unsigned int xs,ys,zs;

   if (col<0 || row<0 || data!=NULL) ERRORMSG();

   minicrs::UTM2LL(moex,moey,4,3,&py,&px);
   volume->loadPVMdata(PVMfilename,1,21,0.0f,2.0f,px,py,0.0f,100.0f,100.0f,2000.0f);

   for (xs=3; xs<volume->xsize; xs=2*xs-1);
   for (ys=3; ys<volume->ysize; ys=2*ys-1);
   for (zs=3; zs<volume->zsize; zs=2*zs-1);

   volume->resampledata(xs,ys,zs);

   xs=volume->xsize;
   ys=volume->ysize;
   zs=volume->zsize;

   for (i=0; i<lod; i++)
      {
      xs=xs/2+1;
      ys=ys/2+1;
      zs=zs/2+1;
      }

   volume->resampledata(xs,ys,zs);
   }

const char MOE1filename[]="data/PRECIPf_01-02.MOEVar";
const char MOE2filename[]="data/UH_TempDeriv_3600.MoeVar";

int MOE1available_callback(int col,int row,int lod,void *data)
   {
   if (col<0 || row<0 || data!=NULL) ERRORMSG();

   return(lod==0 && checkfile(MOE1filename));
   }

int MOE2available_callback(int col,int row,int lod,void *data)
   {
   if (col<0 || row<0 || data!=NULL) ERRORMSG();

   return(lod==0 && checkfile(MOE2filename));
   }

void MOE1load_callback(int col,int row,int lod,databuf *volume,void *data)
   {
   float usefs,usefg;
   float minval,maxval;

   if (col<0 || row<0 || lod<0 || data!=NULL) ERRORMSG();

   volume->loadMOEdata(MOE1filename,&usefs,&usefg);

   volume->getminmax(usefs,usefg,&minval,&maxval);
   volume->replaceinvalid(usefs,usefg,(minval>=0.0f)?minval:maxval);

   volume->swx*=60*60;
   volume->swy*=60*60;
   volume->nwx*=60*60;
   volume->nwy*=60*60;
   volume->nex*=60*60;
   volume->ney*=60*60;
   volume->sex*=60*60;
   volume->sey*=60*60;
   }

void MOE2load_callback(int col,int row,int lod,databuf *volume,void *data)
   {
   float usefs,usefg;

   if (col<0 || row<0 || lod<0 || data!=NULL) ERRORMSG();

   volume->loadMOEdata(MOE2filename,&usefs,&usefg);

   volume->replaceinvalid(usefs,usefg,0.0f);
   volume->computeabsolute();

   volume->swx*=60*60;
   volume->swy*=60*60;
   volume->nwx*=60*60;
   volume->nwy*=60*60;
   volume->nex*=60*60;
   volume->ney*=60*60;
   volume->sex*=60*60;
   volume->sey*=60*60;
   }

void displayfunc()
   {
   int i;

   double time,delta,idle;

   float sina,cosa,tanp;
   float elev,coef;
   float rx,rz;

   int vtx;
   float perf;

   time=minigettime();

   sina=sin(2.0f*PI/360.0f*turn);
   cosa=cos(2.0f*PI/360.0f*turn);

   tanp=tan(2.0f*PI/360.0f*incline);

   ex+=sina*speed/fps;
   ez+=cosa*speed/fps;

   speed+=(fabs(topspeed-speed)>minspeed/scale?accel:1.0f)*(topspeed-speed);

   elev=terrain.getheight(ex,-ez);

   coef=(ey-elev-exaggeration*height)/(exaggeration*height);

   if (coef>1.0f) coef=1.0f;
   else if (coef<0.05f) coef=0.0f;

   ay-=coef*gravity/fps;
   ay*=pow(1.0f/damping,1.0f/fps);
   ey+=ay/fps;

   if (ey<elev+exaggeration*height)
      {
      ay=(elev+exaggeration*height-ey)*fps;
      ey=elev+exaggeration*height;
      }

   turn+=(fabs(angle-turn)>minturn?accel:1.0f)*(angle-turn);

   incline+=(fabs(pitch-incline)>minincline?accel:1.0f)*(pitch-incline);

   glClearColor(0.7f,0.85f,1.0f,1.0f);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(fovy,(float)winwidth/winheight,nearp/scale,farp/scale);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(ex,ey,-ez,ex+sina,ey-tanp,-ez-cosa,0.0f,1.0f,0.0f);

   if (sw_normals!=0)
      {
      cache.setpixshaderparams(fcos(shadedir/360.0f*2*PI)*fcos(shadeagl/360.0f*2*PI),
                               fsin(shadedir/360.0f*2*PI)*fcos(shadeagl/360.0f*2*PI),
                               fsin(shadeagl/360.0f*2*PI),
                               0.0f,2);

      shadedir+=ddirps/fps;
      }

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
                  farp/scale,1.0f);

   if (sw_stereo==0)
      {
      // draw sky dome
      skydome.drawskydome();

      // render vertex arrays
      vtx=cache.rendercache();

      // draw waypoints
      if (sw_dropwp==0)
         points.drawsignposts(ex,ey,ez,markerheight/scale,markerrange*farp/scale,turn,incline);

      // render volume bricks
      for (i=MINIBRICK_FIRST_RENDER_PHASE; i<=MINIBRICK_LAST_RENDER_PHASE; i++)
         {
         sea.render(ex,ey,ez,srad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         brick.render(ex,ey,ez,brad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         test1.render(ex,ey,ez,trad1/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef1,i);
         test2.render(ex,ey,ez,trad2/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef2,i);
         }
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

      if (sw_dropwp==0)
         points.drawsignposts(ex,ey,ez,markerheight/scale,markerrange*farp/scale,turn,incline);

      for (i=MINIBRICK_FIRST_RENDER_PHASE; i<=MINIBRICK_LAST_RENDER_PHASE; i++)
         {
         sea.render(ex-rx,ey,-ez+rz,srad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         brick.render(ex-rx,ey,-ez+rz,brad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         test1.render(ex-rx,ey,-ez+rz,trad1/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef1,i);
         test2.render(ex-rx,ey,-ez+rz,trad2/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef2,i);
         }

      // right channel:

      glClear(GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(ex+rx,ey,-ez-rz,ex+sina+rx,ey-tanp,-ez-cosa-rz,0.0f,1.0f,0.0f);

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK_RIGHT);
      else glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_FALSE);

      skydome.drawskydome();

      vtx+=cache.rendercache();

      if (sw_dropwp==0)
         points.drawsignposts(ex,ey,ez,markerheight/scale,markerrange*farp/scale,turn,incline);

      for (i=MINIBRICK_FIRST_RENDER_PHASE; i<=MINIBRICK_LAST_RENDER_PHASE; i++)
         {
         sea.render(ex+rx,ey,-ez-rz,srad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         brick.render(ex+rx,ey,-ez-rz,brad/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps,i);
         test1.render(ex+rx,ey,-ez-rz,trad1/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef1,i);
         test2.render(ex+rx,ey,-ez-rz,trad2/scale,farp/scale,fovy,(float)winwidth/winheight,tsteps/fps*ttimef2,i);
         }

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK);
      else glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
      }

   glutSwapBuffers();

   if (output)
      printf("position: lat=%g lon=%g direction=%g elevation=%gm mem=%gMB texmem=%gMB fancnt=%d vtxcnt=%d\n",
             (ez/arcsec[1]+viewy)/3600,(ex/arcsec[0]+viewx)/3600,angle,elev*scale/exaggeration,
             terrain.getmem(),terrain.gettexmem(),
             cache.getfancnt(),cache.getvtxcnt());

   delta=minigettime()-time;

   if (delta>0.0)
      {
      perf=vtx/delta;

      if (perf>peak) peak=perf;
      avg+=perf;

      load+=delta*fps;

      frames++;
      }

   idle=1.0f/fps-delta;
   if (idle>0.0) miniwaitfor(idle);

   if (stopmotion==0) tsteps++;
   }

void reshapefunc(int width,int height)
   {
   winwidth=width;
   winheight=height;

   if (winwidth<1) winwidth=1;
   if (winheight<1) winheight=1;

   glViewport(0,0,width,height);

   displayfunc();
   }

void keyboardfunc(unsigned char key,int x,int y)
   {
   float elev,coef;

   float mousex,mousey;

   mousex=(float)x/(winwidth-1);
   mousey=(float)y/(winwidth-1);

   if (key=='0') initview(viewx,viewy,viewa,viewp);
   else if (key=='w' || key=='W'|| key==' ')
      if (topspeed==0.0f) topspeed=maxspeed/scale;
      else topspeed=0.0f;
   else if (key=='a' || key=='A')
      {
      elev=terrain.getheight(ex,-ez);

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
      elev=terrain.getheight(ex,-ez);

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
   else if (key=='b')
      {
      minicrs::UTM2LL(benx,beny,5,4,&ez,&ex); // UTM zone 5 NAD83 datum
      initview(ex,ez,bena,benp);
      }
   else if (key=='g')
      {
      minicrs::UTM2LL(gfnx,gfny,4,3,&ez,&ex); // UTM zone 4 WGS84 datum
      initview(ex,ez,gfna,gfnp);
      }
   else if (key=='m')
      {
      minicrs::UTM2LL(moex,moey,4,3,&ez,&ex); // UTM zone 4 WGS84 datum
      initview(ex,ez,moea,moep);
      }
   else if (key=='p') sw_dropwp=1-sw_dropwp;
   else if (key=='c')
      {
      sw_contours=1-sw_contours;

      cache.usevtxshader(sw_contours);
      cache.usepixshader(sw_contours);
      }
   else if (key=='l')
      {
      wireframe=1-wireframe;
      if (wireframe!=0) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      }
   else if (key=='t') stopmotion=1-stopmotion;
   else if (key=='o') output=1-output;
   else if (key=='q' || key==27)
      {
      if (frames>0)
         printf("performance: peak=%gMVtx/s avg=%gMVtx/s load=%d%%\n",
                peak/1.0E6f,avg/frames/1.0E6f,(int)(100.0f*load/frames));

#ifdef PTHREADS
      brick.stopthread();
      test1.stopthread();
      threadexit();
#endif

      glutDestroyWindow(winid);
      exit(0);
      }
   }

unsigned char *genTONEmap(int twidth,int theight,
                          void (*shader)(float elev,float ny,float *hsv))
   {
   int i,j;

   float x,y;
   float hsv[3],rgb[3];

   unsigned char *tmap,*tptr;

   if (twidth<2 || theight<2) ERRORMSG();

   if ((tmap=(unsigned char *)malloc(3*twidth*theight))==NULL) ERRORMSG();

   for (tptr=tmap,j=0; j<theight; j++)
      for (i=0; i<twidth; i++)
         {
         x=(float)i/(twidth-1);
         y=(float)j/(theight-1);

         shader(x,y,hsv);
         hsv2rgb(hsv[0],hsv[1],hsv[2],rgb);

         *tptr++=ftrc(255.0f*rgb[0]+0.5f);
         *tptr++=ftrc(255.0f*rgb[1]+0.5f);
         *tptr++=ftrc(255.0f*rgb[2]+0.5f);
         }

   return(tmap);
   }

void toneshader(float elev,float ny,float *hsv)
   {
   if (elev==0.0f)
      {
      hsv[0]=240.0f;
      hsv[1]=0.5f;
      }
   else
      {
      hsv[0]=(1.0f-elev)*120.0f+elev*30.0f;
      hsv[1]=0.25f*fsqr(1.0f-elev)*ny+0.25f;
      }

   if (elev<0.05f) hsv[1]*=(1.0f-elev/0.05f);
   else if (elev<0.1f) hsv[1]*=(elev-0.05f)/0.05f;

   hsv[2]=1.0f;
   }

int main(int argc,char *argv[])
   {
   int i;

   float px,py;

   // process command line options
   for (i=1; i<argc; i++)
      if (strcmp(argv[i],"-q")==0) sw_quit=1;
      else if (strcmp(argv[i],"-h")==0) sw_hires=1;
      else if (strcmp(argv[i],"-s")==0) sw_stereo=1;
      else if (strcmp(argv[i],"-a")==0) sw_anaglyph=1;
      else if (strcmp(argv[i],"-c")==0) sw_contours=1;
      else if (strcmp(argv[i],"-n")==0) sw_normals=1;
      else if (strcmp(argv[i],"-d")==0) sw_dropwp=0;
      else if (strcmp(argv[i],"-o")==0) sw_sea=1;
      else if (strcmp(argv[i],"-b")==0) sw_brick=1;
      else if (strcmp(argv[i],"-1")==0) sw_test1=1;
      else if (strcmp(argv[i],"-2")==0) sw_test2=1;
      else if (strcmp(argv[i],"-t")==0) sw_test1=sw_test2=1;

   // turn off supersampling
   configure_supersampling(0);

   // turn on upsampling
   configure_upsampling(1);

   // enable startup file
   configure_startupfile(1);

   // define tile set path
   configure_tilesetpath("data/HawaiiTileset/");

   // resample tiles
   if (sw_hires==0)
      {
      if (sw_normals!=0)
         {
         // generate normal maps
         normalize(113,usgsmaps,
                   rbasepath4);

         // resample normal maps
         resample(114,normaps,
                  tilesL,hdownL,maxsize,rbasepath4);
         }

      // downscale bathymetric DEM
      resample(2,bathymaps,
               1,0,2048,rbasepath3);

      // shade bathymetric DEM
      texturemap("data/HawaiiTileset/bathymetry/tile.1-1.pgm","data/HawaiiTileset/bathymetry/tile.1-1.ppm",
                 2048,2048,shader,1000.0f,snowline);

      // resample orthophotos
      resample(10,landmaps,
               tilesL,tdownL,maxsize,rbasepath2);

      // resample USGS DEM
      resample(113,usgsmaps,
               tilesL,hdownL,maxsize,rbasepath1,
               NULL,NULL,NULL,NULL,NULL,-9999,
               &cols,&rows,outparams);
      }
   else
      {
      if (sw_normals!=0)
         {
         normalize(113,usgsmaps,
                   rbasepath4);

         resample(114,normaps,
                  tilesH,hdownH,maxsize,rbasepath4);
         }

      resample(2,bathymaps,
               1,0,2048,rbasepath3);

      texturemap("data/HawaiiTileset/bathymetry/tile.1-1.pgm","data/HawaiiTileset/bathymetry/tile.1-1.ppm",
                 2048,2048,shader,1000.0f,snowline);

      resample(10,landmaps,
               tilesH,tdownH,maxsize,rbasepath2);

      resample(113,usgsmaps,
               tilesH,hdownH,maxsize,rbasepath1,
               NULL,NULL,NULL,NULL,NULL,-9999,&cols,&rows);
      }

   // check for exit option
   if (sw_quit!=0) return(0);

   // open window with GLUT:

   winwidth=768;
   winheight=512;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   if (sw_stereo!=0 && sw_anaglyph==0) glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_STEREO);
   else glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   winid=glutCreateWindow("Hawaii Demo");

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

#ifdef PTHREADS
   // initialize threading
   threadinit();
#endif

   // turn off mip-mapping
   terrain.configure_mipmaps(0);

   // tell pager about request callback
   terrain.setloader(request_callback,NULL);

   // load resampled tiles
   terrain.load(cols,rows, // number of columns and rows
                abasepath1,abasepath2,NULL, // directories for tiles and textures (and no fogmaps)
                -viewx,-viewy, // horizontal offset in arc-seconds
                0.0f, // no vertical offset
                1.0f,1.0f,1.0f, // no horizontal stretching
                exaggeration,scale, // vertical exaggeration and global scale
                0.0f,0.0f, // no fog parameters required
                0.0f, // choose default minimum resolution
                0.0f, // disable base offset safety
                outparams, // geometric output parameters
                arcsec); // one arcsec in meters

   // initialize view point
   initview(viewx,viewy,viewa,viewp);

   // attach vertex array cache
   cache.attach(terrain.getminitile());

   // use default vertex shader plugin
   cache.setvtxshader();
   cache.usevtxshader(sw_contours);

   // fragment program for adding contour lines to the bathymetry
   static const char *frgprog1="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM c0=program.env[5]; \n\
      TEMP col,colt,vtx; \n\
      MOV col,fragment.color; \n\
      TEX colt,fragment.texcoord[0],texture[0],2D; \n\
      MAD colt,colt,a.x,a.y; \n\
      MUL col,col,colt; \n\
      MUL vtx.x,fragment.texcoord[0].z,c0.x; \n\
      FRC vtx.y,vtx.x; \n\
      MAD vtx.y,vtx.y,c0.z,-c0.w; \n\
      ABS vtx.y,vtx.y; \n\
      SUB vtx.y,c0.w,vtx.y; \n\
      MUL_SAT vtx.y,vtx.y,c0.y; \n\
      CMP vtx.y,vtx.x,vtx.y,c0.w; \n\
      MUL col.xyz,col,vtx.y; \n\
      MOV result.color,col; \n\
      END \n";

   // fragment program for shading normal maps
   static const char *frgprog2="!!ARBfp1.0 \n\
      PARAM a=program.env[0]; \n\
      PARAM t=program.env[1]; \n\
      PARAM c0=program.env[5]; \n\
      PARAM c1=program.env[6]; \n\
      PARAM c2=program.env[7]; \n\
      TEMP col,colt,nrm; \n\
      MOV col,fragment.color; \n\
      TEX nrm,fragment.texcoord[0],texture[0],2D; \n\
      MUL nrm,nrm,a.x; \n\
      MAD nrm.xy,nrm,c0.x,c0.y; \n\
      MOV colt.w,nrm.z; \n\
      MAD nrm.z,nrm.x,nrm.x,c0.y; \n\
      MAD nrm.z,nrm.y,nrm.y,nrm.z; \n\
      RSQ nrm.z,-nrm.z; \n\
      RCP_SAT nrm.z,nrm.z; \n\
      MAD_SAT colt.x,fragment.texcoord[0].z,c1.x,c1.y; \n\
      MAD colt.x,colt.x,t.x,t.y; \n\
      MAD colt.y,nrm.z,t.z,t.w; \n\
      TEX colt.xyz,colt,texture[1],2D; \n\
      MUL colt.xyz,colt,colt.w; \n\
      MUL col.xyz,col,colt; \n\
      DP3_SAT nrm,nrm,c2; \n\
      MAD nrm,nrm,c0.z,c0.w; \n\
      MUL col.xyz,col,nrm; \n\
      MOV result.color,col; \n\
      END \n";

   // use pixel shader plugin
   cache.setpixshader(frgprog1);
   cache.setpixshaderparams(scale/1000.0f,3.0f,2.0f,1.0f);
   cache.usepixshader(sw_contours);

   // override pixel shader plugin
   if (sw_normals!=0)
      {
      cache.setpixshader(frgprog2);
      cache.setpixshaderparams(2.0f,-1.0f,0.75f,0.25f,0);
      cache.setpixshaderparams(scale/2000.0f,0.0f,0.0f,0.0f,1);
      cache.usepixshader(1);

      int tsizex=64,tsizey=8;
      unsigned char *tmap=genTONEmap(tsizex,tsizey,toneshader);
      cache.setpixshadertexRGB(tmap,tsizex,tsizey);
      free(tmap);

      cache.setvtxshader();
      cache.usevtxshader(1);
      }

   // set sea level brick pager
   if (sw_sea!=0)
      sea.setloader(SEAavailable_callback,NULL,SEAload_callback,
                    -viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale,
                    1,1.5f,ftrc(1000*fps));

   // set iso spectrum
   sea.addiso(0.0f,0.0f,0.0f,1.0f,0.5f);

   // set volume brick pager
   if (sw_brick!=0)
      brick.setloader(PVMavailable_callback,NULL,PVMload_callback,
                      -viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale,
                      1,1.5f,ftrc(1000*fps));

   // set iso spectrum
   brick.addiso(0.1f,0.0f,0.0f,1.0f,0.9f);
   brick.addiso(0.3f,0.0f,1.0f,0.0f,0.9f);
   brick.addiso(0.5f,1.0f,0.0f,0.0f,1.0f);

   // set clipping plane
   minicrs::UTM2LL(moex,moey,4,3,&py,&px);
   brick.addclip(0,px,0.0f,py,-1.0f,0.0f,0.0f);

   // set render method
   brick.configure_renderpasses(4);

   // use multi-threading
#ifdef PTHREADS
   brick.setthread(startthread,&pdata[0],jointhread,lock_cs1,unlock_cs1,lock_cs2,unlock_cs2);
#endif

   // set test brick pager #1
   if (sw_test1!=0)
      test1.setloader(MOE1available_callback,NULL,MOE1load_callback,
                      -viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale);

   // set iso spectrum
   test1.addiso(0.0005f,0.25f,0.5f,1.0f,0.9f);
   test1.addiso(0.001f,0.25f,1.0f,0.5f,0.9f);
   test1.addiso(0.0015f,1.0f,0.5f,0.25f,1.0f);

   // use multi-threading
#ifdef PTHREADS
   test1.setthread(startthread,&pdata[1],jointhread,lock_cs1,unlock_cs1,lock_cs2,unlock_cs2);
#endif

   // set test brick pager #2
   if (sw_test2!=0)
      test2.setloader(MOE2available_callback,NULL,MOE2load_callback,
                      -viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale);

   // set iso spectrum
   test2.addiso(0.5f,1.0f,0.5f,0.25f,1.0f);
   test2.addiso(0.3f,1.0f,1.0f,1.0f,0.25f);
   test2.addiso(0.2f,1.0f,1.0f,1.0f,0.1f);

   // load way points
   points.load("data/HawaiiTileset/Waypoints.txt",-viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale,terrain.getminitile());
   points.load("data/Geocache.txt",-viewx,-viewy,0.0f,arcsec[0],arcsec[1],exaggeration/scale,terrain.getminitile());

   // create sky dome
   skydome.loadskydome("data/HawaiiTileset/SkyDome.ppm",
                       outparams[2],0.0f,-outparams[3],
                       cols*outparams[0],rows*outparams[1]/(cols*outparams[0]));

   // enter event loop
   glutMainLoop();

   // never reached
   return(0);
   }
