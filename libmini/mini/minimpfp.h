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

   minimpfp minval() const {return(minimpfp(S,N::zero(),N::min()));}
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
         add2(max(),*this);
         add2(min(),*this);
         }
      }

   void cpm() {S=!S;}

   void add(const minimpfp &value,minimpfp &result) const
      {
      if (S^value.getsgn()) add2(value,result);
      else if (add2(value,result)) result=result.maxval();
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

   BOOLINT sml(const minimpfp &value) const
      {
      minimpfp result;

      if (S)
         if (value.getsgn())
            {
            sub2(value,result);

            if (!result.getsgn())
               if (result.isnotzero()) return(TRUE);
            }
         else
            if (isnotzero() || value.isnotzero()) return(FALSE);
            else return(TRUE);
      else
         if (value.getsgn())
            if (isnotzero() || value.isnotzero()) return(TRUE);
            else return(FALSE);
         else
            {
            sub2(value,result);

            if (result.getsgn())
               if (result.isnotzero()) return(TRUE);
            }

      return(FALSE);
      }

   BOOLINT grt(const minimpfp &value) const
      {
      minimpfp result;

      if (S)
         if (value.getsgn())
            {
            sub2(value,result);

            if (result.getsgn())
               if (result.isnotzero()) return(TRUE);
            }
         else
            if (isnotzero() || value.isnotzero()) return(TRUE);
            else return(FALSE);
      else
         if (value.getsgn())
            if (isnotzero() || value.isnotzero()) return(FALSE);
            else return(TRUE);
         else
            {
            sub2(value,result);

            if (!result.getsgn())
               if (result.isnotzero()) return(TRUE);
            }

      return(FALSE);
      }

   minimpfp min(const minimpfp &value) {return((sml(value))?*this:value);}
   minimpfp max(const minimpfp &value) {return((grt(value))?*this:value);}

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
      minimpfp result;
      inv2(result);
      return(result);
      }

   minimpfp inv2(minimpfp &result) const
      {
      BOOLINT sign;
      N result1,result2;
      N overflow1,overflow2;
      minimpfp result3,result4;
      minimpfp fraction,remainder;
      minimpfp overflow;

      if (iszero())
         {
         result=maxval();
         return(max());
         }
      else if (M.iszero())
         {
         overflow1=F.inv2(result1);
         result=minimpfp(S,result1,overflow1.left());
         return(minimpfp(overflow1.right(),N::zero()));
         }
      else if (F.iszero())
         {
         overflow1=M.inv2(result1);
         result=minimpfp(S,overflow1.right(),result1);
         return(minimpfp(N::zero(),overflow1.left()));
         }
      else
         {
         sign=FALSE;

         overflow1=M.inv2(result1);

         if (overflow1.right().isnotzero())
            {
            result1=N::max();
            overflow1=N::zero();
            }

         result3=minimpfp(result1,overflow1.left());

         overflow2=F.inv2(result2);

         if (overflow2.right().isnotzero())
            {
            result2=N::max();
            overflow2=N::zero();
            }

         result4=minimpfp(result2,overflow2.left());

         minimpfp(M,N::zero()).mul2(result4,fraction);
         overflow=mul2(fraction,remainder);

         if (remainder.getmag().isnotzero())
            while (overflow.getmag().iszero())
               {
               overflow1=remainder.getmag().inv2(result1);

               if (overflow1.right().isnotzero())
                  {
                  result1=N::max();
                  overflow1=N::zero();
                  }

               result4=minimpfp(result1,overflow1.left());

               if (sign) result3.add2(result4,result3);
               else result3.sub2(result4,result3);

               sign=!sign;

               if (remainder.getfrc().iszero()) break;

               overflow2=remainder.getfrc().inv2(result2);

               if (overflow2.right().isnotzero())
                  {
                  result2=N::max();
                  overflow2=N::zero();
                  }

               result4=minimpfp(result2,overflow2.left());

               minimpfp(remainder.getmag(),N::zero()).mul2(result4,fraction);
               overflow=remainder.mul2(fraction,remainder);
               }
            }

      result=minimpfp(S,N::zero(),result3.getmag());

      return(minimpfp(N::zero(),result3.getfrc()));
      }

   minimpfp sqroot() const
      {
      minimpfp r,r2,e,e2,e3;

      if (!S) return(zero());
      if (iszero()) return(zero());

      r.set(sqrt(get()));
      e=max();

      do
         {
         e2=e;
         div(r,r2);
         r2.sub(r,r2);
         r2.mul(minimpfp(0.5),e);
         r.add(e,r);
         e.add(e,e3);
         }
      while (e3.abs().sml(e2.abs()));

      return(r);
      }

   minimpfp invsqroot() const
      {
      minimpfp r,r2,e,e2,e3;

      if (!S) return(zero());
      if (iszero()) return(maxval());

      r.set(1.0/sqrt(get()));
      e=max();

      do
         {
         e2=e;
         mul(r,r2);
         r2.mul(r,r2);
         r2.mul(minimpfp(0.5),r2);
         minimpfp(1.5).sub(r2,r2);
         r2.mul(r,r2);
         r2.sub(r,e);
         r=r2;
         e.add(e,e3);
         }
      while (e3.abs().sml(e2.abs()));

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

   minimpfp_base inv2(minimpfp_base &result) const
      {
      unsigned long long int iv;
      if (V==0) {result=max(); return(max());}
      iv=(((unsigned long long int)1)<<48)/(unsigned long long int)V;
      result.V=(unsigned int)(iv>>16);
      return(minimpfp_base((unsigned int)(iv>>48),(unsigned int)iv&0xFFFF));
      }

   unsigned int V;
   };

typedef minimpfp<minimpfp_base> minimpfp1; // 64bit precision
typedef minimpfp<minimpfp1> minimpfp2;     // 128bit precision
typedef minimpfp<minimpfp2> minimpfp4;     // 256 bit precision

typedef minimpfp4 minimf;

// multi-precision floating point operators:

inline minimf operator + (const minimf &a,const minimf &b)
   {
   minimf result;
   a.add(b,result);
   return(result);
   }

inline minimf operator + (const double a,const minimf &b)
   {
   minimf result;
   minimf(a).add(b,result);
   return(result);
   }

inline minimf operator + (const minimf &a,const double b)
   {
   minimf result;
   a.add(minimf(b),result);
   return(result);
   }

inline minimf operator - (const minimf &a,const minimf &b)
   {
   minimf result;
   a.sub(b,result);
   return(result);
   }

inline minimf operator - (const double a,const minimf &b)
   {
   minimf result;
   minimf(a).sub(b,result);
   return(result);
   }

inline minimf operator - (const minimf &a,const double b)
   {
   minimf result;
   a.sub(minimf(b),result);
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

inline minimf operator * (const double a,const minimf &b)
   {
   minimf result;
   minimf(a).mul(b,result);
   return(result);
   }

inline minimf operator * (const minimf &a,const double b)
   {
   minimf result;
   a.mul(minimf(b),result);
   return(result);
   }

inline minimf operator / (const minimf &a,const minimf &b)
   {
   minimf result;
   a.div(b,result);
   return(result);
   }

inline minimf operator / (const double a,const minimf &b)
   {
   minimf result;
   minimf(a).div(b,result);
   return(result);
   }

inline minimf operator / (const minimf &a,const double b)
   {
   minimf result;
   a.div(minimf(b),result);
   return(result);
   }

inline int operator == (const minimf &a,const minimf &b)
   {return(a.isequal(b));}

inline int operator == (const minimf &a,const double b)
   {return(a.isequal(minimf(b)));}

inline int operator != (const minimf &a,const minimf &b)
   {return(a.isnotequal(b));}

inline int operator != (const minimf &a,const double b)
   {return(a.isnotequal(minimf(b)));}

inline int operator < (const minimf &a,const minimf &b)
   {return(a.sml(b));}

inline int operator < (const minimf &a,const double b)
   {return(a.sml(minimf(b)));}

inline int operator > (const minimf &a,const minimf &b)
   {return(a.grt(b));}

inline int operator > (const minimf &a,const double b)
   {return(a.grt(minimf(b)));}

inline int operator <= (const minimf &a,const minimf &b)
   {return(!a.grt(b));}

inline int operator <= (const minimf &a,const double b)
   {return(!a.grt(minimf(b)));}

inline int operator >= (const minimf &a,const minimf &b)
   {return(!a.sml(b));}

inline int operator >= (const minimf &a,const double b)
   {return(!a.sml(minimf(b)));}

inline std::ostream& operator << (std::ostream &out,const minimf &mf)
   {return(out << mf.get());}

#endif
