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

void cpy_mtx(miniv3d cpy[3],miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],miniv4d mtx[3]);
void cpy_mtx(miniv3d cpy[3],miniv4d mtx[3]);

void cpy_mtx(miniv3d cpy[3],const miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],const miniv3d mtx[3]);
void cpy_mtx(miniv4d cpy[3],const miniv4d mtx[3]);
void cpy_mtx(miniv3d cpy[3],const miniv4d mtx[3]);

void cpy_mtx4(miniv4d cpy[4],const miniv4d mtx[4]);

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

void rot_mtx(miniv3d rot[3],const miniv3d &v1,const miniv3d &v2);

double intersect_unitsphere(miniv3d p,miniv3d d);

double intersect_ellipsoid(miniv3d p,miniv3d d,
                           miniv3d o,double r1,double r2,double r3);

double intersect_ellipsoid_line(miniv3d p,miniv3d d,
                                miniv3d o,double r1,double r2,double r3);

double intersect_plane(miniv3d p,miniv3d d,
                       miniv3d o,miniv3d n);

double intersect_plane_line(miniv3d p,miniv3d d,
                            miniv3d o,miniv3d n);

}

using namespace minimath;

#endif
