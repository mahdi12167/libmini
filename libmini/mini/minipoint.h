// (c) by Stefan Roettger

#ifndef MINIPOINT_H
#define MINIPOINT_H

#include "minitile.h"
#include "minilod.h"

class minipointopts
   {
   public:

   enum
      {
      OPTION_DELIMITER=1,
      OPTION_EQ=2,
      OPTION_TYPE=3,
      OPTION_SIGNPOSTHEIGHT=4,
      OPTION_SIGNPOSTTURN=5,
      OPTION_SIGNPOSTINCLINE=6,
      OPTION_BRICKFILE=7,
      OPTION_BRICKSIZE=8,
      OPTION_BRICKTURN=9,
      OPTION_BRICKINCLINE=10,
      OPTION_BRICKPASSES=11
      };

   // default constructor
   minipointopts();

   // destructor
   ~minipointopts();

   int type; // type of the waypoint: none=-1 any=0 signpost=1 brick=2

   // signpost parameters
   float signpostheight;
   float signpostturn;
   float signpostincline;

   // brick parameters
   char *brickfile;
   float bricksize;
   float brickturn;
   float brickincline;
   int brickpasses;
   int brickindex;
   };

typedef struct
   {
   float x,y;
   float elev; // waypoint elevation
   float height; // terrain height

   char *desc;
   char *meta;
   char *comment;
   char *system;
   char *latitude;
   char *longitude;
   char *elevation;
   int zone,datum;

   minipointopts *opts;

   int number;
   }
minipointdata;

class minipoint
   {
   public:

   //! default constructor
   minipoint(minitile *tile=0);

   //! destructor
   ~minipoint();

   //! add waypoint
   void add(minipointdata *point);

   //! load waypoints
   void load(char *filename,
             float offsetlat=0.0f,float offsetlon=0.0f,
             float scalex=1.0f,float scaley=1.0f,float scaleelev=1.0f,
             minitile *tile=0,
             char delimiter=';');

   //! get number of waypoints
   int getnum() {return(PNUM);}

   //! get first waypoint
   minipointdata *getfirst() {return(getpoint(0));}

   //! get waypoint
   minipointdata *getpoint(int p);

   //! calculate visible waypoints
   void calcvdata(int type1=0,int type2=0);

   //! sort visible waypoints
   void sortvdata(float x,float y,float elev,
                  float dx,float dy,float de);

   //! get visible waypoints
   minipointdata **getvdata() {return(VPOINTS);}

   //! get number of visible waypoints
   int getvnum() {return(VNUM);}

   //! get nearest waypoint
   minipointdata *getnearest(float x,float y,float elev,int type1=0,int type2=1);

   //! get squared distance to waypoint
   float getdistance2(float x,float y,float elev,minipointdata *point);

   //! render waypoints with signposts
   void drawsignposts(float ex,float ey,float ez,
                      float height,float range,
                      float turn,float yon,
                      int type1=0,int type2=1);

   //! render waypoints with bricks
   void drawbricks(float ex,float ey,float ez,
                   float brad,float farp,
                   float fovy,float aspect,
                   float size,
                   int type1=0,int type2=2);

   //! set brick file name
   void setbrick(char *filename);

   //! getters:
   float getoffsetlat() {return(OFFSETLAT);}
   float getoffsetlon() {return(OFFSETLON);}
   float getscalex() {return(SCALEX);}
   float getscaley() {return(SCALEY);}
   float getscaleelev() {return(SCALEELEV);}
   int getzone() {return(CONFIGURE_DSTZONE);}
   int getdatum() {return(CONFIGURE_DSTDATUM);}

   //! configuring
   void configure_srcdatum(int datum=3); // source UTM datum
   void configure_dstzone(int zone=0); // dest UTM zone (LL=0)
   void configure_dstdatum(int datum=3); // dest UTM datum
   void configure_automap(int automap=0); // take mapping from first waypoint

   //! configuring of brick rendering
   void configure_brickceiling(float brickceiling=0.0f); // elevation modulates brick color
   void configure_brickpasses(int brickpasses=1); // render passes for brick display
   void configure_brickalpha(float brickalpha=0.5f); // opacity of brick
   void configure_bricklods(int bricklods=16); // number of brick LODs
   void configure_brickstagger(float brickstagger=1.25f); // staggering of brick LODs
   void configure_brickstripes(float brickstripes=0.0f); // offset of brick stripes

   protected:

   minitile *TILE;
   int COLS,ROWS;

   minipointdata **POINTS;
   int *NUM,*MAXNUM;
   int PNUM;

   minipointdata **VPOINTS;
   int VNUM,MAXVNUM;

   char *addch(char *str,char ch);

   void parsecomment(minipointdata *point);
   void parseoption(minipointdata *point,lunascan *scanner);

   private:

   int TAKEN,TRANS;

   float OFFSETLAT,OFFSETLON;
   float SCALEX,SCALEY,SCALEELEV;

   minilod *LODS;

   char *BRICKNAME;

   int CONFIGURE_SRCDATUM;
   int CONFIGURE_DSTZONE;
   int CONFIGURE_DSTDATUM;
   int CONFIGURE_AUTOMAP;

   float CONFIGURE_BRICKCEILING;
   int CONFIGURE_BRICKPASSES;
   float CONFIGURE_BRICKALPHA;
   int CONFIGURE_BRICKLODS;
   float CONFIGURE_BRICKSTAGGER;
   float CONFIGURE_BRICKSTRIPES;

   void shellsort(minipointdata *a[],const int n,
                  float x,float y,float elev,
                  float dx,float dy,float de);

   int compare(minipointdata *a,minipointdata *b,
               float x,float y,float elev,
               float dx,float dy,float de);

   void drawsequence(float ex,float ey,float ez,
                     float brad,float farp,
                     float fovy,float aspect,
                     float size,int mpasses,int passes);
   };

#endif
