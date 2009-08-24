// (c) by Stefan Roettger

#ifndef MINIMTX_H
#define MINIMTX_H

#include <iostream>

#include "minibase.h"
#include "minidyna.h"

//! templated dynamic matrix
template <class Scalar,const unsigned int Minsize=0>
class minimtx: public minidyna<Scalar,Minsize*Minsize>
   {
   public:

   //! default constructor
   minimtx() {setdim(0,0);}

   //! constructor
   minimtx(unsigned int cols,unsigned int rows,const Scalar &val=0)
      {
      setdim(cols,rows);
      set(val);
      }

   //! destructor
   ~minimtx() {}

   //! get dimensions
   unsigned int getcols() const {return(COLS);}
   unsigned int getrows() const {return(ROWS);}

   //! set dimensions
   void setdim(unsigned int cols,unsigned int rows)
      {
      COLS=cols;
      ROWS=rows;

      minidyna<Scalar,Minsize*Minsize>::setsize(cols*rows);
      }

   //! set diagonal
   void set(const Scalar &val=0)
      {
      unsigned int i;

      // initialize with zero
      minidyna<Scalar,Minsize*Minsize>::set(0);

      // initialize diagonal
      if (val!=0)
         if (cols<rows)
            for (i=0; i<cols; i++) set(i,i,val);
         else
            for (i=0; i<rows; i++) set(i,i,val);
      }

   //! accessor/setter
   void set(unsigned int x,unsigned int y,const Scalar &val)
      {
      ERRORCHK(x>=COLS || y>=ROWS);
      minidyna<Scalar,Minsize*Minsize>::set(x+y*COLS,val);
      }

   //! accessor/getter
   Scalar get(unsigned int x,unsigned int y) const
      {
      ERRORCHK(x>=COLS || y>=ROWS);
      return(minidyna<Scalar,Minsize*Minsize>::get(x+y*COLS));
      }

   //! Gaussian elimination with back-substitution
   //! solves linear system of equations defined by square matrix
   //! working matrix has to be in the augmented form (N+1)xN
   //! right-hand-side vector is right-most column of working matrix
   //! returns true if the linear system has a solution
   BOOLINT solve(minimtx<Scalar,Minsize> &sol)
      {
      unsigned int i,j,k,l;

      Scalar item,factor,sum;

      minimtx<unsigned int,Minsize> row(1,getrows());

      // check dimensions
      if (getrows()<1) return(FALSE);
      if (getcols()!=getrows()+1) return(FALSE);

      // set dimensions of solution
      sol.setdim(1,getrows());

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
      for (i=getrows(); i>0; i--)
         {
         k=i-1;
         if (get(k,k)!=0)
            {
            sum=get(getrows(),k);
            for (j=i; j<getrows(); j++) sum-=get(j,k)*sol.get(0,row.get(0,j));
            if (get(k,k)!=0) sol.set(0,row.get(0,k),sum/get(k,k));
            else if (sum!=0) return(FALSE);
            }
         }

      return(TRUE);
      }

   protected:

   unsigned int COLS,ROWS;
   };

//! add operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator + (const minimtx<Scalar,Minsize> &a,const minimtx<Scalar,Minsize> &b)
   {
   unsigned int i,j;

   minimtx<Scalar,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,a.get(i,j)+b.get(i,j));

   return(mtx);
   }

//! sub operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator - (const minimtx<Scalar,Minsize> &a,const minimtx<Scalar,Minsize> &b)
   {
   unsigned int i,j;

   minimtx<Scalar,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,a.get(i,j)-b.get(i,j));

   return(mtx);
   }

//! neg operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator - (const minimtx<Scalar,Minsize> &v)
   {
   unsigned int i,j;

   minimtx<Scalar,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++) mtx.set(i,j,-v.get(i,j));

   return(mtx);
   }

//! mul operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator * (const minimtx<Scalar,Minsize> &a,const minimtx<Scalar,Minsize> &b)
   {
   unsigned int i,j,k;

   Scalar val;

   minimtx<Scalar,Minsize> mtx(b.getcols(),a.getrows());

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
template <class Scalar,const unsigned int Minsize>
inline std::ostream& operator << (std::ostream &out,const minimtx<Scalar,Minsize> &a)
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
