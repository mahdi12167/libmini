// (c) by Stefan Roettger

#include "minibase.h"

#include "miniv3d.h"
#include "minimath.h"

#include "minicrs.h"

const double minicrs::EARTH_radius=6370997.0; // mean earth radius

const double minicrs::WGS84_r_major=6378137.0; // WGS84 semi-major axis
const double minicrs::WGS84_r_minor=6356752.314245; // WGS84 semi-minor axis
const double minicrs::WGS84_f=1.0-WGS84_r_minor/WGS84_r_major; // WGS84 flattening
const double minicrs::WGS84_e2=2*WGS84_f-WGS84_f*WGS84_f; // WGS84 eccentricity squared
const double minicrs::WGS84_ed2=WGS84_r_major*WGS84_r_major/(WGS84_r_minor*WGS84_r_minor)-1.0; // WGS84 eccentricity derived
const double minicrs::WGS84_e=sqrt(WGS84_e2); // WGS84 eccentricity

int minicrs::crs_datum=0; // actual configured crs datum
int minicrs::crs_zone=0; // actual configured crs zone
double minicrs::crs_ratio=0.0; // actual configured axis ratio

double minicrs::r_major=EARTH_radius,minicrs::r_minor=EARTH_radius; // semi-major and minor radius of ellipsoid
double minicrs::o_dx=0.0,minicrs::o_dy=0.0,minicrs::o_dz=0.0; // origin shift of ellipsoid

double minicrs::scale_factor=0.9996; // scale factor

// ellipsoid selector based on semi-major and minor radius
void minicrs::choose_ellipsoid(double r_maj,double r_min)
   {
   r_major=r_maj;
   r_minor=r_min;
   }

// ellipsoid selector based on semi-major radius and flattening
void minicrs::choose_ellipsoid_flat(double r_maj,double f)
   {
   if (f>0.0) choose_ellipsoid(r_maj,(1.0-1.0/f)*r_maj);
   else choose_ellipsoid(r_maj,r_maj);
   }

// datum selector based on ellipsoid and shift
void minicrs::choose_datum(int ellipsoid,
                           double dx,double dy,double dz)
   {
   if (ellipsoid<1 || ellipsoid>25) ellipsoid=3;

   switch (ellipsoid)
      {
      // global ellipsoids:
      // 1=Clarke1866 (Clarke ellipsoid of 1866)
      case 1: choose_ellipsoid(6378206.4,6356583.8); break;
      // 2=WGS72 (World Geodetic System of 1972)
      case 2: choose_ellipsoid_flat(6378135.0,298.26); break;
      // 3=WGS84 (World Geodetic System of 1984)
      case 3: choose_ellipsoid(6378137.0,6356752.314245); break;
      // 4=GRS80 (Geodetic Reference System of 1980)
      case 4: choose_ellipsoid(6378137.0,6356752.31414); break;

      // uncommon global ellipsoids:
      // 5=Clarke1880 (Clarke ellipsoid of 1880)
      case 5: choose_ellipsoid_flat(6378249.145,293.465); break;
      // 6=WGS60 (World Geodetic System of 1960)
      case 6: choose_ellipsoid(6378165.0,6356783.287); break;
      // 7=WGS66 (World Geodetic System of 1966)
      case 7: choose_ellipsoid(6378145.0,6356759.769356); break;
      // 8=International1924 (Geodetic Reference System of 1924, aka Hayford ellipsoid of 1909)
      case 8: choose_ellipsoid_flat(6378388.0,297.0); break;
      // 9=GRS67 (Geodetic Reference System of 1967)
      case 9: choose_ellipsoid(6378157.5,6356772.2); break;
      // 10=GRS75 (Geodetic Reference System of 1975)
      case 10: choose_ellipsoid_flat(6378140.0,298.257); break;
      // 11=Sphere (with constant mean radius)
      case 11: choose_ellipsoid_flat(EARTH_radius,0.0); break;

      // local ellipsoids:
      // 12=Bessel1841 (German ellipsoid of 1841)
      case 12: choose_ellipsoid(6377397.155,6356078.96284); break;
      // 13=Bessel1841Mod (Modified German ellipsoid for Namibia)
      case 13: choose_ellipsoid_flat(6377483.865,299.1528128); break;
      // 14=Krassovsky1940 (East-European ellipsoid of 1940)
      case 14: choose_ellipsoid_flat(6378245.0,298.3); break;
      // 15=Airy1830 (British ellipsoid of 1830)
      case 15: choose_ellipsoid_flat(6377563.396,299.3249646); break;
      // 16=Airy1830Mod (Modified British ellipsoid for Ireland of 1830)
      case 16: choose_ellipsoid_flat(6377340.189,299.3249646); break;
      // 17=Australian (Australian National ellipsoid)
      case 17: choose_ellipsoid_flat(6378160.0,298.25); break;
      // 18=Everest1830 (Everest ellipsoid of 1830)
      case 18: choose_ellipsoid_flat(6377276.345,300.8017); break;
      // 19=SouthAmerican1969 (South American ellipsoid of 1969)
      case 19: choose_ellipsoid_flat(6378160.0,298.25); break;
      // 20=Indonesian1974 (Indonesian ellipsoid of 1974)
      case 20: choose_ellipsoid_flat(6378160.0,298.247); break;

      // uncommon local ellipsoids:
      // 21=Fischer1960 (Fischer ellipsoid of 1960)
      case 21: choose_ellipsoid_flat(6378166.0,298.3); break;
      // 22=Fischer1960Mod (Modified Fischer ellipsoid of 1960)
      case 22: choose_ellipsoid_flat(6378155.0,298.3); break;
      // 23=Fischer1968 (Fischer ellipsoid of 1968)
      case 23: choose_ellipsoid_flat(6378150.0,298.3); break;
      // 24=Helmert1906 (Helmert ellipsoid of 1906)
      case 24: choose_ellipsoid_flat(6378200.0,298.3); break;
      // 25=Hough1956 (Hough ellipsoid of 1956/1960)
      case 25: choose_ellipsoid_flat(6378270.0,297.0); break;

      default: ERRORMSG();
      }

   o_dx=dx;
   o_dy=dy;
   o_dz=dz;
   }

// datum selector
void minicrs::choose_datum(int datum)
   {
   if (datum<1 || datum>14) datum=3; // fallback to WGS84

   if (datum==crs_datum) return;

   switch (datum)
      {
      // global datums:
      // 1=NAD27 (Mean North American Datum of 1927)
      case 1: choose_datum(1,-8.0,160.0,176.0); break;
      // 2=WGS72 (World Geodetic System of 1972)
      case 2: choose_datum(2,0.0,0.0,0.0); break;
      // 3=WGS84 (World Geodetic System of 1984)
      case 3: choose_datum(3,0.0,0.0,0.0); break;
      // 4=NAD83 (Mean North American Datum of 1983, practically identical to WGS84)
      case 4: choose_datum(4,0.0,0.0,0.0); break;
      // 5=Sphere (with constant mean radius)
      case 5: choose_datum(11,0.0,0.0,0.0); break;

      // local datums:
      // 6=ED50 (Mean European Datum of 1950, centered at the Munich Frauenkirche)
      case 6: choose_datum(8,-87.0,-98.0,-121.0); break;
      // 7=ED87 (Mean European Datum of 1987)
      case 7: choose_datum(8,-86.0,-98.0,-119.0); break;
      // 8=OldHawaiian (mean datum for Hawaii/Maui/Molokai/Oahu/Kauai)
      case 8: choose_datum(1,61.0,-285.0,-181.0); break;
      // 9=Luzon (Philippine Datum)
      case 9: choose_datum(1,-133.0,-77.0,-51.0); break;
      // 10=Tokyo (Mean Japanese Datum)
      case 10: choose_datum(12,-148.0,507.0,685.0); break;
      // 11=OSGB1936 (mean datum of the Ordnance Survey Great Britain 1936)
      case 11: choose_datum(15,375.0,-111.0,431.0); break;
      // 12=Australian1984 (Mean Australian Geodetic Datum of 1984)
      case 12: choose_datum(17,-133.0,-48.0,148.0); break;
      // 13=NewZealand1949 (New Zealand Datum of 1949)
      case 13: choose_datum(8,84.0,-22.0,209.0); break;
      // 14=SouthAmerican1969 (Mean South American Datum of 1969)
      case 14: choose_datum(19,-57.0,1.0,-41.0); break;
      // add custom datums here...

      default: ERRORMSG();
      }

   crs_datum=datum;
   }

// transform Lat/Lon to UTM
// input in arc-seconds, output in meters
// latitude in [-90*60*60,90*60*60] arc-seconds
// longitude in [-180*60*60,180*60*60] arc-seconds
// 1 arc-second equals about 30 meters
void minicrs::LL2UTM(double lat,double lon,
                     int zone,int datum,
                     double *x,double *y)
   {
   molodensky(3,datum,&lat,&lon);

   choose_datum(datum);

   initUTM(zone,r_minor/r_major);
   calcLL2UTM(lat,lon,x,y);
   }

void minicrs::LL2UTM(double lat,double lon,
                     int zone,int datum,
                     float *x,float *y)
   {
   double tx,ty;

   LL2UTM(lat,lon,zone,datum,&tx,&ty);

   *x=(float)tx;
   *y=(float)ty;
   }

// transform UTM to Lat/Lon
// input in meters, output in arc-seconds
// latitude in [-90*60*60,90*60*60] arc-seconds
// longitude in [-180*60*60,180*60*60] arc-seconds
// 1 arc-second equals about 30 meters
void minicrs::UTM2LL(double x,double y,
                     int zone,int datum,
                     double *lat,double *lon)
   {
   choose_datum(datum);

   initUTM(zone,r_minor/r_major);
   calcUTM2LL(x,y,lat,lon);

   molodensky(datum,3,lat,lon);
   }

void minicrs::UTM2LL(double x,double y,
                     int zone,int datum,
                     float *lat,float *lon)
   {
   double tlat,tlon;

   UTM2LL(x,y,zone,datum,&tlat,&tlon);

   *lat=(float)tlat;
   *lon=(float)tlon;
   }

// transform LL to UTM zone
int minicrs::LL2UTMZ(double lat,double lon)
   {
   int zone;
   zone=dtrc(LONSUB(lon)/(6*60*60))+31;
   return((lat<0.0)?-zone:zone);
   }

// transform Lat/Lon to Mercator
// input in arc-seconds, output in meters
void minicrs::LL2MERC(double lat,double lon,
                      double *x,double *y)
   {
   choose_datum(3);
   calcLL2MERC(lat,lon,x,y,0.0,0.0);
   }

void minicrs::LL2MERC(double lat,double lon,
                      float *x,float *y)
   {
   double tx,ty;

   LL2MERC(lat,lon,&tx,&ty);

   *x=(float)tx;
   *y=(float)ty;
   }

// transform Mercator to Lat/Lon
// input in meters, output in arc-seconds
void minicrs::MERC2LL(double x,double y,
                      double *lat,double *lon)
   {
   choose_datum(3);
   calcMERC2LL(x,y,lat,lon,0.0,0.0);
   }

void minicrs::MERC2LL(double x,double y,
                      float *lat,float *lon)
   {
   double tlat,tlon;

   MERC2LL(x,y,&tlat,&tlon);

   *lat=(float)tlat;
   *lon=(float)tlon;
   }

// transform Lat/Lon/H to ECEF
// input in arc-seconds, height in meters, output in meters
// latitude in [-90*60*60,90*60*60] arc-seconds
// longitude in [-180*60*60,180*60*60] arc-seconds
// 1 arc-second equals about 30 meters
void minicrs::LLH2ECEF(double lat,double lon,double h,
                       double xyz[3])
   {
   double slat,clat,slon,clon; // sine and cosine values
   double r;                   // radius in prime vertical

   lat*=2*PI/(360*60*60);
   lon*=2*PI/(360*60*60);

   slat=sin(lat);
   clat=cos(lat);
   slon=sin(lon);
   clon=cos(lon);

   r=WGS84_r_major/sqrt(1.0-WGS84_e2*slat*slat);

   xyz[0]=(r+h)*clat*clon;
   xyz[1]=(r+h)*clat*slon;
   xyz[2]=(r*(1.0-WGS84_e2)+h)*slat;
   }

void minicrs::LLH2ECEF(double lat,double lon,double h,
                       float xyz[3])
   {
   double txyz[3];

   LLH2ECEF(lat,lon,h,txyz);

   xyz[0]=(float)txyz[0];
   xyz[1]=(float)txyz[1];
   xyz[2]=(float)txyz[2];
   }

// transform ECEF to Lat/Lon/H
// input in meters, height in meters, output in arc-seconds
// latitude in [-90*60*60,90*60*60] arc-seconds
// longitude in [-180*60*60,180*60*60] arc-seconds
// 1 arc-second equals about 30 meters
void minicrs::ECEF2LLH(double xyz[3],
                       double *lat,double *lon,double *h)
   {
   double sth,cth,slat,clat; // sine and cosine values
   double p,th;              // temporary variables
   double r;                 // radius in prime vertical

   p=sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
   if (p==0.0) p=1.0;

   th=atan(xyz[2]*WGS84_r_major/(p*WGS84_r_minor));

   sth=sin(th);
   cth=cos(th);

   // transformed latitude
   *lat=atan((xyz[2]+WGS84_ed2*WGS84_r_minor*sth*sth*sth)/(p-WGS84_e2*WGS84_r_major*cth*cth*cth));

   // transformed longitude
   *lon=atan2(xyz[1],xyz[0]);

   slat=sin(*lat);
   clat=cos(*lat);

   r=WGS84_r_major/sqrt(1.0-WGS84_e2*slat*slat);

   // transformed height
   if (clat>0.0) *h=p/clat-r;
   else *h=r;

   *lat*=360*60*60/(2*PI);
   *lon*=360*60*60/(2*PI);
   }

void minicrs::ECEF2LLH(float xyz[3],
                       float *lat,float *lon,float *h)
   {
   double txyz[3];
   double tlat,tlon,th;

   txyz[0]=xyz[0];
   txyz[1]=xyz[1];
   txyz[2]=xyz[2];

   ECEF2LLH(txyz,&tlat,&tlon,&th);

   *lat=tlat;
   *lon=tlon;
   *h=th;
   }

// transform OG/H to ECEF
// input and output in meters
void minicrs::OGH2ECEF(double x,double y,double h, // oblique gnomonic input coordinates in meters
                       int zone, // oblique gnomonic zone of input coordinates
                       double xyz[3]) // output ECEF coordinates
   {
   miniv3d pos,nrm;
   miniv3d right,up;
   double dist;

   if (zone<1) zone=1;
   else if (zone>6) zone=6;

   switch (zone)
      {
      case 1: pos=miniv3d(0.0,0.0,EARTH_radius); right=miniv3d(0.0,1.0,0.0); up=miniv3d(-1.0,0.0,0.0); break; // north pole
      case 2: pos=miniv3d(EARTH_radius,0.0,0.0); right=miniv3d(0.0,1.0,0.0); up=miniv3d(0.0,0.0,1.0); break; // greenwich
      case 3: pos=miniv3d(-EARTH_radius,0.0,0.0); right=miniv3d(0.0,-1.0,0.0); up=miniv3d(0.0,0.0,1.0); break; // honolulu
      case 4: pos=miniv3d(0.0,EARTH_radius,0.0); right=miniv3d(-1.0,0.0,0.0); up=miniv3d(0.0,0.0,1.0); break; // tokyo
      case 5: pos=miniv3d(0.0,-EARTH_radius,0.0); right=miniv3d(1.0,0.0,0.0); up=miniv3d(0.0,0.0,1.0); break; // new york
      case 6: pos=miniv3d(0.0,0.0,-EARTH_radius); right=miniv3d(0.0,1.0,0.0); up=miniv3d(1.0,0.0,0.0); break; // south pole
      default: ERRORMSG();
      }

   pos+=x*right+y*up;

   dist=intersect_ray_ellipsoid(miniv3d(0.0,0.0,0.0),pos,miniv3d(0.0,0.0,0.0),WGS84_r_major,WGS84_r_major,WGS84_r_minor);
   pos*=dist;

   nrm=pos;
   nrm.normalize();

   pos+=nrm*h;

   xyz[0]=pos.x;
   xyz[1]=pos.y;
   xyz[2]=pos.z;
   }

void minicrs::OGH2ECEF(double x,double y,double h,
                       int zone,
                       float xyz[3])
   {
   double txyz[3];

   OGH2ECEF(x,y,h,zone,txyz);

   xyz[0]=txyz[0];
   xyz[1]=txyz[1];
   xyz[2]=txyz[2];
   }

// transform ECEF to OG/H
// input and output in meters
void minicrs::ECEF2OGH(double xyz[3], // input ECEF coordinates
                       double *x,double *y,double *h, // oblique gnomonic output coordinates in meters
                       int zone) // oblique gnomonic zone of output coordinates
   {
   double prj[3];

   miniv3d pos;
   double dist,height;

   ECEF2PRJ(xyz,prj,&height);
   pos=miniv3d(prj);

   if (zone<1) zone=1;
   else if (zone>6) zone=6;

   switch (zone)
      {
      case 1: // north pole

         dist=intersect_line_plane(pos,pos,miniv3d(0.0,0.0,EARTH_radius),miniv3d(0.0,0.0,EARTH_radius));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=pos.y;
         *y=-pos.x;
         *h=height;

         break;

      case 2: // greenwich

         dist=intersect_line_plane(pos,pos,miniv3d(EARTH_radius,0.0,0.0),miniv3d(EARTH_radius,0.0,0.0));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=pos.y;
         *y=pos.z;
         *h=height;

         break;

      case 3: // honolulu

         dist=intersect_line_plane(pos,pos,miniv3d(-EARTH_radius,0.0,0.0),miniv3d(-EARTH_radius,0.0,0.0));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=-pos.y;
         *y=pos.z;
         *h=height;

         break;

      case 4: // tokyo

         dist=intersect_line_plane(pos,pos,miniv3d(0.0,EARTH_radius,0.0),miniv3d(0.0,EARTH_radius,0.0));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=-pos.x;
         *y=pos.z;
         *h=height;

         break;

      case 5: // new york

         dist=intersect_line_plane(pos,pos,miniv3d(0.0,-EARTH_radius,0.0),miniv3d(0.0,-EARTH_radius,0.0));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=pos.x;
         *y=pos.z;
         *h=height;

         break;

      case 6: // south pole

         dist=intersect_line_plane(pos,pos,miniv3d(0.0,0.0,-EARTH_radius),miniv3d(0.0,0.0,-EARTH_radius));
         if (dist<0.0) dist=MAXFLOAT;
         pos+=dist*pos;

         *x=pos.y;
         *y=pos.x;
         *h=height;

         break;

      default: ERRORMSG();
      }
   }

void minicrs::ECEF2OGH(float xyz[3],
                       float *x,float *y,float *h,
                       int zone)
   {
   double txyz[3];
   double tx,ty,th;

   txyz[0]=xyz[0];
   txyz[1]=xyz[1];
   txyz[2]=xyz[2];

   ECEF2OGH(txyz,&tx,&ty,&th,zone);

   *x=tx;
   *y=ty;
   *h=th;
   }

// transform ECEF to OG/H zone
int minicrs::ECEF2OGHZ(double xyz[3]) // input ECEF coordinates
   {
   double prj[3];
   double h;

   ECEF2PRJ(xyz,prj,&h);

   if (dabs(prj[2])>=dabs(prj[0]) && dabs(prj[2])>=dabs(prj[1]))
      if (prj[2]>0.0) return(1); // north pole
      else return(6); // south pole
   else if (dabs(prj[1])>=dabs(prj[0]) && dabs(prj[1])>=dabs(prj[2]))
      if (prj[1]>0.0) return(4); // tokyo
      else return(5); // new york
   else if (dabs(prj[0])>=dabs(prj[1]) && dabs(prj[0])>=dabs(prj[2]))
      if (prj[0]>0.0) return(2); // greenwich
      else return(3); // honolulu

   return(0);
   }

// project ECEF to ellipsoid
void minicrs::ECEF2PRJ(double xyz[3], // input ECEF coordinates
                       double prj[3], // output ECEF coordinates
                       double *h) // output altitude
   {
   miniv3d pos0,pos,nrm;
   double dist;

   pos0=miniv3d(xyz);

   dist=intersect_ray_ellipsoid(miniv3d(0.0,0.0,0.0),pos0,miniv3d(0.0,0.0,0.0),WGS84_r_major,WGS84_r_major,WGS84_r_minor);
   pos=pos0*dist;

   prj[0]=pos.x;
   prj[1]=pos.y;
   prj[2]=pos.z;

   nrm=pos;
   nrm.normalize();

   *h=(pos0-pos)*nrm;
   }

// Molodensky transformation
void minicrs::molodensky(double *lat,double *lon,double *h, // transformed coordinates
                         double r_maj,double f,             // semi-major axis and flattening
                         double dr_maj,double df,           // ellipsoid change
                         double dx,double dy,double dz)     // origin change
   {
   double es;                  // eccentricity squared
   double slat,clat,slon,clon; // sine and cosine values
   double ssqlat,asp,rn,rm;    // temporary variables
   double dlat,dlon,dh;        // datum shift

   es=2*f-f*f;

   slat=sin(*lat*2*PI/(360*60*60));
   clat=cos(*lat*2*PI/(360*60*60));
   slon=sin(*lon*2*PI/(360*60*60));
   clon=cos(*lon*2*PI/(360*60*60));

   ssqlat=slat*slat;
   asp=1.0/(1.0-f);

   rn=r_major/sqrt(1.0-es*ssqlat);
   rm=r_major*(1.0-es)/pow(1.0-es*ssqlat,1.5);

   // latitude shift
   dlat=(-dx*slat*clon-dy*slat*slon+dz*clat+dr_maj*(rn*es*slat*clat)/r_maj+df*(rm*asp+rn/asp)*slat*clat)/(rm+*h);

   // longitude shift
   dlon=(-dx*slon+dy*clon)/((rn+*h)*clat);

   // height shift
   dh=dx*clat*clon+dy*clat*slon+dz*slat-dr_maj*r_maj/rn+df*rn*ssqlat/asp;

   *lat+=dlat*360*60*60/(2*PI);
   *lon+=dlon*360*60*60/(2*PI);
   *h+=dh;
   }

// Molodensky transformation between two datums
void minicrs::molodensky(int src,int dst,double *lat,double *lon)
   {
   double r_maj,r_min; // src semi-major and minor radius
   double dx,dy,dz;    // src origin shift
   double fs,fd;       // src and dst flattening
   double h;           // transformed height

   if (src==dst) return;

   // treat WGS84 and GRS80 datum as identical
   if (src==3 && dst==4) return;
   if (src==4 && dst==3) return;

   choose_datum(src);

   r_maj=r_major;
   r_min=r_minor;

   dx=o_dx;
   dy=o_dy;
   dz=o_dz;

   choose_datum(dst);

   fs=(r_maj-r_min)/r_maj;
   fd=(r_major-r_minor)/r_major;

   h=0.0;
   molodensky(lat,lon,&h,r_maj,fs,r_major-r_maj,fd-fs,dx-o_dx,dy-o_dy,dz-o_dz);
   }

// 1 arc-second equals about 30 meters
// 1 arc-minute equals about 1.8 kilometers
// 1 arc-degree equals about 111 kilometers (at the equator)
void minicrs::arcsec2meter(double lat,double *as2m)
   {
   if (lat<-90*60*60 || lat>90*60*60) ERRORMSG();

   as2m[1]=2*PI*EARTH_radius/(360*60*60);
   as2m[0]=as2m[1]*cos(lat*2*PI/(360*60*60));
   }

void minicrs::arcsec2meter(double lat,float *as2m)
   {
   if (lat<-90*60*60 || lat>90*60*60) ERRORMSG();

   as2m[1]=(float)(2*PI*EARTH_radius/(360*60*60));
   as2m[0]=(float)(as2m[1]*cos(lat*2*PI/(360*60*60)));
   }

// adapted coordinate system transformations from GCTP,
// the General Cartographic Transformation Package:

// per-zone parameters of the Universal Transverse Mercator (UTM) projection
double minicrs::lon_center; // center longitude
double minicrs::e0,minicrs::e1,minicrs::e2,minicrs::e3; // eccentricity constants
double minicrs::e,minicrs::es,minicrs::esp; // eccentricity constants
double minicrs::false_northing; // y offset in meters
double minicrs::false_easting; // x offset in meters

// initialize the Universal Transverse Mercator (UTM) projection
void minicrs::initUTM(int zone, // zone number
                      double ratio) // axis ratio = 1-flattening
   {
   if (zone==0 || abs(zone)>60) ERRORMSG();
   if (ratio<=0.0 || ratio>1.0) ERRORMSG();

   if (zone==crs_zone && ratio==crs_ratio) return;

   lon_center=(6*abs(zone)-183)*60*60;

   false_easting=500000.0;
   false_northing=(zone<0)?10000000.0:0.0;

   es=1.0-dsqr(ratio);
   e=sqrt(es);

   e0=1.0-0.25*es*(1.0+0.0625*es*(3.0+1.25*es));
   e1=0.375*es*(1.0+0.25*es*(1.0+0.46875*es));
   e2=0.05859375*es*es*(1.0+0.75*es);
   e3=35.0/3072.0*es*es*es;

   esp=es/(1.0-es);

   crs_zone=zone;
   crs_ratio=ratio;
   }

// calculate the UTM equations
void minicrs::calcLL2UTM(double lat,double lon, // geographic input coordinates (Lat/Lon)
                         double *x,double *y)   // output UTM coordinates (Easting and Northing)
   {
   double delta_lon;       // delta longitude
   double sin_phi,cos_phi; // sine and cosine values
   double a,as,t,ts,n,m,b; // temporary variables

   delta_lon=LONSUB(lon,lon_center);

   lon*=2*PI/(360*60*60);
   lat*=2*PI/(360*60*60);

   delta_lon*=2*PI/(360*60*60);

   sin_phi=sin(lat);
   cos_phi=cos(lat);

   a=cos_phi*delta_lon;
   as=a*a;

   t=tan(lat);
   ts=t*t;

   n=r_major/sqrt(1.0-es*sin_phi*sin_phi);
   m=r_major*(e0*lat-e1*sin(2.0*lat)+e2*sin(4.0*lat)-e3*sin(6.0*lat));

   b=esp*cos_phi*cos_phi;

   // UTM Easting in meters
   *x=scale_factor*n*a*(1.0+as/6.0*(1.0-ts+b+as/20.0*(5.0-18.0*ts+ts*ts+72.0*b-58.0*esp)))+false_easting;

   // UTM Northing in meters
   *y=scale_factor*(m+n*t*as*(0.5+as/24.0*(5.0-ts+9.0*b+4.0*b*b+as/30.0*(61.0-58.0*ts+ts*ts+600.0*b-330.0*esp))))+false_northing;
   }

// calculate the inverse UTM equations
void minicrs::calcUTM2LL(double x,double y,       // input UTM coordinates (Easting and Northing)
                         double *lat,double *lon) // geographic output coordinates (Lat/Lon)
   {
   int i;

   double con,phi;                 // temporary angles
   double delta_phi;               // difference between longitudes
   double sin_phi,cos_phi,tan_phi; // sine, cosine and tangent values
   double a,as,t,ts,n,r,b,bs;      // temporary variables

   static const int maxiter=10;
   static const double maxerror=1E-20;

   x-=false_easting;
   y-=false_northing;

   con=y/(r_major*scale_factor);
   phi=con;

   for (i=0; i<maxiter; i++)
      {
      delta_phi=(con+e1*sin(2.0*phi)-e2*sin(4.0*phi)+e3*sin(6.0*phi))/e0-phi;
      phi+=delta_phi;

      if (dabs(delta_phi)<maxerror) break;
      }

   sin_phi=sin(phi);
   cos_phi=cos(phi);
   tan_phi=tan(phi);

   a=esp*cos_phi*cos_phi;
   as=a*a;

   t=tan_phi*tan_phi;
   ts=t*t;

   con=1.0-es*sin_phi*sin_phi;
   n=r_major/sqrt(con);
   r=n*(1.0-es)/con;

   b=x/(n*scale_factor);
   bs=b*b;

   // UTM latitude extends from -90 to 90 degrees
   *lat=(phi-n*tan_phi*bs/r*(0.5-bs/24.0*(5.0+3.0*t+10.0*a-4.0*as-9.0*esp-bs/30.0*(61.0+90.0*t+298.0*a+45.0*ts-252.0*esp-3.0*as))))*360*60*60/(2*PI);

   // UTM longitude extends from -180 to 180 degrees
   *lon=LONSUB(lon_center+b*(1.0-bs/6.0*(1.0+2.0*t+a-bs/20.0*(5.0-2.0*a+28.0*t-3.0*as+8.0*esp+24.0*ts)))/cos_phi*360*60*60/(2*PI));
   }

// calculate the Mercator equations
void minicrs::calcLL2MERC(double lat,double lon,double *x,double *y,double lat_center,double lon_center)
   {
   double e;  // eccentricity
   double es; // eccentricity squared
   double eh; // eccentricity half

   double phi;
   double phi_center;

   double co;
   double m1;
   double ts;

   static const double epsln=1E-5;

   es=1.0-dsqr(r_minor/r_major);
   e=sqrt(es);
   eh=0.5*e;

   phi=lat*2*PI/(360*60*60);
   phi_center=lat_center*2*PI/(360*60*60);

   if (phi>0.5*PI-epsln) phi=0.5*PI-epsln;
   if (phi<-0.5*PI+epsln) phi=-0.5*PI+epsln;

   co=e*sin(phi);
   co=pow(((1.0-co)/(1.0+co)),eh);

   if (phi_center==0.0) m1=1.0;
   else m1=cos(phi_center)/sqrt(1.0-dsqr(e*sin(phi_center)));

   ts=tan(0.5*(0.5*PI-phi))/co;

   *x=r_major*m1*LONSUB(lon,lon_center)*2*PI/(360*60*60);
   *y=-r_major*m1*log(ts);
   }

// calculate the inverse Mercator equations
void minicrs::calcMERC2LL(double x,double y,double *lat,double *lon,double lat_center,double lon_center)
   {
   int i;

   double e;  // eccentricity
   double es; // eccentricity squared
   double eh; // eccentricity half

   double phi,dphi;
   double phi_center;

   double co;
   double m1;
   double ts;

   static const int maxiter=10;
   static const double maxerror=1E-20;

   es=1.0-dsqr(r_minor/r_major);
   e=sqrt(es);
   eh=0.5*e;

   phi_center=lat_center*2*PI/(360*60*60);

   if (phi_center==0.0) m1=1.0;
   else m1=cos(phi_center)/sqrt(1.0-dsqr(e*sin(phi_center)));

   ts=exp(-y/(r_major*m1));

   phi=0.5*PI-2*atan(ts);

   for (i=0; i<maxiter; i++)
      {
      co=e*sin(phi);
      dphi=0.5*PI-2*atan(ts*(pow(((1.0-co)/(1.0+co)),eh)))-phi;
      phi+=dphi;

      if (dabs(dphi)<maxerror) break;
      }

   *lat=phi*360*60*60/(2*PI);
   *lon=LONSUB(lon_center+x/(r_major*m1)*360*60*60/(2*PI));
   }
