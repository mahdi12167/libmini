// (c) by Stefan Roettger

#ifndef MINIMATH_H
#define MINIMATH_H

#include "miniv3d.h"
#include "miniv4d.h"

namespace minimath {

// basic mathematical operations:

unsigned int gcd(unsigned int a,unsigned int b); // greatest common divisor
unsigned int lcm(unsigned int a,unsigned int b); // lowest common multiple

// matrix operations:

void set_mtx(miniv3d mtx[3]);
void set_mtx(miniv4d mtx[3]);

void cpy_mtx(miniv3d cpy[3],miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],miniv4d mtx[3]);
void cpy_mtx(miniv3d cpy[3],miniv4d mtx[3]);

void cpy_mtx(miniv3d cpy[3],const miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],const miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],const miniv4d mtx[3]);
void cpy_mtx(miniv3d cpy[3],const miniv4d mtx[3]);

void cpy_mtx4(miniv4d cpy[4],const miniv4d mtx[4]);

inline miniv3d mlt_vec(const miniv3d mtx[3],const miniv3d &vec)
   {return(miniv3d(mtx[0]*vec,mtx[1]*vec,mtx[2]*vec));}

inline miniv3d mlt_vec(const miniv4d mtx[3],const miniv4d &vec)
   {return(miniv3d(mtx[0]*vec,mtx[1]*vec,mtx[2]*vec));}

inline miniv3d mlt_vec(const miniv4d mtx[3],const miniv3d &vec)
   {return(mlt_vec(mtx,miniv4d(vec,1)));}

void mlt_mtx(miniv3d mtx[3],const miniv3d mtx1[3],const miniv3d mtx2[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3]);

void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3],const miniv4d mtx8[3]);

void mlt_mtx4(miniv4d mtx[4],const miniv4d mtx1[4],const miniv4d mtx2[4]);

double det_mtx(const miniv3d mtx[3]);
double det_mtx(const miniv4d mtx[3]);

double det_mtx4(const miniv4d mtx[4]);

void inv_mtx(miniv3d inv[3],const miniv3d mtx[3]);
void inv_mtx(miniv3d inv[3],const miniv4d mtx[3]);
void inv_mtx(miniv4d inv[3],const miniv4d mtx[3]);

void inv_mtx4(miniv4d inv[4],const miniv4d mtx[4]);

void tra_mtx(miniv3d tra[3],const miniv3d mtx[3]);
void tra_mtx(miniv3d tra[3],const miniv4d mtx[3]);
void tra_mtx(miniv4d tra[3],const miniv4d mtx[3]);

void tra_mtx4(miniv4d tra[4],const miniv4d mtx[4]);

void rot_mtx(miniv3d rot[3],double delta,const miniv3d &axis);
void rot_mtx(miniv3d rot[3],const miniv3d &v1,const miniv3d &v2);

// geometric operations:

double intersect_ray_unitsphere(miniv3d p,miniv3d d);

double intersect_ray_ellipsoid(miniv3d p,miniv3d d,
                               miniv3d o,double r1,double r2,double r3);

double intersect_ray_plane(miniv3d p,miniv3d d,
                           miniv3d o,miniv3d n);

double intersect_line_plane(miniv3d p,miniv3d d,
                            miniv3d o,miniv3d n);

// Moeller-Trumbore ray/triangle intersection:

int intersect_ray_triangle(const miniv3d &o,const miniv3d &d,
                           const miniv3d &v0,const miniv3d &v1,const miniv3d &v2,
                           miniv3d *tuv);

double ray_triangle_dist(const miniv3d &o,const miniv3d &d,
                         const miniv3d &v1,const miniv3d &v2,const miniv3d &v3);

// geometric tests:

int itest_ray_sphere(const miniv3d &o,const miniv3d &d,
                     const miniv3d &b,const double r2);

int itest_ray_bbox(const miniv3d &o,const miniv3d &d,
                   const miniv3d &b,const miniv3d &r);

int itest_plane_sphere(const miniv3d &o,const miniv3d &n,const double radius,
                       const miniv3d &b,const double r2);

}

using namespace minimath;

#endif
