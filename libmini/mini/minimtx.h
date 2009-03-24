// (c) by Stefan Roettger

#ifndef MINIMTX_H
#define MINIMTX_H

#include <iostream>

#include "minibase.h"
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

   void diag(const Item &val=1)
      {
      unsigned int i;

      if (getcols()<getrows())
         for (i=0; i<getcols(); i++) set(i,i,val);
      else
         for (i=0; i<getrows(); i++) set(i,i,val);
     }

   //! Gaussian elimination with back-substitution
   //! solves linear system of equations defined by square matrix
   //! working matrix has to be in the augmented form (N+1)xN
   //! right-hand-side vector is right-most column of working matrix
   //! returns true if the linear system has a solution
   BOOLINT solve(minimtx<Item,Minsize> &sol)
      {
      unsigned int i,j,k,l;

      Item item,factor,sum;

      minimtx<unsigned int,Minsize> row(1,getrows());

      // check dimensions
      if (getrows()<1) return(FALSE);
      if (getcols()!=getrows()+1) return(FALSE);

      // initialize solution vector
      for (i=0; i<getrows(); i++)
         {
         sol.set(0,i,0);
         row.set(0,i,i);
         }

      // reorder rows to have non-zero elements on the diagonal
      for (i=0; i<getrows()-1; i++)
         if (get(i,i)==0)
            for (j=i+1; j<getrows(); j++)
               if (get(i,j)!=0)
                  {
                  l=row.get(0,i);
                  row.set(0,i,row.get(0,j));
                  row.set(0,j,l);

                  for (k=0; k<getcols(); k++)
                     {
                     item=get(k,i);
                     set(k,i,get(k,j));
                     set(k,j,item);
                     }
                  }

      // compute upper triangular form 
      for (i=0; i<getrows()-1; i++)
         for (j=getrows()-1; j>i; j--)
            if (get(i,j)!=0)
               if (get(i,i)!=0)
                  {
                  factor=get(i,j)/get(i,i);
                  for (k=i+1; k<getcols(); k++) set(k,j,get(k,j)-factor*get(k,i));
                  set(i,j,0);
                  }

      // back-substitution
      for (i=getrows()-1; i+1>0; i--)
         if (get(i,i)!=0)
            {
            sum=get(getrows(),i);
            for (j=i+1; j<getrows(); j++) sum-=get(j,i)*sol.get(0,row.get(0,j));
            if (get(i,i)!=0) sol.set(0,row.get(0,i),sum/get(i,i));
            else if (sum!=0) return(FALSE);
            }

      return(TRUE);
      }

   protected:

   unsigned int COLS,ROWS;
   };

//! add operator
template <class Item,const unsigned int Minsize>
inline minimtx<Item,Minsize> operator + (const minimtx<Item,Minsize> &a,const minimtx<Item,Minsize> &b)
   {
   unsigned int i,j;

   minimtx<Item,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,a.get(i,j)+b.get(i,j));

   return(mtx);
   }

//! sub operator
template <class Item,const unsigned int Minsize>
inline minimtx<Item,Minsize> operator - (const minimtx<Item,Minsize> &a,const minimtx<Item,Minsize> &b)
   {
   unsigned int i,j;

   minimtx<Item,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,a.get(i,j)-b.get(i,j));

   return(mtx);
   }

//! neg operator
template <class Item,const unsigned int Minsize>
inline minimtx<Item,Minsize> operator - (const minimtx<Item,Minsize> &v)
   {
   unsigned int i,j;

   minimtx<Item,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,-v.get(i,j));

   return(mtx);
   }

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

typedef minimtx<double,8> minimatrix;

#endif
