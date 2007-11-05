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
void LL2UTM(float lat,float lon, // geographic input coordinates in arc-seconds (WGS84 datum)
            int zone,int datum, // UTM zone and datum of output coordinates
            float *x,float *y); // output UTM coordinates (Easting and Northing)

//! transform UTM to Lat/Lon
void UTM2LL(float x,float y, // input UTM coordinates (Easting and Northing)
            int zone,int datum, // UTM zone and datum of input coordinates
            float *lat,float *lon); // geographic output coordinates in arc-seconds (WGS84 datum)

//! transform Lat/Lon/H to ECEF
void LLH2ECEF(float lat,float lon,float h,
              float *xyz);

//! transform ECEF to Lat/Lon/H
void ECEF2LLH(float *xyz,
              float *lat,float *lon,float *h);

// 1 arc-second equals about 30 meters
void arcsec2meter(float lat,float *as2m);

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
