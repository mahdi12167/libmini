// (c) by Stefan Roettger

#ifndef MINIDYNA_H
#define MINIDYNA_H

#include <iostream>

#include "minibase.h"

template <class Item,const unsigned int Minsize=0>
class minidyna
   {
   protected:

   unsigned int SIZE,MINSIZE,MAXSIZE;
   Item MINARRAY[(Minsize==0)?1:Minsize];
   Item *ARRAY;

   public:

   //! default constructor
   minidyna()
      {
      SIZE=MAXSIZE=0;
      MINSIZE=(Minsize==0)?1:Minsize;
      ARRAY=NULL;
      }

   //! constructor
   minidyna(const Item &v)
      {
      unsigned int i;

      MINSIZE=(Minsize==0)?1:Minsize;
      SIZE=MAXSIZE=MINSIZE;
      ARRAY=NULL;

      for (i=0; i<MINSIZE; i++) MINARRAY[i]=v;
      }

   //! constructor
   minidyna(const Item *v)
      {
      unsigned int i;

      MINSIZE=(Minsize==0)?1:Minsize;
      SIZE=MAXSIZE=MINSIZE;
      ARRAY=NULL;

      for (i=0; i<MINSIZE; i++) MINARRAY[i]=v[i];
      }

   //! copy constructor
   minidyna(const minidyna &a)
      {
      unsigned int i;

      SIZE=MAXSIZE=0;
      MINSIZE=(Minsize==0)?1:Minsize;
      ARRAY=NULL;

      setsize(a.getsize());

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=a.get(i);
         else ARRAY[i-MINSIZE]=a.get(i);
      }

   //! destructor
   ~minidyna() {setsize(0);}

   //! get array size
   unsigned int getsize() const {return(SIZE);}

   //! set array size
   void setsize(unsigned int size)
      {
      unsigned int i;

      unsigned int s;
      Item *a;

      if (size<=MINSIZE)
         {
         if (ARRAY!=NULL) delete[] ARRAY;

         SIZE=MAXSIZE=size;
         ARRAY=NULL;
         }
      else
         {
         for (s=1; s<size; s*=2);

         if (s!=MAXSIZE)
            {
            a=new Item[s-MINSIZE];

            if (ARRAY!=NULL)
               {
               if (size<SIZE)
                  for (i=MINSIZE; i<size; i++) a[i-MINSIZE]=ARRAY[i-MINSIZE];
               else
                  for (i=MINSIZE; i<SIZE; i++) a[i-MINSIZE]=ARRAY[i-MINSIZE];

               delete[] ARRAY;
               }

            MAXSIZE=s;
            ARRAY=a;
            }

         SIZE=size;
         }
      }

   //! set array size with initialization
   void setsize(unsigned int size,const Item &c)
      {
      unsigned int i;

      setsize(size);

      for (i=0; i<SIZE; i++) set(i,c);
      }

   //! grow array size
   void growsize(unsigned int size)
      {if (size>SIZE) setsize(size);}

   //! grow array size with initialization of grown part
   void growsize(unsigned int size,const Item &c)
      {
      unsigned int i;

      unsigned int s;

      s=SIZE;
      growsize(size);

      for (i=s; i<SIZE; i++) set(i,c);
      }

   //! set null!
   void setnull() {setsize(0);}

   //! is null?
   BOOLINT isnull() const {return(SIZE==0);}

   //! set single value
   void set(const Item &v)
      {
      setsize(1);
      MINARRAY[0]=v;
      }

   //! set single value
   void set(const unsigned int idx,const Item &v)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) MINARRAY[idx]=v;
      else ARRAY[idx-MINSIZE]=v;
      }

   //! get single value
   const Item &get(const unsigned int idx=0) const
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! get reference to single value
   Item &ref(const unsigned int idx=0)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! append item to array
   void append(const Item &v)
      {
      setsize(SIZE+1);
      set(SIZE-1,v);
      }

   //! append array
   void append(const minidyna &a)
      {
      unsigned int i;

      setsize(SIZE+a.getsize());

      for (i=0; i<a.getsize(); i++) set(SIZE-i-1,a.get(a.getsize()-i-1));
      }

   //! remove item
   void remove(const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      ref(idx)=get(SIZE-1);
      setsize(SIZE-1);
      }

   //! assignment operator
   minidyna &operator = (const minidyna &a)
      {
      unsigned int i;

      setsize(a.getsize());

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=a.get(i);
         else ARRAY[i-MINSIZE]=a.get(i);

      return(*this);
      }

   //! subscript operator for non-const objects returns modifiable lvalue
   Item &operator [] (const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! subscript operator for const objects returns rvalue
   const Item &operator [] (const unsigned int idx) const
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   };

//! cmp operator
template <class Item,const unsigned int Minsize>
inline int operator == (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   if (b.getsize()!=size) return(0);

   for (i=0; i<size; i++)
      if (a[i]!=b[i]) return(0);

   return(1);
   }

//! neq operator
template <class Item,const unsigned int Minsize>
inline int operator != (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   if (b.getsize()!=size) return(1);

   for (i=0; i<size; i++)
      if (a[i]!=b[i]) return(1);

   return(0);
   }

//! add operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator + (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int sizea,sizeb,size;

   sizea=a.getsize();
   sizeb=b.getsize();

   size=(sizea<sizeb)?sizea:sizeb;

   array.setsize(size);

   for (i=0; i<size; i++) array[i]=a[i]+b[i];

   return(array);
   }

//! sub operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator - (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int sizea,sizeb,size;

   sizea=a.getsize();
   sizeb=b.getsize();

   size=(sizea<sizeb)?sizea:sizeb;

   array.setsize(size);

   for (i=0; i<size; i++) array[i]=a[i]-b[i];

   return(array);
   }

//! neg operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator - (const minidyna<Item,Minsize> &v)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int size;

   size=v.getsize();
   array.setsize(size);

   for (i=0; i<size; i++) array[i]=-v[i];

   return(array);
   }

//! mul operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator * (const double a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int size;

   size=b.getsize();
   array.setsize(size);

   for (i=0; i<size; i++) array[i]=a*b[i];

   return(array);
   }

//! mul operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator * (const minidyna<Item,Minsize> &a,const double b)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int size;

   size=a.getsize();
   array.setsize(size);

   for (i=0; i<size; i++) array[i]=a[i]*b;

   return(array);
   }

//! div operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator / (const minidyna<Item,Minsize> &a,const double b)
   {
   unsigned int i;

   minidyna<Item,Minsize> array;
   unsigned int size;

   size=a.getsize();
   array.setsize(size);

   for (i=0; i<size; i++) array[i]=a[i]/b;

   return(array);
   }

//! stream output
template <class Item,const unsigned int Minsize>
inline std::ostream& operator << (std::ostream &out,const minidyna<Item,Minsize> &a)
   {
   unsigned int i;

   out << "minidyna[ ";

   for (i=0; i<a.getsize(); i++)
      {
      out << a.get(i);
      if (i<a.getsize()-1) out << ", ";
      }

   out << " ]";

   return(out);
   }

#endif
