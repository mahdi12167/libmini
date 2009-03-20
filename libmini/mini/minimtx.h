// (c) by Stefan Roettger

#ifndef MINIMTX_H
#define MINIMTX_H

#include <iostream>

#include "minidyna.h"

//! templated dynamic matrix
template <class Item,const unsigned int Minsize=0>
class minimtx: public minidyna<Item>
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

   void set(const Item &val=0) {minidyna<Item>::set(val);}

   void set(unsigned int x,unsigned int y,const Item &val)
      {
      if (x>=COLS || y>=ROWS) WARNMSG();
      minidyna<Item>::set(x+y*COLS,val);
      }

   Item get(unsigned int x,unsigned int y) const
      {
      if (x>=COLS || y>=ROWS) WARNMSG();
      return(minidyna<Item>::get(x+y*COLS));
      }

   void diag(const Item &val=1)
      {
      unsigned int i;

      if (getcols()<getrows())
         for (i=0; i<getcols(); i++) set(i,i,val);
      else 
         for (i=0; i<getrows(); i++) set(i,i,val);
     }

   //! Gaussian elimination with back-substitution
   // constant vector is right-most column
   // assumes non-zero entries on the diagonal
   minimtx<Item> solve()
      {
      unsigned int i,j,k,l;

      Item factor,sum;

      minimtx<Item,Minsize> sol(1,getrows());

      // check dimensions
      if (getcols()!=getrows()+1) return(sol);

      // check diagonal elements to be non-zero
      for (i=0; i<getrows(); i++)
         if (get(i,i)==0) return(sol);

      // compute upper triangular form
      for (j=getrows()-1; j>0; j--)
         for (i=0; i<j; i++)
            if (get(i,j)!=0)
               {
               for (k=j-1; k>0; k--)
                  if (get(i,k)!=0) break;

               factor=get(i,j)/get(i,k);

               for (l=i+1; l<getcols(); l++) set(l,j,get(l,j)-factor*get(i,k));

               set(i,j,0);
               }

      // back-substitution
      for (i=getrows()-1; i+1>0; i--)
         {
         sum=get(getrows(),i);

         for (j=i+1; j<getrows(); j++) sum-=get(j,i)*sol.get(0,j);

         sol.set(0,i,sum/get(i,i));
         }

      return(sol);
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

//! stream output
template <class Item,const unsigned int Minsize>
inline std::ostream& operator << (std::ostream &out,const minimtx<Item,Minsize> &a)
   {
   unsigned int i,j;

   out << "minimtx[ ";

   if (a.getcols()>1)
      for (j=0; j<a.getrows(); j++)
         {
         out << "[ ";

         for (i=0; i<a.getcols(); i++)
            {
            out << a.get(i,j);
            if (i+1<a.getcols()) out << ",";
            }

         out << " ]";
         }
   else
      {
      out << "[ ";

      for (j=0; j<a.getrows(); j++)
         {
         out << a.get(0,j);
         if (j+1<a.getrows()) out << ",";
         }

      out << " ]T";
      }

   out << " ]";

   return(out);
   }

#endif
