// (c) by Stefan Roettger

#ifndef MINIMPFP_H
#define MINIMPFP_H

#include "iostream"

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
   static minimpfp one() {return(minimpfp(N::min(),N::zero()));}

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

      M=N(floor(av)*(1.0/N::getlimit()));
      F=N((av-floor(av))*N::getlimit());
      }

   double get() const
      {
      double v;

      v=M.get()*N::getlimit();
      v+=F.get()*(1.0/N::getlimit());

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

   minimpfp left() const {return(minimpfp(F,N::zero()));}
   minimpfp right() const {return(minimpfp(N::zero(),M));}

   minimpfp neg() const {return(minimpfp(!S,M,F));}
   minimpfp abs() const {return(minimpfp(M,F));}

   void nrm()
      {
      if (!S)
         {
         add(max());
         add(min());
         }
      }

   void cpm() {S=!S;}

   void add(const minimpfp &value,minimpfp &result) const
      {
      if (add2(value,result))
         if (!S^value.getsgn()) result=result.maxval();
      }

   void add(const minimpfp &value) {add(value,*this);}

   BOOLINT add2(const minimpfp &value,minimpfp &result) const
      {
      BOOLINT sign;
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
            sign=TRUE;

            overflow1=M.sub2(value.getmag(),result1);

            if (overflow1)
               {
               sign=FALSE;
               result1.cpm();

               overflow2=value.getfrc().sub2(F,result2);
               }
            else overflow2=F.sub2(value.getfrc(),result2);

            if (overflow2)
               {
               result2.nrm();
               overflow2=result1.sub2(N::min(),result1);

               if (overflow2)
                  {
                  sign=FALSE;
                  result1=N::zero();
                  result2.cpm();
                  result2.nrm();
                  }
               }

            result=minimpfp(sign,result1,result2);
            }
      else
         if (value.getsgn())
            {
            sign=TRUE;

            overflow1=value.getmag().sub2(M,result1);

            if (overflow1)
               {
               sign=FALSE;
               result1.cpm();

               overflow2=F.sub2(value.getfrc(),result2);
               }
            else overflow2=value.getfrc().sub2(F,result2);

            if (overflow2)
               {
               result2.nrm();
               overflow2=result1.sub2(N::min(),result1);

               if (overflow2)
                  {
                  sign=FALSE;
                  result1=N::zero();
                  result2.cpm();
                  result2.nrm();
                  }
               }

            result=minimpfp(sign,result1,result2);
            }
         else
            {
            overflow1=value.getfrc().add2(F,result1);
            overflow2=value.getmag().add2(M,result2);

            if (overflow1) overflow1=result2.add2(N::min(),result2);

            result=minimpfp(FALSE,result2,result1);
            }

      return(overflow1 || overflow2);
      }

   void sub(const minimpfp &value,minimpfp &result) const
      {add(value.neg(),result);}

   void sub(const minimpfp &value)
      {sub(value.neg(),*this);}

   BOOLINT sub2(const minimpfp &value,minimpfp &result) const
      {return(add2(value.neg(),result));}

   BOOLINT grt(const minimpfp &value) const
      {
      minimpfp result;

      sub(value,result);

      if (result.getsgn())
         if (result.isnotzero()) return(TRUE);

      return(FALSE);
      }

   minimpfp min(const minimpfp &value) {return((grt(value))?*this:value);}
   minimpfp max(const minimpfp &value) {return((grt(value))?value:*this);}

   void mul(const minimpfp &value,minimpfp &result) const
      {if (mul2(value,result).getmag().isnotzero()) result=result.maxval();}

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

   void div(const minimpfp &value,minimpfp &result) const
      {if (div2(value,result).getmag().isnotzero()) result=result.maxval();}

   minimpfp div2(const minimpfp &value,minimpfp &result) const
      {return(mul2(value.inv(),result));}

   minimpfp inv() const
      {
      BOOLINT sign;
      N result1,result2,result3;
      N overflow1;
      minimpfp overflow2;
      minimpfp remainder;

      if (iszero()) return(maxval());
      else if (M.iszero()) return(minimpfp(S,F.inv(),N::zero()));
      else if (F.iszero()) return(minimpfp(S,N::zero(),M.inv()));
      else if (isequal(one())) return(one());
      else
         {
         result1=M.inv();
         result2=result1;

         overflow1=M.left().mul2(F.right().inv(),result3);
         overflow2=mul2(minimpfp(result3.right(),result3.left()),remainder);

         sign=FALSE;

         while (overflow1.iszero() && overflow2.getmag().iszero())
            {
            result1=remainder.getmag().inv();

            if (sign) result2.add2(result1,result2);
            else result2.sub2(result1,result2);

            sign=!sign;

            overflow1=remainder.getmag().left().mul2(remainder.getfrc().right().inv(),result3);
            overflow2=remainder.mul2(minimpfp(result3.right(),result3.left()),remainder);
            }
         }

      return(minimpfp(S,N::zero(),result2));
      }

   minimpfp sqrt(minimpfp &result) const
      {
      minimpfp r,r2,e;

      r.set(sqrt(get()));

      do
         {
         div2(r,r2);
         r2.sub2(r,r2);
         r2.mul2(minimpfp(0.5),e);
         r.add2(e,r);
         }
      while (e.isnotzero());

      return(r);
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
   static minimpfp_base max() {return(minimpfp_base(0xFFFF,0xFFFF));}

   unsigned int getmag() const {return(V>>16);}
   unsigned int getfrc() const {return(V&0xFFFF);}

   void set(const double v) {V=(unsigned int)floor(v*(1<<16)+0.5);}
   double get() const {return(V*(1.0/(double)(1<<16)));}

   BOOLINT iszero() const {return(V==0);}
   BOOLINT isnotzero() const {return(V!=0);}

   BOOLINT isequal(const minimpfp_base &value) const {return(value.V==V);}
   BOOLINT isnotequal(const minimpfp_base &value) const {return(value.V!=V);}

   minimpfp_base left() const {return(minimpfp_base(getfrc(),0));}
   minimpfp_base right() const {return(minimpfp_base(0,getmag()));}

   void nrm() {}
   void cpm() {V=~V+1;}

   BOOLINT add2(const minimpfp_base &value,minimpfp_base &result) const
      {
      unsigned int v=V;
      result.V=V+value.V;
      return(result.V<v);
      }

   BOOLINT sub2(const minimpfp_base &value,minimpfp_base &result) const
      {
      unsigned int v=V;
      result.V=V-value.V;
      return(result.V>v);
      }

   minimpfp_base mul2(const minimpfp_base &value,minimpfp_base &result) const
      {
      unsigned long long int mv;
      mv=(unsigned long long int)V*(unsigned long long int)value.V;
      result.V=(unsigned int)(mv>>16);
      return(minimpfp_base((unsigned int)(mv>>48),(unsigned int)mv&0xFFFF));
      }

   minimpfp_base inv() const
      {
      unsigned long long int iv;
      if (V<=1) return(max());
      iv=(((unsigned long long int)1)<<48)/(unsigned long long int)V;
      return(minimpfp_base((unsigned int)(iv>>32)&0xFFFF,(unsigned int)(iv>>16)&0xFFFF));
      }

   unsigned int V;
   };

typedef minimpfp<minimpfp_base> minimpfp1; // 64bit precision
typedef minimpfp<minimpfp1> minimpfp2;     // 128bit precision
typedef minimpfp<minimpfp2> minimpfp4;     // 256 bit precision

typedef minimpfp1 minimf; //!! minimpfp4

// multi-precision floating point operators:

inline minimf operator + (const minimf &a,const minimf &b)
   {
   minimf result;
   a.add(b,result);
   return(result);
   }

inline minimf operator - (const minimf &a,const minimf &b)
   {
   minimf result;
   a.sub(b,result);
   return(result);
   }

inline minimf operator - (const minimf &v)
   {return(v.neg());}

inline minimf operator * (const minimf &a,const minimf &b)
   {
   minimf result;
   a.mul(b,result);
   return(result);
   }

inline minimf operator / (const minimf &a,const minimf &b)
   {
   minimf result;
   a.div(b,result);
   return(result);
   }

inline int operator == (const minimf &a,const minimf &b)
   {return(a.isequal(b));}

inline int operator != (const minimf &a,const minimf &b)
   {return(a.isnotequal(b));}

inline int operator < (const minimf &a,const minimf &b)
   {return(!a.grt(b));}

inline int operator > (const minimf &a,const minimf &b)
   {return(a.grt(b));}

inline int operator <= (const minimf &a,const minimf &b)
   {return(a.isequal(b) || !a.grt(b));}

inline int operator >= (const minimf &a,const minimf &b)
   {return(a.isequal(b) || a.grt(b));}

inline std::ostream& operator << (std::ostream &out,const minimf &mf)
   {return(out << mf.get());}

#endif
