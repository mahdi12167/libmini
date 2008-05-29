// (c) by Stefan Roettger

#ifndef MINIPOINT_H
#define MINIPOINT_H

#include "minibase.h"

#include "datacache.h"

#include "minitile.h"
#include "minilod.h"

// forward declarations
class minipointopts;
class minipointrndr;

//! waypoint data
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
      OPTION_BRICKALPHA=17
      };

   enum
      {
      OPTION_TYPE_NONE=-1, // omitted type
      OPTION_TYPE_ANY=0, // unspecific type
      OPTION_TYPE_SIGNPOST=1, // signpost
      OPTION_TYPE_BRICK1=2, // 1-pass brick
      OPTION_TYPE_BRICK2=3, // 2-pass brick
      OPTION_TYPE_BRICK3=4, // 3-pass brick
      OPTION_TYPE_BRICK4=5, // 4-pass brick
      };

   //! default constructor
   minipointopts();

   //! destructor
   ~minipointopts();

   int type; // type of the waypoint

   // signpost parameters
   float signpostsize; // 0=default size
   float signpostheight; // height of post
   float signpostnoauto; // automatic orientation
   float signpostturn; // clockwise turning angle
   float signpostincline; // downward inclination angle
   float signpostalpha; // opacity

   // global signpost parameters
   float signpostrange; // display range

   // brick parameters
   char *brickfile; // data file
   float bricksize; // 0=default size
   float brickturn; // clockwise turning angle
   float brickincline; // downward inclination angle
   float brickcolor_red; // red color component
   float brickcolor_green; // green color component
   float brickcolor_blue; // blue color component
   float brickalpha; // opacity

   // global brick parameters
   float brickrad; // brick radius
   float brickceiling; // elevation modulates brick color
   int bricklods; // number of brick LODs
   float brickstagger; // staggering of brick LODs
   float brickstripes; // offset of brick stripes

   // brick variables
   BOOLINT brickloaded;
   int brickindex;
   };

//! virtual waypoint renderer
class minipointrndr
   {
   public:

   //! default constructor
   minipointrndr(int type,int passes) {TYPE=type; PASSES=passes;}

   //! destructor
   virtual ~minipointrndr() {}

   int gettype() {return(TYPE);}
   int getpasses() {return(PASSES);}

   virtual void init() {}

   virtual void render(minipointdata *point,int pass,
                       float ex,float ey,float ez,
                       float farp,float fovy,float aspect,double time,
                       minipointopts *global) {}

   virtual void exit() {}

   protected:

   int TYPE;
   int PASSES;
   };

//! signpost renderer
class minipointrndr_signpost: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_signpost(): minipointrndr(minipointopts::OPTION_TYPE_SIGNPOST,2) {}

   //! destructor
   ~minipointrndr_signpost() {}

   void render(minipointdata *point,int pass,
               float ex,float ey,float ez,
               float farp,float fovy,float aspect,double time,
               minipointopts *global);
   };

//! brick renderer
class minipointrndr_brick: public minipointrndr
   {
   public:

   //! default constructor
   minipointrndr_brick(int passes=4): minipointrndr(minipointopts::OPTION_TYPE_BRICK1+passes-1,passes) {}

   //! destructor
   ~minipointrndr_brick() {}

   void render(minipointdata *point,int pass,
               float ex,float ey,float ez,
               float farp,float fovy,float aspect,double time,
               minipointopts *global);
   };

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

   //! register renderer
   void registerrndr(minipointrndr *rndr);

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
   void calcvdata();

   //! sort visible waypoints
   void sortvdata(float x,float y,float elev,
                  float dx,float dy,float de);

   //! get visible waypoints
   minipointdata **getvdata() {return(VPOINTS);}

   //! get number of visible waypoints
   int getvnum() {return(VNUM);}

   //! get nearest waypoint
   minipointdata *getnearest(float x,float y,float elev,
                             int fallback=minipointopts::OPTION_TYPE_NONE,
                             int exclstart=minipointopts::OPTION_TYPE_NONE,
                             int exclend=minipointopts::OPTION_TYPE_NONE);

   //! get squared distance to waypoint
   float getdistance2(float x,float y,float elev,minipointdata *point);

   //! render waypoints
   void draw(float ex,float ey,float ez,
             float farp,float fovy,float aspect,
             double time,minipointopts *global,
             int fallback=minipointopts::OPTION_TYPE_NONE,
             int exclstart=minipointopts::OPTION_TYPE_NONE,
             int exclend=minipointopts::OPTION_TYPE_NONE);

   //! render waypoints with signposts
   void drawsignposts(float ex,float ey,float ez,
                      float height,float range,
                      float turn,float yon,
                      int fallback=minipointopts::OPTION_TYPE_SIGNPOST,
                      int exclstart=minipointopts::OPTION_TYPE_BRICK1,
                      int exclend=minipointopts::OPTION_TYPE_BRICK4);

   //! set brick file name
   void setbrick(char *filename);

   //! render waypoints with bricks
   void drawbricks(float ex,float ey,float ez,
                   float brad,float farp,
                   float fovy,float aspect,
                   float size,
                   int fallback=minipointopts::OPTION_TYPE_NONE,
                   int exclstart=minipointopts::OPTION_TYPE_SIGNPOST,
                   int exclend=minipointopts::OPTION_TYPE_SIGNPOST);

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
   void configure_brickalpha(float brickalpha=0.5f); // alpha value of bricks
   void configure_brickceiling(float brickceiling=0.0f); // elevation modulates brick color
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

   minipointrndr **RNDRS;
   int RNUM,MAXRNUM;

   datacache *CACHE;
   char *ALTPATH;

   char *addch(char *str,char ch);

   void parsecomment(minipointdata *point);
   void parseoption(minipointdata *point,lunascan *scanner);

   char *getfile(char *filename,char *altpath);

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

   float CONFIGURE_BRICKALPHA;
   float CONFIGURE_BRICKCEILING;
   int CONFIGURE_BRICKLODS;
   float CONFIGURE_BRICKSTAGGER;
   float CONFIGURE_BRICKSTRIPES;

   static minipointrndr_signpost RNDR_SIGNPOST;

   static minipointrndr_brick RNDR_BRICK1;
   static minipointrndr_brick RNDR_BRICK2;
   static minipointrndr_brick RNDR_BRICK3;
   static minipointrndr_brick RNDR_BRICK4;

   void shellsort(minipointdata *a[],int n,
                  float x,float y,float elev,
                  float dx,float dy,float de);

   int compare(const minipointdata *a,const minipointdata *b,
               const float x,const float y,const float elev,
               const float dx,const float dy,const float de);

   /*
   //!!
   void drawsequence(float ex,float ey,float ez,
                     float brad,float farp,
                     float fovy,float aspect,
                     float size,int mpasses,int passes);
   */
   };

#endif
