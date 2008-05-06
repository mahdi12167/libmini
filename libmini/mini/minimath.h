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

void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3]);
void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3],const miniv4d mtx5[3],const miniv4d mtx6[3],const miniv4d mtx7[3],const miniv4d mtx8[3]);

double det_mtx(const miniv3d mtx[3]);

void inv_mtx(miniv3d inv[3],const miniv3d mtx[3]);
void inv_mtx(miniv4d inv[3],const miniv4d mtx[3]);

void tra_mtx(miniv3d tra[3],const miniv3d mtx[3]);
void tra_mtx(miniv4d tra[3],const miniv4d mtx[3]);

}

using namespace minimath;

// fixed point arithmetic:

template <class N>
class minifixed
   {
   public:

   //! default constructor
   minifixed()
      {
      S=TRUE;
      M=F=N::zero();
      }

   //! constructor
   minifixed(const BOOLINT s,const N &m,const N &f)
      {
      S=s;
      M=m;
      F=f;
      }

   //! constructor
   minifixed(const double v) {set(v);}

   //! destructor
   ~minifixed() {}

   static double getlimit() {return(N::getlimit()*N::getlimit());}

   static minifixed zero() {return(minifixed());}
   static minifixed one() {return(minifixed(TRUE,N::one(),N::zero()));}

   BOOLINT getsgn() const {return(S);}
   N getmag() const {return(M);}
   N getfrc() const {return(F);}

   void set(const double v)
      {
      double av;

      S=(v>=0.0);
      av=S?v:-v;

      M=N(floor(av)/N::getlimit());
      F=N((av-floor(av))*N::getlimit());
      }

   double get()
      {
      double v;

      v=M.get()*N::getlimit();
      v+=F.get()/N::getlimit();

      if (S) return(v);
      else return(-v);
      }

   BOOLINT isequal(const minifixed &value) const
      {
      if (M.isequal(value.getmag()))
         if (F.isequal(value.getfrc()))
            if (S==value.getsgn()) return(TRUE);
            else
               if (M.isequal(N::zero()))
                  if (F.isequal(N::zero())) return(TRUE);

      return(FALSE);
      }

   minifixed neg() const {return(minifixed(!S,M,F));}
   minifixed abs() const {return(minifixed(TRUE,M,F));}

   BOOLINT add(const minifixed &value,minifixed &result) const
      {
      N result1,result2;
      BOOLINT overflow1,overflow2;

      if (S)
         if (value.getsgn())
            {
            overflow1=F.add(value.getfrc(),result1);
            overflow2=M.add(value.getmag(),result2);

            if (overflow1) overflow1=result2.add(N::one(),result2);

            result=minifixed(TRUE,result2,result1);
            }
         else
            {
            overflow1=M.sub(value.getmag(),result1);
            overflow2=F.sub(value.getfrc(),result2);

            if (overflow2) overflow2=result1.sub(N::one(),result1);

            if (overflow1 || overflow2) result=minifixed(FALSE,result1,result2);
            else result=minifixed(TRUE,result1,result2);

            return(FALSE);
            }
      else
         if (value.getsgn())
            {
            overflow1=value.getmag().sub(M,result1);
            overflow2=value.getfrc().sub(F,result2);

            if (overflow2) overflow2=result1.sub(N::one(),result1);

            if (overflow1 || overflow2) result=minifixed(FALSE,result1,result2);
            else result=minifixed(TRUE,result1,result2);

            return(FALSE);
            }
         else
            {
            overflow1=F.add(value.getfrc(),result1);
            overflow2=M.add(value.getmag(),result2);

            if (overflow1) overflow1=result2.add(N::one(),result2);

            result=minifixed(FALSE,result2,result1);
            }

      return(overflow1 || overflow2);
      }

   BOOLINT sub(const minifixed &value,minifixed &result) const {return(add(value.neg(),result));}

   BOOLINT grt(const minifixed &value) const
      {
      minifixed result;

      sub(value,result);

      if (result.getsgn())
         if (!result.isequal(zero())) return(TRUE);

      return(FALSE);
      }

   minifixed min(const minifixed &value) {return((grt(value))?*this:value);}
   minifixed max(const minifixed &value) {return((grt(value))?value:*this);}

   minifixed mul(const minifixed &value,minifixed &result) const
      {
      N result1,result2,result3,result4;
      N overflow1,overflow2,overflow3,overflow4;

      overflow1=F.mul(value.getfrc(),result1);
      overflow2=F.mul(value.getmag(),result2);
      overflow3=M.mul(value.getfrc(),result3);
      overflow4=M.mul(value.getmag(),result4);

      result=minifixed(N::zero(),overflow1);
      result.add(minifixed(overflow2,result2),result);
      result.add(minifixed(overflow3,result3),result);
      result.add(minifixed(result4,N::zero),result);

      return(minifixed(N::zero(),overflow4));
      }

   private:

   BOOLINT S;
   N M,F;
   };

class minifixed_base
   {
   public:

   //! default constructor
   minifixed_base() {V=0;}

   //! constructor
   minifixed_base(const unsigned int m,const unsigned int f) {V=(m<<16)+f;}

   //! constructor
   minifixed_base(const double v) {set(v);}

   //! destructor
   ~minifixed_base() {}

   static double getlimit() {return((double)(1<<16));}

   static minifixed_base zero() {return(minifixed_base());}
   static minifixed_base one() {return(minifixed_base(1,0));}

   void set(const double v) {V=(unsigned int)floor(v*(1<<16)+0.5);}
   double get() {return(V/(double)(1<<16));}

   BOOLINT isequal(const minifixed_base &value) const {return(value.V==V);}

   BOOLINT add(const minifixed_base &value,minifixed_base &result) const
      {
      result.V=V+value.V;
      return(result.V<V);
      }

   BOOLINT sub(const minifixed_base &value,minifixed_base &result) const
      {
      result.V=V-value.V;
      return(result.V>V);
      }

   minifixed_base mul(const minifixed_base &value,minifixed_base &result) const
      {
      unsigned long long int mv;
      mv=(unsigned long long int)V*(unsigned long long int)value.V;
      result.V=(unsigned int)(mv>>16);
      return(minifixed_base(0,(unsigned int)(mv>>48)));
      }

   unsigned int V;
   };

typedef minifixed<minifixed_base> minifixed1;
typedef minifixed<minifixed1> minifixed2;
typedef minifixed<minifixed2> minifixed4;

#endif
