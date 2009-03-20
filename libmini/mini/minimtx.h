// (c) by Stefan Roettger

#ifndef MINIMTX_H
#define MINIMTX_H

#include "minidyna.h"

//! templated dynamic matrix
template <class Item,const unsigned int Minsize=0>
class minimtx: public minidyna<Item,Minsize*Minsize>
   {
   public:

   //! default constructor
   minimtx(unsigned int cols,unsigned int rows,const Item &val=0)
      {
      COLS=cols;
      ROWS=rows;

      minidyna<Item,Minsize*Minsize>::setsize(cols*rows,val);
      }

   //! destructor
   ~minimtx() {}

   unsigned int getcols() const {return(COLS);}
   unsigned int getrows() const {return(ROWS);}

   // accessors:

   void set(const Item &val=0) {minidyna<Item,Minsize*Minsize>::set(val);}

   void set(unsigned int x,unsigned int y,const Item &val)
      {
      if (x>=COLS || y>=ROWS) WARNMSG();
      minidyna<Item,Minsize*Minsize>::set(x+y*COLS,val);
      }

   Item get(unsigned int x,unsigned int y) const
      {
      if (x>=COLS || y>=ROWS) WARNMSG();
      return(minidyna<Item,Minsize*Minsize>::get(x+y*COLS));
      }

   protected:

   unsigned int COLS,ROWS;
   };

//! mul operator
template <class Item,const unsigned int Minsize>
inline minimtx<Item,Minsize> operator * (const minimtx<Item,Minsize> &a,const minimtx<Item,Minsize> &b)
   {
   unsigned int i,j,k;

   Item val;

   minimtx<Item,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++)
         {
         val=0;

         for (k=0; k<b.getcols(); k++) val+=a.get(k,j)*b.get(i,k);

         mtx.set(i,j,val);
         }

   return(mtx);
   }

#endif
