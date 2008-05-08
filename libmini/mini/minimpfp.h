// (c) by Stefan Roettger

#ifndef MINIMPFP_H
#define MINIMPFP_H

#include "minibase.h"

// multi-precision fixed-point arithmetic:

template <class N>
class minimpfp
   {
   public:

   //! default constructor
   minimpfp() {}

   //! constructor
   minimpfp(const BOOLINT s,const N &m,const N &f)
      {
      S=s;
      M=m;
      F=f;
      }

   //! constructor
   minimpfp(const N &m,const N &f)
      {
      S=TRUE;
      M=m;
      F=f;
      }

   //! constructor
   minimpfp(const double v) {set(v);}

   //! destructor
   ~minimpfp() {}

   static unsigned int getbits() {return(2*N::getbits());}
   static double getlimit() {return(N::getlimit()*N::getlimit());}

   static minimpfp zero() {return(minimpfp(N::zero(),N::zero()));}
   static minimpfp one() {return(minimpfp(N::one(),N::zero()));}

   static minimpfp min() {return(minimpfp(N::zero(),N::min()));}
   static minimpfp max() {return(minimpfp(N::max(),N::max()));}

   minimpfp maxval() const {return(minimpfp(S,N::max(),N::max()));}

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

   double get() const
      {
      double v;

      v=M.get()*N::getlimit();
      v+=F.get()/N::getlimit();

      return(S?v:-v);
      }

   BOOLINT iszero() const
      {
      if (M.iszero())
         if (F.iszero()) return(TRUE);

      return(FALSE);
      }

   BOOLINT isnotzero() const
      {
      if (M.isnotzero()) return(TRUE);
      if (F.isnotzero()) return(TRUE);

      return(FALSE);
      }

   BOOLINT isequal(const minimpfp &value) const
      {
      if (M.isequal(value.getmag()))
         if (F.isequal(value.getfrc()))
            if (S==value.getsgn()) return(TRUE);
            else
               if (M.iszero())
                  if (F.iszero()) return(TRUE);

      return(FALSE);
      }

   BOOLINT isnotequal(const minimpfp &value) const
      {
      if (M.isnotequal(value.getmag())) return(TRUE);
      if (F.isnotequal(value.getfrc())) return(TRUE);
      if (S==value.getsgn()) return(FALSE);

      if (M.isnotzero()) return(TRUE);
      if (F.isnotzero()) return(TRUE);

      return(FALSE);
      }

   minimpfp neg() const {return(minimpfp(!S,M,F));}
   minimpfp abs() const {return(minimpfp(M,F));}

   void add(const minimpfp &value,minimpfp &result) const
      {
      if (add2(value,result))
         if (!S^value.getsgn()) result=result.maxval();
      }

   BOOLINT add2(const minimpfp &value,minimpfp &result) const
      {
      N result1,result2;
      BOOLINT overflow1,overflow2;

      if (S)
         if (value.getsgn())
            {
            overflow1=F.add2(value.getfrc(),result1);
            overflow2=M.add2(value.getmag(),result2);

            if (overflow1) overflow1=result2.add2(N::min(),result2);

            result=minimpfp(result2,result1);
            }
         else
            {
            overflow1=M.sub2(value.getmag(),result1);

            if (overflow1)
               {
               overflow1=value.getmag().sub2(M,result1);
               overflow2=value.getfrc().sub2(F,result2);

               if (overflow2) overflow2=result1.sub2(N::min(),result1);

               result=minimpfp(FALSE,result1,result2);
               }
            else
               {
               overflow2=F.sub2(value.getfrc(),result2);

               if (overflow2)
                  if (result1.isnotzero()) overflow2=result1.sub2(N::min(),result1);
                  else overflow2=!value.getfrc().sub2(F,result2);

               result=minimpfp(!overflow2,result1,result2);
               }
            }
      else
         if (value.getsgn())
            {
            overflow1=value.getmag().sub2(M,result1);

            if (overflow1)
               {
               overflow1=M.sub2(value.getmag(),result1);
               overflow2=F.sub2(value.getfrc(),result2);

               if (overflow2) overflow2=result1.sub2(N::min(),result1);

               result=minimpfp(FALSE,result1,result2);
               }
            else
               {
               overflow2=value.getfrc().sub2(F,result2);

               if (overflow2)
                  if (result1.isnotzero()) overflow2=result1.sub2(N::min(),result1);
                  else overflow2=!F.sub2(value.getfrc(),result2);

               result=minimpfp(!overflow2,result1,result2);
               }
            }
         else
            {
            overflow1=F.add2(value.getfrc(),result1);
            overflow2=M.add2(value.getmag(),result2);

            if (overflow1) overflow1=result2.add2(N::min(),result2);

            result=minimpfp(FALSE,result2,result1);
            }

      return(overflow1 || overflow2);
      }

   void sub(const minimpfp &value,minimpfp &result) const
      {add(value.neg(),result);}

   BOOLINT sub2(const minimpfp &value,minimpfp &result) const
      {return(add2(value.neg(),result));}

   BOOLINT grt(const minimpfp &value) const
      {
      minimpfp result;

      sub(value,result);

      if (result.getsgn())
         if (result.isnotequal(zero())) return(TRUE);

      return(FALSE);
      }

   minimpfp min(const minimpfp &value) {return((grt(value))?*this:value);}
   minimpfp max(const minimpfp &value) {return((grt(value))?value:*this);}

   void mul(const minimpfp &value,minimpfp &result) const
      {if (mul2(value,result).isnotequal(zero())) result=result.maxval();}

   minimpfp mul2(const minimpfp &value,minimpfp &result) const
      {
      BOOLINT sign;

      N result1,result2,result3,result4;
      N overflow1,overflow2,overflow3,overflow4;

      sign=!(S^value.getsgn());

      overflow1=F.mul2(value.getfrc(),result1);
      overflow2=F.mul2(value.getmag(),result2);
      overflow3=M.mul2(value.getfrc(),result3);
      overflow4=M.mul2(value.getmag(),result4);

      result=minimpfp(sign,N::zero(),N(overflow1.getmag(),result1.getmag()));
      result.add2(minimpfp(sign,N(overflow2.getmag(),result2.getmag()),N(result2.getfrc(),overflow2.getfrc())),result);
      result.add2(minimpfp(sign,N(overflow3.getmag(),result3.getmag()),N(result3.getfrc(),overflow3.getfrc())),result);
      result.add2(minimpfp(sign,N(result4.getfrc(),overflow4.getfrc()),N::zero()),result);

      return(minimpfp(N(overflow4.getmag(),result4.getmag()),N(result1.getfrc(),overflow1.getfrc())));
      }

   private:

   BOOLINT S;
   N M,F;
   };

class minimpfp_base
   {
   public:

   //! default constructor
   minimpfp_base() {}

   //! constructor
   minimpfp_base(const unsigned int m,const unsigned int f) {V=(m<<16)+f;}

   //! constructor
   minimpfp_base(const double v) {set(v);}

   //! destructor
   ~minimpfp_base() {}

   static unsigned int getbits() {return(32);}
   static double getlimit() {return((double)(1<<16));}

   static minimpfp_base zero() {return(minimpfp_base(0,0));}
   static minimpfp_base one() {return(minimpfp_base(1,0));}

   static minimpfp_base min() {return(minimpfp_base(0,1));}
   static minimpfp_base max() {return(minimpfp_base((unsigned int)-1,(unsigned int)-1));}

   unsigned int getmag() const {return(V>>16);}
   unsigned int getfrc() const {return(V&((1<<16)-1));}

   void set(const double v) {V=(unsigned int)floor(v*(1<<16)+0.5);}
   double get() const {return(V/(double)(1<<16));}

   BOOLINT iszero() const {return(V==0);}
   BOOLINT isnotzero() const {return(V!=0);}

   BOOLINT isequal(const minimpfp_base &value) const {return(value.V==V);}
   BOOLINT isnotequal(const minimpfp_base &value) const {return(value.V!=V);}

   BOOLINT add2(const minimpfp_base &value,minimpfp_base &result) const
      {
      result.V=V+value.V;
      return(result.V<V);
      }

   BOOLINT sub2(const minimpfp_base &value,minimpfp_base &result) const
      {
      result.V=V-value.V;
      return(result.V>V);
      }

   minimpfp_base mul2(const minimpfp_base &value,minimpfp_base &result) const
      {
      unsigned long long int mv;
      mv=(unsigned long long int)V*(unsigned long long int)value.V;
      result.V=(unsigned int)(mv>>16);
      return(minimpfp_base((unsigned int)(mv>>48),(unsigned int)(mv&((1<<16)-1))));
      }

   unsigned int V;
   };

typedef minimpfp<minimpfp_base> minimpfp1; // 64bit precision
typedef minimpfp<minimpfp1> minimpfp2;     // 128bit precision
typedef minimpfp<minimpfp2> minimpfp4;     // 256 bit precision

#endif
