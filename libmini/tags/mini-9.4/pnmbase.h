// (c) by Stefan Roettger

#ifndef PNMBASE_H
#define PNMBASE_H

namespace pnmbase {

//! helper class to construct a PNM comment
class PNMcomment
   {
   public:

   static const int MAX_STR=1024;
   static const int MAX_COM=4096;

   PNMcomment();
   ~PNMcomment();

   void reset();

   char addchar(char ch);
   void addstring(const char *str);
   void addfloat(double v,int leading=0);
   void addunits(int units);

   char *str() {return(com);}

   private:

   char *com;
   int c,maxc;
   };

//! write a PNM image with optional comment to a file
int writePNMimage(const char *pnmfilename,
                  unsigned char *image,
                  int width,int height,int components,
                  PNMcomment *comment=0);

//! read a PNM image with optional comment from a file or memory
unsigned char *readPNMimage(const unsigned char *pnmimage,
                            int *width,int *height,int *components,
                            int loaded=0,unsigned char **ptr=0,
                            PNMcomment *comment=0);

//! read a PNM image with optional comment from a file
unsigned char *readPNMfile(const char *pnmfilename,
                           int *width,int *height,int *components,
                           PNMcomment *comment=0);

//! write a PVM volume
int writePVMvolume(const char *filename,unsigned char *volume,
                   int width,int height,int depth,int components);

//! read a PVM volume
unsigned char *readPVMvolume(const char *filename,
                             int *width,int *height,int *depth,int *components);

//! put a geographic grid descriptor into a PNM comment
void putPNMparams(PNMcomment *comment, // output PNM comment
                  const char *pnm_description=0, // data description
                  int coord_sys=0, // reference coordinate system: 0=LL 1=UTM
                  int coord_zone=0, // UTM zone: +- 1-60 0=LL (negative zones on southern hemisphere)
                  int coord_datum=0, // UTM datum: 1=NAD27 2=WGS72 3=WGS84 4=GRS80 5=Sphere 0=LL
                  int coord_units=4, // coordinate units: 0=radians 1=feet 2=meters 3=decimeters 4=arc-seconds
                  double coord_SW_x=-1800.0,double coord_SW_y=-1800.0, // SW corner in coordinate units
                  double coord_NW_x=-1800.0,double coord_NW_y=1800.0, // NW corner in coordinate units
                  double coord_NE_x=1800.0,double coord_NE_y=1800.0, // NE corner in coordinate units
                  double coord_SE_x=1800.0,double coord_SE_y=-1800.0, // SE corner in coordinate units
                  float cell_size_x=3.0f,float cell_size_y=3.0f, // cell size in coordinate units
                  int scaling_units=2, // elevation units: 1=feet 2=meters 3=decimeters
                  float vertical_scaling=1.0f, // elevation scaling factor
                  int missing_value=-9999); // missing data value

//! calculate the grid parameters in geographic coordinates
int getPNMparams(PNMcomment *comment, // input PNM comment
                 double *coord, // output corners in arc-seconds
                 float *cell_size, // output cell size in arc-seconds (mean approximate value)
                 float *vertical_scaling, // output elevation scaling factor (to yield meters)
                 int *missing_value, // output missing data value
                 int *utm_zone=0, // UTM zone: +- 1-60 0=LL
                 int *utm_datum=0); // UTM datum

}

using namespace pnmbase;

#endif
