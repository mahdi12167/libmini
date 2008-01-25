// (c) by Stefan Roettger

#ifndef DATABASE_H
#define DATABASE_H

#include "minibase.h"

/* This class represents a single 1D, 2D, 3D or 4D data buffer with
   additional metadata such as dimension and type. */

class databuf
   {
   public:

   //! default constructor
   databuf();

   //! mandatory metadata
   unsigned int xsize;  // data size along the x-axis of 1D data
   unsigned int ysize;  // optional data size along the y-axis of 2D data
   unsigned int zsize;  // optional data size along the z-axis of 3D data
   unsigned int tsteps; // optional number of frames for time-dependent data
   unsigned int type;   // 0 = unsigned byte, 1 = signed short, 2 = float, 3 = RGB, 4 = RGBA, 5 = compressed RGB (S3TC DXT1), 6 = compressed RGBA (S3TC DXT1 with 1-bit alpha)

   //! optional metadata
   float swx,swy; // SW corner of data brick
   float nwx,nwy; // NW corner of data brick
   float nex,ney; // NE corner of data brick
   float sex,sey; // SE corner of data brick
   float h0,dh;   // base elevation and height of data brick
   float t0,dt;   // time frame start and exposure time

   //! optional scaling
   float scaling; // scale factor of data values, default=1.0f
   float bias;    // bias of data values, default=0.0f

   //! optional minimum and maximum value
   float minvalue;
   float maxvalue;

   //! indicator for external format
   unsigned int extformat; // 0=plain, 1=JPEG, 2=PNG

   //! indicator for implicit format
   unsigned int implformat;

   //! optional corner points in Lat/Lon (WGS84, degrees)
   float LLWGS84_swx,LLWGS84_swy; // SW corner of data brick
   float LLWGS84_nwx,LLWGS84_nwy; // NW corner of data brick
   float LLWGS84_nex,LLWGS84_ney; // NE corner of data brick
   float LLWGS84_sex,LLWGS84_sey; // SE corner of data brick

   //! data chunk
   void *data;         // pointer to raw data, null pointer indicates missing data
   unsigned int bytes; // number of raw data bytes

   //! check for missing data
   int missing() {return(data==NULL || extformat!=0 || implformat!=0);}

   //! allocate a new memory chunk
   void alloc(unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! set data to memory chunk
   void set(void *chunk,unsigned int length,
            unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! copy data from memory chunk
   void copy(void *chunk,unsigned int length,
             unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! reset buffer
   void reset();

   //! release buffer
   void release();

   //! set native extents
   void set_extents(float left,float right,float bottom,float top);

   //! set LLWGS84 extents
   void set_LLWGS84extents(float left,float right,float bottom,float top);

   //! set native extents
   void set_corners(float sw_corner_x,float sw_corner_y,
                    float se_corner_x,float se_corner_y,
                    float nw_corner_x,float nw_corner_y,
                    float ne_corner_x,float ne_corner_y);

   //! set native extents
   void set_LLWGS84corners(float sw_corner_x,float sw_corner_y,
                           float se_corner_x,float se_corner_y,
                           float nw_corner_x,float nw_corner_y,
                           float ne_corner_x,float ne_corner_y);

   //! native input/output
   void savedata(const char *filename,unsigned int extfmt=0); // data is saved in MSB format
   int loaddata(const char *filename); // data is converted from MSB into native format

   //! set conversion hook for external formats
   static void setconversion(int (*conversion)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data),void *data);

   //! set hook for automatic s3tc compression
   static void setautocompress(void (*autocompress)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data),void *data);

   //! automatic s3tc compression
   void autocompress();

   //! set interpreter hook for implicit format
   static void setinterpreter(void (*parser)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data),void *data,
                              void (*interpreter)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data));

   //! read one line in either UNIX or WINDOWS format
   char *readoneline(FILE *file);

   //! data is converted from PNM into native format
   int loadPNMdata(const char *filename);

   //! data is converted from PPM into compressed native format
   int loadPPMcompressed(const char *filename);

   //! data is converted from normalized PPM into compressed native format
   int loadPPMnormalized(const char *filename,const char *normalizedpath);

   //! data is converted from PVM into native format
   int loadPVMdata(const char *filename,
                   float midx,float midy,float basez,
                   float dx,float dy,float dz);

   //! data is converted from multiple time-dependent PVM files into native format
   int loadPVMdata(const char *filename, // the actual time step n is appended to the file name
                   unsigned int t, unsigned int n,
                   float timestart,float timestep,
                   float midx,float midy,float basez,
                   float dx,float dy,float dz);

   //! data is converted from MOE into native format
   int loadMOEdata(const char *filename,float *useful_smallest=0,float *useful_greatest=0);

   //! data is saved as plain PNM image
   void savePNMimage(const char *filename);

   //! data is generated from plane equation
   void generateplane(int size, // grid size
                      float px,float py,float pz, // point on plane
                      float nx,float ny,float nz, // plane normal
                      float dx,float dy,float dz); // dimension of box

   //! convert data from one type to another
   void convertdata(unsigned int newtype);

   //! resample data at a higher resolution
   void resampledata(unsigned int xs,unsigned int ys,unsigned int zs);

   //! get the minimum and maximum scalar value
   void getminmax(float *minval=0,float *maxval=0);

   //! get the minimum and maximum scalar value within a useful range
   void getminmax(float usefs,float usefg, // useful range
                  float *minval=0,float *maxval=0); // result range

   //! replace invalid values
   void replaceinvalid(float usefs,float usefg,float useful);

   //! compute absolute values
   void computeabsolute();

   //! set a single scalar value
   void setval(const unsigned int i,const unsigned int j,const unsigned int k,const float value);
   void setval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float value);

   //! get a single scalar value
   float getval(const unsigned int i,const unsigned int j,const unsigned int k);
   float getval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t);

   //! set rgb[a] color
   void setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const float *value);
   void setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value);
   void setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const float *value);
   void setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value);

   protected:

   static unsigned int MAGIC1;
   static unsigned int MAGIC2;
   static unsigned int MAGIC3;
   static unsigned int MAGIC4;

   private:

   static unsigned short int INTEL_CHECK;

   static int (*CONVERSION_HOOK)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data);
   static void *CONVERSION_DATA;

   static void (*AUTOCOMPRESS_HOOK)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,databuf *obj,void *data);
   static void *AUTOCOMPRESS_DATA;

   static void (*INTERPRETER_INIT)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data);
   static void (*INTERPRETER_HOOK)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data);
   static void *INTERPRETER_DATA;

   void loadblock(FILE *file);

   void writeparam(char *tag,float v,FILE *file,int digits=8);
   int readparam(char *tag,float *v,FILE *file);

   int readparami(char *tag,int *v,FILE *file);
   int readparamu(char *tag,unsigned int *v,FILE *file);

   void convertchunk(int israw,unsigned int extfmt);
   void interpretechunk(unsigned int implfmt);

   float getvalue(float x,float y,float z,unsigned int t);

   void swapbytes();
   };

#endif
