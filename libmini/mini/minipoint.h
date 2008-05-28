// (c) by Stefan Roettger

#ifndef MINIPOINT_H
#define MINIPOINT_H

#include "minibase.h"

#include "datacache.h"

#include "minitile.h"
#include "minilod.h"

//! waypoint options
class minipointopts
   {
   public:

   enum
      {
      OPTION_DELIMITER=1,
      OPTION_EQ=2,
      OPTION_TYPE=3,
      OPTION_SIGNPOSTSIZE=4,
      OPTION_SIGNPOSTHEIGHT=5,
      OPTION_SIGNPOSTNOAUTO=6,
      OPTION_SIGNPOSTTURN=7,
      OPTION_SIGNPOSTINCLINE=8,
      OPTION_SIGNPOSTALPHA=9,
      OPTION_BRICKFILE=10,
      OPTION_BRICKSIZE=11,
      OPTION_BRICKTURN=12,
      OPTION_BRICKINCLINE=13,
      OPTION_BRICKCOLOR_RED=14,
      OPTION_BRICKCOLOR_GREEN=15,
      OPTION_BRICKCOLOR_BLUE=16,
      OPTION_BRICKPASSES=17
      };

   enum
      {
      OPTION_TYPE_NONE=-1,
      OPTION_TYPE_ANY=0;
      OPTION_TYPE_SIGNPOST=1;
      OPTION_TYPE_BRICK=2;
      };

   //! default constructor
   minipointopts();

   //! destructor
   ~minipointopts();

   int type; // type of the waypoint: none=-1 any=0 signpost=1 brick=2

   // signpost parameters
   float signpostsize; // 0=default size
   float signpostheight; // height of post
   float signpostnoauto; // automatic orientation
   float signpostturn; // clockwise turning angle
   float signpostincline; // downward inclination angle
   float signpostalpha; // opacity

   // brick parameters
   char *brickfile; // data file
   float bricksize; // 0=default size
   float brickturn; // clockwise turning angle
   float brickincline; // downward inclination angle
   float brickcolor_red; // red color component
   float brickcolor_green; // green color component
   float brickcolor_blue; // blue color component
   int brickpasses; // 0=default 1=striped 2-4=semi-transparent

   // optional brick parameters
   float brickrad; // brick radius
   float brickceiling; // elevation modulates brick color
   float brickalpha; // opacity of brick
   int bricklods; // number of brick LODs
   float brickstagger; // staggering of brick LODs
   float brickstripes; // offset of brick stripes
   };

//! virtual waypoint renderer
class minipointrndr
   {
   public:

   //! default constructor
   minipointrndr(BOOLINT sort,int passes)
      {
      SORT=sort;
      PASSES=passes;
      }

   //! destructor
   ~minipointrndr();

   BOOLINT required_sort() {return(SORT);}
   int required_passes() {return(PASSES);}

   virtual void init() {}

   virtual void render(minipointdata *point,int pass,
                       float ex,float ey,float ez,
                       float farp,float fovy,float aspect,double time,
                       minipointopts *global);

   virtual void exit() {}

   protected:

   BOOLINT SORT;
   int PASSES;
   };

// waypoint data
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
   minipointrndr *rndr;

   int number;
   }
minipointdata;

//! waypoint class
class minipoint
   {
   public:

   //! default constructor
   minipoint(minitile *tile=NULL);

   //! destructor
   ~minipoint();

   //! add waypoint
   void add(minipointdata *point);

   //! set file cache
   void setcache(datacache *cache,char *altpath);

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
   void calcvdata(int exclude=OPTION_TYPE_NONE);

   //! sort visible waypoints
   void sortvdata(float x,float y,float elev,
                  float dx,float dy,float de);

   //! get visible waypoints
   minipointdata **getvdata() {return(VPOINTS);}

   //! get number of visible waypoints
   int getvnum() {return(VNUM);}

   //! get nearest waypoint
   minipointdata *getnearest(float x,float y,float elev,
                             int exclude=OPTION_TYPE_NONE);

   //! get squared distance to waypoint
   float getdistance2(float x,float y,float elev,minipointdata *point);

   //! render waypoints
   void draw(float ex,float ey,float ez,
             float farp,float fovy,float aspect,
             double time,
             int fallback=OPTION_TYPE_NONE,
             int exclude=OPTION_TYPE_NONE);

   //! render waypoints with signposts
   void drawsignposts(float ex,float ey,float ez,
                      float height,float range,
                      float turn,float yon);

   //! set brick file name
   void setbrick(char *filename);

   //! render waypoints with bricks
   void drawbricks(float ex,float ey,float ez,
                   float brad,float farp,
                   float fovy,float aspect,
                   float size);

   //! getters
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

   //! configuring of signpost rendering
   void configure_signpostalpha(float signpostalpha=0.5f); // alpha value of signposts

   //! configuring of brick rendering
   void configure_brickpasses(int brickpasses=1); // render passes for brick display
   void configure_brickceiling(float brickceiling=0.0f); // elevation modulates brick color
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

   datacache *CACHE;
   char *ALTPATH;

   char *getfile(char *filename,char *altpath);

   char *addch(char *str,char ch);

   void parsecomment(minipointdata *point);
   void parseoption(minipointdata *point,lunascan *scanner);

   private:

   int TAKEN,TRANS;

   float OFFSETLAT,OFFSETLON;
   float SCALEX,SCALEY,SCALEELEV;

   char *BRICKNAME;

   minilod *LODS;

   int CONFIGURE_SRCDATUM;
   int CONFIGURE_DSTZONE;
   int CONFIGURE_DSTDATUM;
   int CONFIGURE_AUTOMAP;

   float CONFIGURE_SIGNPOSTALPHA;

   int CONFIGURE_BRICKPASSES;
   float CONFIGURE_BRICKCEILING;
   float CONFIGURE_BRICKALPHA;
   int CONFIGURE_BRICKLODS;
   float CONFIGURE_BRICKSTAGGER;
   float CONFIGURE_BRICKSTRIPES;

   void shellsort(minipointdata *a[],int n,
                  float x,float y,float elev,
                  float dx,float dy,float de);

   int compare(const minipointdata *a,const minipointdata *b,
               const float x,const float y,const float elev,
               const float dx,const float dy,const float de);

   void drawsequence(float ex,float ey,float ez,
                     float brad,float farp,
                     float fovy,float aspect,
                     float size,int mpasses,int passes);
   };

//! signpost renderer
class minipointrndr_signpost: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_signpost(): minipointrndr(TRUE,2) {}

   //! destructor
   ~minipointrndr_signpost() {}

   void render(minipointdata *point,int pass,
               float ex,float ey,float ez,
               float farp,float fovy,float aspect,double time,
               minipointopts *global);
   };

//! virtual brick renderer
class minipointrndr_brick: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_brick(int passes): minipointrndr(FALSE,passes)
      {
      //!!
      brickloaded=0;
      brickindex=-1;
      }

   //! destructor
   virtual ~minipointrndr_brick() {}

   void render(minipointdata *point,int pass,
               float ex,float ey,float ez,
               float farp,float fovy,float aspect,double time,
               minipointopts *global);

   private:

   // brick state variables
   int brickloaded;
   int brickindex;
   };

//! 1-pass brick renderer
class minipointrndr_brick1: public minipointrndr_brick
   {
   public:

   //! default constructor
   minipointrndr_brick1(): minipointrndr_brick(1) {}

   //! destructor
   ~minipointrndr_brick1() {}
   };

//! 2-pass brick renderer
class minipointrndr_brick2: public minipointrndr_brick
   {
   public:

   //! default constructor
   minipointrndr_brick2(): minipointrndr_brick(2) {}

   //! destructor
   ~minipointrndr_brick2() {}
   };

//! 3-pass brick renderer
class minipointrndr_brick3: public minipointrndr_brick
   {
   public:

   //! default constructor
   minipointrndr_brick3(): minipointrndr_brick(3) {}

   //! destructor
   ~minipointrndr_brick3() {}
   };

//! 4-pass brick renderer
class minipointrndr_brick4: public minipointrndr_brick
   {
   public:

   //! default constructor
   minipointrndr_brick4(): minipointrndr_brick(4) {}

   //! destructor
   ~minipointrndr_brick4() {}
   };

#endif
