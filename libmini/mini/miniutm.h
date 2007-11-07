// (c) by Stefan Roettger

#ifndef MINIUTM_H
#define MINIUTM_H

namespace miniutm {

extern double EARTH_radius; // radius of the earth

extern double WGS84_r_major; // WGS84 semi-major axis
extern double WGS84_r_minor; // WGS84 semi-minor axis
extern double WGS84_f;       // WGS84 flattening
extern double WGS84_e2;      // WGS84 eccentricity squared
extern double WGS84_ed2;     // WGS84 eccentricity derived

//! transform Lat/Lon to UTM
void LL2UTM(double lat,double lon, // geographic input coordinates in arc-seconds (WGS84 datum)
            int zone,int datum, // UTM zone and datum of output coordinates
            double *x,double *y); // output UTM coordinates (Easting and Northing)

void LL2UTM(double lat,double lon,
            int zone,int datum,
            float *x,float *y);

//! transform UTM to Lat/Lon
void UTM2LL(double x,double y, // input UTM coordinates (Easting and Northing)
            int zone,int datum, // UTM zone and datum of input coordinates
            double *lat,double *lon); // geographic output coordinates in arc-seconds (WGS84 datum)

void UTM2LL(double x,double y,
            int zone,int datum,
            float *lat,float *lon);

//! transform Lat/Lon/H to ECEF
void LLH2ECEF(double lat,double lon,double h, // geographic input coordinates in arc-seconds (WGS84 datum)
              double *xyz); // output ECEF coordinates

void LLH2ECEF(double lat,double lon,double h,
              float *xyz);

//! transform ECEF to Lat/Lon/H
void ECEF2LLH(double *xyz, // input ECEF coordinates
              double *lat,double *lon,double *h); // geographic output coordinates in arc-seconds (WGS84 datum)

void ECEF2LLH(float *xyz,
              float *lat,float *lon,float *h);

// 1 arc-second equals about 30 meters
void arcsec2meter(double lat,double *as2m);
void arcsec2meter(double lat,float *as2m);

// longitude arithmetic:

//! add longitudes
inline float LONADDLL(float a,float b=0)
   {
   float lon=a+b;
   while (lon<0) lon+=360*60*60;
   while (lon>360*60*60) lon-=360*60*60;
   return(lon);
   }

//! subtract longitudes
inline float LONSUBLL(float a,float b=0)
   {
   float diff=a-b;
   while (diff<-180*60*60) diff+=360*60*60;
   while (diff>180*60*60) diff-=360*60*60;
   return(diff);
   }

//! return leftmost longitude
inline float LONLEFTLL(float a,float b)
   {
   if (LONSUBLL(a,b)<0) return(a);
   else return(b);
   }

//! return rightmost longitude
inline float LONRIGHTLL(float a,float b)
   {
   if (LONSUBLL(a,b)>0) return(a);
   else return(b);
   }

//! linear interpolation of longitudes
inline float LONLERPLL(float a,float b,float lerp=0.5f)
   {
   float diff=LONADDLL(b,-a);
   return(LONSUBLL(a,-lerp*diff));
   }

//! average longitudes
inline float LONMEANLL(float a,float b,float weight=0.5f)
   {
   if (LONSUBLL(a,b)<0) return(LONLERPLL(a,b,weight));
   else return(LONLERPLL(b,a,1.0f-weight));
   }

}

using namespace miniutm;

#endif
