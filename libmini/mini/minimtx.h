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
      {setdim(cols,rows,val);}

   //! destructor
   ~minimtx() {}

   //! get dimensions
   unsigned int getcols() const {return(COLS);}
   unsigned int getrows() const {return(ROWS);}

   //! set dimensions
   void setdim(unsigned int cols,unsigned int rows,const Scalar &val=0)
      {
      COLS=cols;
      ROWS=rows;

      minidyna<Scalar,Minsize*Minsize>::setsize(cols*rows,0);

      if (val!=0) diag(val);
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

   //! set diagonal
   void diag(const Scalar &val=0)
      {
      unsigned int i,j;

      for (i=0; i<COLS; i++)
         for (j=0; j<ROWS; j++) set(i,j,(i==j)?val:0);
      }

   //! augment matrix with right hand side
   void augment(const minimtx<Scalar,Minsize> &rhs)
      {
      unsigned int i,j;

      ERRORCHK(getrows()!=rhs.getrows());

      minimtx<Scalar,Minsize> mtx(getcols()+rhs.getcols(),getrows());

      for (i=0; i<getcols(); i++)
         for (j=0; j<getrows(); j++) mtx.set(i,j,get(i,j));

      for (i=0; i<rhs.getcols(); i++)
         for (j=0; j<rhs.getrows(); j++) mtx.set(i+getcols(),j,rhs.get(i,j));

      *this=mtx;
      }

   //! solves linear system of equations defined by square matrix and right hand side vector
   BOOLINT solve(const minimtx<Scalar,Minsize> &rhs,minimtx<Scalar,Minsize> &sol) const
      {
      minimtx<Scalar,Minsize> mtx(*this);

      mtx.augment(rhs);
      return(mtx.solve(sol));
      }

   //! invert square matrix
   BOOLINT invert(minimtx<Scalar,Minsize> &inv) const
      {
      minimtx<Scalar,Minsize> mtx(*this);
      minimtx<Scalar,Minsize> one(getcols(),getrows(),1);

      mtx.augment(one);
      return(mtx.gauss(inv));
      }

   //! transpose matrix
   void transpose(minimtx<Scalar,Minsize> &tra) const
      {
      unsigned int i,j;

      tra.setdim(getrows(),getcols());

      for (i=0; i<getcols(); i++)
         for (j=0; j<getrows(); j++) tra.set(j,i,get(i,j));
      }

   //! multiple regression
   //! correlates sample vectors x_i = (x_i1, x_i2, ..., x_ip) for i=1..n
   //! that are stored in the matrix X = (x_i) for i=1..n
   //! with linearly dependent samples y_i for i=1..n
   //! so that y_i = b_0 + b_1*x_i1 + b_2*x_i2 + ... + b_p*x_ip + e_i
   //! with the sum of e_i squared being minimal for the corresponding solution vector b
   //! the solution vector b is computed as follows:
   //!  Y = X*b + e
   //!  with Y = (y_1, y_2, ..., y_i)T
   //!  and X = (1, x_11, x_12, ..., x_1p
   //!           1, x_21, x_22, ..., x_2p
   //!           ...
   //!           1, x_n1, x_n2, ..., x_np)
   //!  then b = ( (X)T * X )^-1 * ( (X)T * Y )
   //! returns true if a solution vector exists
   BOOLINT mulreg(const minimtx<Scalar,Minsize> &y,
                  minimtx<Scalar,Minsize> &b) const
      {
      minimtx<Scalar,Minsize> X,XT;
      minimtx<Scalar,Minsize> XTX,XTY;

      b.setdim(0,getcols()+1);

      // check dimensions
      if (getrows()!=y.getrows() || y.getcols()!=1) return(FALSE);

      // augment sample vectors
      X.setdim(1,getrows());
      X.clear(1);
      X.augment(*this);

      // compute solution vector
      X.transpose(XT);
      XTX=XT*X;
      XTY=XT*y;
      if (!XTX.invert(XTX)) return(FALSE);
      b=XTX*XTY;

      return(TRUE);
      }

   protected:

   unsigned int COLS,ROWS;

   //! Gaussian elimination with back-substitution
   //! solves linear system of equations defined by square matrix
   //! working matrix has to be in the augmented form (N+1)xN
   //! right-hand-side vector is right-most column of working matrix
   //! returns true if the linear system has a solution
   BOOLINT solve(minimtx<Scalar,Minsize> &sol)
      {
      unsigned int i,j,k;

      Scalar item,factor,sum;

      // check dimensions
      if (getrows()<1) return(FALSE);
      if (getcols()!=getrows()+1) return(FALSE);

      // set dimensions of solution vector
      sol.setdim(1,getrows());

      // reorder rows to have non-zero elements on the diagonal
      for (i=0; i<getrows()-1; i++)
         if (get(i,i)==0)
            for (j=i+1; j<getrows(); j++)
               if (get(i,j)!=0)
                  for (k=0; k<getcols(); k++)
                     {
                     item=get(k,i);
                     set(k,i,get(k,j));
                     set(k,j,item);
                     }

      // compute upper triangular form
      for (i=0; i<getrows()-1; i++)
         if (get(i,i)!=0)
            for (j=getrows()-1; j>i; j--)
               if (get(i,j)!=0)
                  {
                  factor=get(i,j)/get(i,i);
                  for (k=i+1; k<getcols(); k++) set(k,j,get(k,j)-factor*get(k,i));
                  set(i,j,0);
                  }

      // back-substitution
      for (i=0; i<getrows(); i++)
         {
         k=getrows()-1-i;

         sum=get(getrows(),k);
         for (j=k+1; j<getrows(); j++) sum-=get(j,k)*sol.get(0,j);

         if (get(k,k)!=0) sol.set(0,k,sum/get(k,k));
         else if (sum==0) sol.set(0,k,0);
         else return(FALSE);
         }

      return(TRUE);
      }

   //! Gaussian elimination
   //! inverts linear system of equations defined by square matrix
   //! working matrix has to be in the augmented form (2*N)xN
   //! returns true if the linear system is invertable
   BOOLINT gauss(minimtx<Scalar,Minsize> &inv)
      {
      unsigned int i,j,k,l;

      Scalar item,factor;

      // check dimensions
      if (getrows()<1) return(FALSE);
      if (getcols()!=2*getrows()) return(FALSE);

      // set dimensions of inverse matrix
      inv.setdim(getrows(),getrows());

      // reorder rows to have non-zero elements on the diagonal
      for (i=0; i<getrows()-1; i++)
         if (get(i,i)==0)
            for (j=i+1; j<getrows(); j++)
               if (get(i,j)!=0)
                  for (k=0; k<getcols(); k++)
                     {
                     item=get(k,i);
                     set(k,i,get(k,j));
                     set(k,j,item);
                     }

      // compute upper triangular form
      for (i=0; i<getrows()-1; i++)
         {
         if (get(i,i)==0) return(FALSE);

         for (j=getrows()-1; j>i; j--)
            if (get(i,j)!=0)
               {
               factor=get(i,j)/get(i,i);
               for (k=i+1; k<getcols(); k++) set(k,j,get(k,j)-factor*get(k,i));
               set(i,j,0);
               }
         }

      // compute diagonal form
      for (i=0; i<getrows(); i++)
         {
         k=getrows()-1-i;

         if (get(k,k)==0) return(FALSE);

         for (j=0; j<k; j++)
            if (get(k,j)!=0)
               {
               factor=get(k,j)/get(k,k);
               for (l=i+1; l<getcols(); l++) set(l,j,get(l,j)-factor*get(l,k));
               set(k,j,0);
               }
         }

      // copy inverse matrix
      for (i=0; i<getrows(); i++)
         for (j=0; j<getrows(); j++) inv.set(i,j,get(i+getrows(),j)/get(j,j));

      return(TRUE);
      }
   };

//! add operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator + (const minimtx<Scalar,Minsize> &a,const minimtx<Scalar,Minsize> &b)
   {
   unsigned int i,j;

   ERRORCHK(a.getcols()!=b.getcols() || a.getrows()!=b.getrows());

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

   ERRORCHK(a.getcols()!=b.getcols() || a.getrows()!=b.getrows());

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

   minimtx<Scalar,Minsize> mtx(v.getcols(),v.getrows());

   for (i=0; i<v.getcols(); i++)
      for (j=0; j<v.getrows(); j++) mtx.set(i,j,-v.get(i,j));

   return(mtx);
   }

//! mul operator
template <class Scalar,const unsigned int Minsize>
inline minimtx<Scalar,Minsize> operator * (const minimtx<Scalar,Minsize> &a,const minimtx<Scalar,Minsize> &b)
   {
   unsigned int i,j,k;

   Scalar val;

   ERRORCHK(a.getcols()!=b.getrows());

   minimtx<Scalar,Minsize> mtx(b.getcols(),a.getrows());

   for (i=0; i<b.getcols(); i++)
      for (j=0; j<a.getrows(); j++)
         {
         val=0;
         for (k=0; k<a.getcols(); k++) val+=a.get(k,j)*b.get(i,k);
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
