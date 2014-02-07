// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "glslmath.h"

// calculate determinant of 4x4 matrix
double mat4::det() const
   {
   return(mtx[0][3]*mtx[1][2]*mtx[2][1]*mtx[3][0]-
          mtx[0][2]*mtx[1][3]*mtx[2][1]*mtx[3][0]-
          mtx[0][3]*mtx[1][1]*mtx[2][2]*mtx[3][0]+
          mtx[0][1]*mtx[1][3]*mtx[2][2]*mtx[3][0]+
          mtx[0][2]*mtx[1][1]*mtx[2][3]*mtx[3][0]-
          mtx[0][1]*mtx[1][2]*mtx[2][3]*mtx[3][0]-
          mtx[0][3]*mtx[1][2]*mtx[2][0]*mtx[3][1]+
          mtx[0][2]*mtx[1][3]*mtx[2][0]*mtx[3][1]+
          mtx[0][3]*mtx[1][0]*mtx[2][2]*mtx[3][1]-
          mtx[0][0]*mtx[1][3]*mtx[2][2]*mtx[3][1]-
          mtx[0][2]*mtx[1][0]*mtx[2][3]*mtx[3][1]+
          mtx[0][0]*mtx[1][2]*mtx[2][3]*mtx[3][1]+
          mtx[0][3]*mtx[1][1]*mtx[2][0]*mtx[3][2]-
          mtx[0][1]*mtx[1][3]*mtx[2][0]*mtx[3][2]-
          mtx[0][3]*mtx[1][0]*mtx[2][1]*mtx[3][2]+
          mtx[0][0]*mtx[1][3]*mtx[2][1]*mtx[3][2]+
          mtx[0][1]*mtx[1][0]*mtx[2][3]*mtx[3][2]-
          mtx[0][0]*mtx[1][1]*mtx[2][3]*mtx[3][2]-
          mtx[0][2]*mtx[1][1]*mtx[2][0]*mtx[3][3]+
          mtx[0][1]*mtx[1][2]*mtx[2][0]*mtx[3][3]+
          mtx[0][2]*mtx[1][0]*mtx[2][1]*mtx[3][3]-
          mtx[0][0]*mtx[1][2]*mtx[2][1]*mtx[3][3]-
          mtx[0][1]*mtx[1][0]*mtx[2][2]*mtx[3][3]+
          mtx[0][0]*mtx[1][1]*mtx[2][2]*mtx[3][3]);
   }

// invert 4x4 matrix
mat4 mat4::invert() const
   {
   mat4 m;
   double d;

   // calculate determinant
   d=det();

   // check determinant
   assert(d!=0.0);

   // calculate inverse
   d=1.0/d;
   m.mtx[0][0]=d*(mtx[1][2]*mtx[2][3]*mtx[3][1]-mtx[1][3]*mtx[2][2]*mtx[3][1]+mtx[1][3]*mtx[2][1]*mtx[3][2]-mtx[1][1]*mtx[2][3]*mtx[3][2]-mtx[1][2]*mtx[2][1]*mtx[3][3]+mtx[1][1]*mtx[2][2]*mtx[3][3]);
   m.mtx[0][1]=d*(mtx[0][3]*mtx[2][2]*mtx[3][1]-mtx[0][2]*mtx[2][3]*mtx[3][1]-mtx[0][3]*mtx[2][1]*mtx[3][2]+mtx[0][1]*mtx[2][3]*mtx[3][2]+mtx[0][2]*mtx[2][1]*mtx[3][3]-mtx[0][1]*mtx[2][2]*mtx[3][3]);
   m.mtx[0][2]=d*(mtx[0][2]*mtx[1][3]*mtx[3][1]-mtx[0][3]*mtx[1][2]*mtx[3][1]+mtx[0][3]*mtx[1][1]*mtx[3][2]-mtx[0][1]*mtx[1][3]*mtx[3][2]-mtx[0][2]*mtx[1][1]*mtx[3][3]+mtx[0][1]*mtx[1][2]*mtx[3][3]);
   m.mtx[0][3]=d*(mtx[0][3]*mtx[1][2]*mtx[2][1]-mtx[0][2]*mtx[1][3]*mtx[2][1]-mtx[0][3]*mtx[1][1]*mtx[2][2]+mtx[0][1]*mtx[1][3]*mtx[2][2]+mtx[0][2]*mtx[1][1]*mtx[2][3]-mtx[0][1]*mtx[1][2]*mtx[2][3]);
   m.mtx[1][0]=d*(mtx[1][3]*mtx[2][2]*mtx[3][0]-mtx[1][2]*mtx[2][3]*mtx[3][0]-mtx[1][3]*mtx[2][0]*mtx[3][2]+mtx[1][0]*mtx[2][3]*mtx[3][2]+mtx[1][2]*mtx[2][0]*mtx[3][3]-mtx[1][0]*mtx[2][2]*mtx[3][3]);
   m.mtx[1][1]=d*(mtx[0][2]*mtx[2][3]*mtx[3][0]-mtx[0][3]*mtx[2][2]*mtx[3][0]+mtx[0][3]*mtx[2][0]*mtx[3][2]-mtx[0][0]*mtx[2][3]*mtx[3][2]-mtx[0][2]*mtx[2][0]*mtx[3][3]+mtx[0][0]*mtx[2][2]*mtx[3][3]);
   m.mtx[1][2]=d*(mtx[0][3]*mtx[1][2]*mtx[3][0]-mtx[0][2]*mtx[1][3]*mtx[3][0]-mtx[0][3]*mtx[1][0]*mtx[3][2]+mtx[0][0]*mtx[1][3]*mtx[3][2]+mtx[0][2]*mtx[1][0]*mtx[3][3]-mtx[0][0]*mtx[1][2]*mtx[3][3]);
   m.mtx[1][3]=d*(mtx[0][2]*mtx[1][3]*mtx[2][0]-mtx[0][3]*mtx[1][2]*mtx[2][0]+mtx[0][3]*mtx[1][0]*mtx[2][2]-mtx[0][0]*mtx[1][3]*mtx[2][2]-mtx[0][2]*mtx[1][0]*mtx[2][3]+mtx[0][0]*mtx[1][2]*mtx[2][3]);
   m.mtx[2][0]=d*(mtx[1][1]*mtx[2][3]*mtx[3][0]-mtx[1][3]*mtx[2][1]*mtx[3][0]+mtx[1][3]*mtx[2][0]*mtx[3][1]-mtx[1][0]*mtx[2][3]*mtx[3][1]-mtx[1][1]*mtx[2][0]*mtx[3][3]+mtx[1][0]*mtx[2][1]*mtx[3][3]);
   m.mtx[2][1]=d*(mtx[0][3]*mtx[2][1]*mtx[3][0]-mtx[0][1]*mtx[2][3]*mtx[3][0]-mtx[0][3]*mtx[2][0]*mtx[3][1]+mtx[0][0]*mtx[2][3]*mtx[3][1]+mtx[0][1]*mtx[2][0]*mtx[3][3]-mtx[0][0]*mtx[2][1]*mtx[3][3]);
   m.mtx[2][2]=d*(mtx[0][1]*mtx[1][3]*mtx[3][0]-mtx[0][3]*mtx[1][1]*mtx[3][0]+mtx[0][3]*mtx[1][0]*mtx[3][1]-mtx[0][0]*mtx[1][3]*mtx[3][1]-mtx[0][1]*mtx[1][0]*mtx[3][3]+mtx[0][0]*mtx[1][1]*mtx[3][3]);
   m.mtx[2][3]=d*(mtx[0][3]*mtx[1][1]*mtx[2][0]-mtx[0][1]*mtx[1][3]*mtx[2][0]-mtx[0][3]*mtx[1][0]*mtx[2][1]+mtx[0][0]*mtx[1][3]*mtx[2][1]+mtx[0][1]*mtx[1][0]*mtx[2][3]-mtx[0][0]*mtx[1][1]*mtx[2][3]);
   m.mtx[3][0]=d*(mtx[1][2]*mtx[2][1]*mtx[3][0]-mtx[1][1]*mtx[2][2]*mtx[3][0]-mtx[1][2]*mtx[2][0]*mtx[3][1]+mtx[1][0]*mtx[2][2]*mtx[3][1]+mtx[1][1]*mtx[2][0]*mtx[3][2]-mtx[1][0]*mtx[2][1]*mtx[3][2]);
   m.mtx[3][1]=d*(mtx[0][1]*mtx[2][2]*mtx[3][0]-mtx[0][2]*mtx[2][1]*mtx[3][0]+mtx[0][2]*mtx[2][0]*mtx[3][1]-mtx[0][0]*mtx[2][2]*mtx[3][1]-mtx[0][1]*mtx[2][0]*mtx[3][2]+mtx[0][0]*mtx[2][1]*mtx[3][2]);
   m.mtx[3][2]=d*(mtx[0][2]*mtx[1][1]*mtx[3][0]-mtx[0][1]*mtx[1][2]*mtx[3][0]-mtx[0][2]*mtx[1][0]*mtx[3][1]+mtx[0][0]*mtx[1][2]*mtx[3][1]+mtx[0][1]*mtx[1][0]*mtx[3][2]-mtx[0][0]*mtx[1][1]*mtx[3][2]);
   m.mtx[3][3]=d*(mtx[0][1]*mtx[1][2]*mtx[2][0]-mtx[0][2]*mtx[1][1]*mtx[2][0]+mtx[0][2]*mtx[1][0]*mtx[2][1]-mtx[0][0]*mtx[1][2]*mtx[2][1]-mtx[0][1]*mtx[1][0]*mtx[2][2]+mtx[0][0]*mtx[1][1]*mtx[2][2]);

   return(m);
   }
