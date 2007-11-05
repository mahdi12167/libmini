// (c) by Stefan Roettger

#ifndef DATACACHE_H
#define DATACACHE_H

/* This class caches data that is arriving from a server and stores the
   downloaded data persistently on disk. The data transfer is delegated
   to an external library like libcurl. */

#include "minibase.h"

#include "database.h"
#include "miniload.h"
#include "datacloud.h"

// one element of file information
struct fileinfoelem
   {
   char *filename;
   unsigned int hash;

   BOOLINT isavailable,islocal;

   BOOLINT istile,istexture;
   float minvalue,maxvalue;
   int width,height;

   fileinfoelem *next;
   };

// a cache for terrain data tile sets and related information
class datacache
   {
   public:

   //! default constructor
   datacache(miniload *terrain);

   //! destructor
   ~datacache();

   //! cloud getter
   datacloud *getcloud() {return(CLOUD);}

   //! set callback for requesting tiles
   void setloader(void (*request)(char *file,int istexture,databuf *buf,void *data),void *data,
                  int paging=0,
                  float pfarp=0.0f,
                  float prange=0.0f,int pbasesize=0,
                  int plazyness=0,int pupdate=0,
                  int expire=0);

   //! set optional callback for requesting tiles from a server
   void setreceiver(void (*receive)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data),void *data,
                    int (*check)(char *src_url,char *src_id,char *src_file,void *data));

   //! set remote url
   void setremoteurl(const char *url);

   //! set remote world id
   void setremoteid(const char *id);

   //! set local path to file cache
   void setlocalpath(const char *path);

   //! set size of hash table
   void sethashsize(int size);

   //! get file
   char *getfile(const char *src_file,const char *altpath=NULL);

   //! set name of tileset file
   void settilesetfile(const char *filename);

   //! set name of vtb ini file for elevation tileset
   void setvtbelevinifile(const char *filename);

   //! set name of vtb ini file for imagery tileset
   void setvtbimaginifile(const char *filename);

   //! set path to vtb elevation tileset
   void setvtbelevpath(const char *elev);

   //! set path to vtb imagery tileset
   void setvtbimagpath(const char *imag);

   //! set name of startup file
   void setstartupfile(const char *filename);

   //! load persistent startup file
   void load();

   //! save persistent startup file
   void save();

   //! reset startup state
   void reset();

   //! getters for tileset info
   BOOLINT hasinfo() {return(HAS_INFO);}
   int getinfo_tilesx() {return(INFO_TILESX);}
   int getinfo_tilesy() {return(INFO_TILESY);}
   float getinfo_centerx() {return(INFO_CENTERX);}
   float getinfo_centery() {return(INFO_CENTERY);}
   float getinfo_sizex() {return(INFO_SIZEX);}
   float getinfo_sizey() {return(INFO_SIZEY);}
   int getinfo_maxsize() {return(INFO_MAXSIZE);}

   //! getters for vtb elev ini file
   BOOLINT haselevini() {return(HAS_ELEVINI);}
   int getelevini_tilesx() {return(ELEVINI_TILESX);}
   int getelevini_tilesy() {return(ELEVINI_TILESY);}
   float getelevini_centerx() {return(ELEVINI_CENTERX);}
   float getelevini_centery() {return(ELEVINI_CENTERY);}
   float getelevini_sizex() {return(ELEVINI_SIZEX);}
   float getelevini_sizey() {return(ELEVINI_SIZEY);}
   int getelevini_maxdemsize() {return(ELEVINI_MAXDEMSIZE);}
   float getelevini_minelev() {return(ELEVINI_MINELEV);}
   float getelevini_maxelev() {return(ELEVINI_MAXELEV);}

   //! getters for vtb imag ini file
   BOOLINT hasimagini() {return(HAS_IMAGINI);}
   int getimagini_tilesx() {return(IMAGINI_TILESX);}
   int getimagini_tilesy() {return(IMAGINI_TILESY);}
   float getimagini_centerx() {return(IMAGINI_CENTERX);}
   float getimagini_centery() {return(IMAGINI_CENTERY);}
   float getimagini_sizex() {return(IMAGINI_SIZEX);}
   float getimagini_sizey() {return(IMAGINI_SIZEY);}
   int getimagini_maxtexsize() {return(IMAGINI_MAXTEXSIZE);}

   //! query total number of tiles in the cache
   int gettotal() {return(CLOUD->gettotal());}

   //! query total number of pending tiles in the cache
   int getpending() {return(CLOUD->getpending());}

   //! query total amount of memory allocated in the cache (in MB)
   double getmem() {return(CLOUD->getmem());}

   //! configuring
   void configure_dontfree(int dontfree=0); // free memory chunks returned by callbacks
   void configure_locthreads(int locthreads=1); // number of background threads for local access
   void configure_netthreads(int netthreads=10); // number of background threads for network access

   protected:

   void loadtilesetinfo();
   void loadvtbelevini();
   void loadvtbimagini();

   void myrequest(unsigned char *mapfile,databuf *map,int istexture,int background);
   int mycheck(unsigned char *mapfile,int istexture);
   int myinquiry(int col,int row,unsigned char *mapfile,int hlod,float *minvalue,float *maxvalue);
   void myquery(int col,int row,unsigned char *texfile,int tlod,int *tsizex,int *tsizey);

   char *sourcefilename(const char *id,const char *filename);
   char *localfilename(const char *filename);
   BOOLINT localfilecheck(const char *filename);

   void insertfilename(const char *filename,BOOLINT isavailable,BOOLINT islocal,BOOLINT istile,BOOLINT istexture,float minvalue=1.0f,float maxvalue=0.0f,int width=0,int height=0,BOOLINT tailinsert=FALSE,BOOLINT dontupdate=FALSE);
   fileinfoelem *checkfilename(const char *filename);

   char *concat(const char *str1,const char *str2);
   unsigned int hashsum(const char *str);

   private:

   datacloud *CLOUD;

   void (*REQUEST_CALLBACK)(char *file,int istexture,databuf *buf,void *data);
   void *REQUEST_DATA;

   void (*RECEIVE_CALLBACK)(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   int (*CHECK_CALLBACK)(char *src_url,char *src_id,char *src_file,void *data);
   void *RECEIVE_DATA;

   char *RURL,*RID;
   char *LPATH;

   BOOLINT LOCAL;

   fileinfoelem **HASHTABLE;
   int HASHSIZE;

   char *TILESETFILE;
   char *VTBELEVINIFILE;
   char *VTBIMAGINIFILE;
   char *VTBELEVPATH;
   char *VTBIMAGPATH;
   char *STARTUPFILE;

   BOOLINT HAS_INFO;
   int INFO_TILESX;
   int INFO_TILESY;
   float INFO_CENTERX;
   float INFO_CENTERY;
   float INFO_SIZEX;
   float INFO_SIZEY;
   int INFO_MAXSIZE;

   BOOLINT HAS_ELEVINI;
   int ELEVINI_TILESX;
   int ELEVINI_TILESY;
   float ELEVINI_CENTERX;
   float ELEVINI_CENTERY;
   float ELEVINI_SIZEX;
   float ELEVINI_SIZEY;
   int ELEVINI_MAXDEMSIZE;
   float ELEVINI_MINELEV;
   float ELEVINI_MAXELEV;

   BOOLINT HAS_IMAGINI;
   int IMAGINI_TILESX;
   int IMAGINI_TILESY;
   float IMAGINI_CENTERX;
   float IMAGINI_CENTERY;
   float IMAGINI_SIZEX;
   float IMAGINI_SIZEY;
   int IMAGINI_MAXTEXSIZE;

   int CONFIGURE_DONTFREE;
   int CONFIGURE_LOCTHREADS;
   int CONFIGURE_NETTHREADS;

   static void mystaticrequest(unsigned char *mapfile,databuf *map,int istexture,int background,void *data);
   static int mystaticcheck(unsigned char *mapfile,int istexture,void *data);
   static int mystaticinquiry(int col,int row,unsigned char *mapfile,int hlod,void *data,float *minvalue,float *maxvalue);
   static void mystaticquery(int col,int row,unsigned char *texfile,int tlod,void *data,int *tsizex,int *tsizey);
   };

#endif
