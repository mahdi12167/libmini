// (c) by Stefan Roettger

#ifndef VIEWERBASE_H
#define VIEWERBASE_H

#include "minibase.h"

#include "convbase.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "miniearth.h"

#include "threadbase.h"
#include "curlbase.h"

//! base class for viewing tilesets
class viewerbase
   {
   public:

   struct VIEWER_PARAMS_STRUCT
      {
      // configurable parameters:

      float fps;    // frames per second (target frame rate)

      float fovy;   //  field of view (degrees)
      float nearp;  //  near plane (meters)
      float farp;   //  far plane (meters)

      // feature switches:

      BOOLINT usewireframe;

      // image conversion parameters:

      MINI_CONVERSION_PARAMS conversion_params;
      };

   typedef VIEWER_PARAMS_STRUCT VIEWER_PARAMS;

   //! default constructor
   viewerbase();

   //! destructor
   ~viewerbase();

   //! get parameters
   void get(VIEWER_PARAMS &params);

   //! set parameters
   void set(VIEWER_PARAMS &params);

   //! get parameters
   VIEWER_PARAMS *get() {return(&PARAMS);}

   //! set parameters
   void set(VIEWER_PARAMS *params) {set(*params);}

   // propagate parameters
   void propagate();

   //! get the encapsulated earth object
   miniearth *getearth() {return(EARTH);}

   //! get initial view point
   minicoord getinitial();

   //! set initial eye point
   void initeyepoint(const minicoord &e);

   //! generate and cache scene for a particular eye point
   void cache(const minicoord &e,const miniv3d &d,const miniv3d &u,float aspect);

   //! render scene
   void render();

   //! get time
   double gettime();

   //! start timer
   void starttimer();

   //! measure timer
   double gettimer();

   //! idle for the remainder of the frame
   //! dt is the time spent for rendering the last frame
   void idle(double dt);

   //! shoot a ray at the scene
   double shoot(const minicoord &o,const miniv3d &d);

   protected:

   VIEWER_PARAMS PARAMS;

   private:

   miniearth *EARTH;

   double START,TIMER;

   threadbase *THREADBASE;
   curlbase *CURLBASE;

   void inithooks();

   static void receive_callback(char *src_url,char *src_id,char *src_file,char *dst_file,int background,void *data);
   static int check_callback(char *src_url,char *src_id,char *src_file,void *data);

   static void autocompress(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);
   };

#endif
