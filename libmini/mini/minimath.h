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
   minifixed(const double v)
      {
      double av;

      S=(v>=0.0);
      av=(S)?v:-v;

      M=N(floor(av));
      F=N(av-floor(av));
      }

   //! destructor
   ~minifixed() {}

   BOOLINT getsgn() const {return(S);}
   N getmag() const {return(M);}
   N getfrc() const {return(F);}

   static minifixed zero() {return(minifixed(TRUE,N::zero(),N::zero()));}
   static minifixed one() {return(minifixed(TRUE,N::one(),N::zero()));}

   BOOLINT isequal(const minifixed &value) const {return(S==value.getsgn() && M.isequal(value.getmag()) && F.isequal(value.getfrc()));}

   BOOLINT add(const minifixed &value,minifixed &result) const
      {
      N result1,result2;
      BOOLINT overflow1,overflow2;

      if (getsgn())
         if (value.getsgn())
            {
            overflow1=getfrc().add(value.getfrc(),result1);
            overflow2=getmag().add(value.getmag(),result2);

            if (overflow1) overflow1=result2.add(N::one(),result2);
            result=minifixed(TRUE,result2,result1);
            }
         else
            {
            overflow1=getmag().sub(value.getmag(),result1);
            overflow2=getfrc().sub(value.getfrc(),result2);

            if (overflow2) overflow2=result1.sub(N::one(),result1);

            if (overflow1 || overflow2) result=minifixed(FALSE,result1,result2);
            else result=minifixed(TRUE,result1,result2);

            return(FALSE);
            }
      else
         if (value.getsgn())
            {
            overflow1=value.getmag().sub(getmag(),result1);
            overflow2=value.getfrc().sub(getfrc(),result2);

            if (overflow2) overflow2=result1.sub(N::one(),result1);

            if (overflow1 || overflow2) result=minifixed(FALSE,result1,result2);
            else result=minifixed(TRUE,result1,result2);

            return(FALSE);
            }
         else
            {
            overflow1=getfrc().add(value.getfrc(),result1);
            overflow2=getmag().add(value.getmag(),result2);

            if (overflow1) overflow1=result2.add(N::one(),result2);
            result=minifixed(FALSE,result2,result1);
            }

      return(overflow1 || overflow2);
      }

   minifixed neg() const {return(minifixed(!getsgn(),getmag(),getfrc()));}
   minifixed abs() const {return(minifixed(TRUE,getmag(),getfrc()));}

   BOOLINT sub(const minifixed &value,minifixed &result) const {return(add(value.neg(),result));}

   BOOLINT grt(const minifixed &value) const
      {
      minifixed result;
      sub(value,result);

      return(result.getsgn());
      }

   minifixed min(const minifixed &value) {return((grt(value))?*this:value);}
   minifixed max(const minifixed &value) {return((grt(value))?value:*this);}

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
   minifixed_base(const unsigned long long int v) {V=v;}

   //! constructor
   minifixed_base(const double v) {V=(unsigned long long int)floor(FABS(v));}

   //! destructor
   ~minifixed_base() {}

   static minifixed_base zero() {return(minifixed_base((unsigned long long int)0));}
   static minifixed_base one() {return(minifixed_base((unsigned long long int)1));}

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

   unsigned long long int V;
   };

typedef minifixed<minifixed_base> minifixed1;
typedef minifixed<minifixed1> minifixed2;
typedef minifixed<minifixed2> minifixed4;

#endif
