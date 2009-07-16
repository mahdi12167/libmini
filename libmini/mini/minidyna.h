// (c) by Stefan Roettger

#ifndef MINIDYNA_H
#define MINIDYNA_H

#include <iostream>

#include "minibase.h"

//! templated dynamic array
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
   minidyna(const Item &v,unsigned int size)
      {
      SIZE=MAXSIZE=0;
      MINSIZE=(Minsize==0)?1:Minsize;
      ARRAY=NULL;

      setsize(size,v);
      }

   //! copy constructor
   minidyna(const minidyna<Item,Minsize> &a)
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
            if (a==NULL) MEMERROR();

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

   //! set array size and copy from array
   void setarray(unsigned int size,const Item *c)
      {
      unsigned int i;

      setsize(size);

      for (i=0; i<SIZE; i++) set(i,c[i]);
      }

   //! grow array size by one element
   void growsize()
      {setsize(SIZE+1);}

   //! shrink array size by one element
   void shrinksize()
      {if (SIZE>0) setsize(SIZE-1);}

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

   //! grow array size with expansion of existing part
   void expandsize(unsigned int size)
      {if (SIZE>0) growsize(size,get(SIZE-1));}

   //! clear!
   void clear() {setsize(0);}

   //! empty?
   BOOLINT empty() const {return(SIZE==0);}

   //! check for existing item
   BOOLINT has(const Item &v) const
      {
      unsigned int i;

      for (i=0; i<SIZE; i++)
         if (get(i)==v) return(TRUE);

      return(FALSE);
      }

   //! check for existing item and return first occurring index
   BOOLINT has(const Item &v,unsigned int &idx) const
      {
      for (idx=0; idx<SIZE; idx++)
         if (get(idx)==v) return(TRUE);

      return(FALSE);
      }

   //! check for existing item in reverse order and return first occurring index
   BOOLINT hasr(const Item &v,unsigned int &idx) const
      {
      unsigned int i;

      for (i=0; i<SIZE; i++)
         {
         idx=SIZE-1-i;
         if (get(idx)==v) return(TRUE);
         }

      return(FALSE);
      }

   //! set to single item
   void set(const Item &v)
      {
      setsize(1);
      MINARRAY[0]=v;
      }

   //! set item
   void set(const unsigned int idx,const Item &v)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) MINARRAY[idx]=v;
      else ARRAY[idx-MINSIZE]=v;
      }

   //! get item
   const Item &get(const unsigned int idx) const
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! get reference to item
   Item &ref(const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! get first item
   const Item &first() const
      {
      ERRORCHK(SIZE==0);

      return(MINARRAY[0]);
      }

   //! get last item
   const Item &last() const
      {
      ERRORCHK(SIZE==0);

      if (SIZE<=MINSIZE) return(MINARRAY[SIZE-1]);
      else return(ARRAY[SIZE-1-MINSIZE]);
      }

   //! append item to array
   void append(const Item &v)
      {
      setsize(SIZE+1);
      set(SIZE-1,v);
      }

   //! append item array
   void append(const minidyna<Item,Minsize> &a)
      {
      unsigned int i;

      setsize(SIZE+a.getsize());

      for (i=0; i<a.getsize(); i++) set(SIZE-i-1,a.get(a.getsize()-i-1));
      }

   //! prepend item by shifting all items
   void prepend(const Item &v)
      {
      unsigned int i;

      growsize();
      for (i=SIZE-1; i>0; i--) ref(i)=get(i-1);
      ref(0)=v;
      }

   //! remove item by moving the last item
   void remove(const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      ref(idx)=get(SIZE-1);
      setsize(SIZE-1);
      }

   //! replace all occurrences of an item
   unsigned int replace(const Item &v1,const Item &v2)
      {
      unsigned int i,c;

      c=0;

      for (i=0; i<SIZE; i++)
         if (get(i)==v1)
            {
            set(i,v2);
            c++;
            }

      return(c);
      }

   //! copy item array
   void copy(const minidyna<Item,Minsize> &a)
      {
      unsigned int i;

      setsize(a.getsize());

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=a.get(i);
         else ARRAY[i-MINSIZE]=a.get(i);
      }

   //! assignment operator
   minidyna<Item,Minsize>& operator = (const minidyna<Item,Minsize> &a)
      {
      copy(a);
      return(*this);
      }

   //! subscript operator for non-const objects returns modifiable lvalue
   Item& operator [] (const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! subscript operator for const objects returns rvalue
   const Item& operator [] (const unsigned int idx) const
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! add operator
   minidyna<Item,Minsize>& operator += (const minidyna<Item,Minsize> &a)
      {
      unsigned int i;

      unsigned int size;

      size=a.getsize();

      setsize((SIZE<size)?SIZE:size);

      for (i=0; i<SIZE; i++) ref(i)+=a[i];

      return(*this);
      }

   //! sub operator
   minidyna<Item,Minsize>& operator -= (const minidyna<Item,Minsize> &a)
      {
      unsigned int i;

      unsigned int size;

      size=a.getsize();

      setsize((SIZE<size)?SIZE:size);

      for (i=0; i<SIZE; i++) ref(i)-=a[i];

      return(*this);
      }

   //! mul operator
   minidyna<Item,Minsize>& operator *= (double c)
      {
      unsigned int i;

      for (i=0; i<SIZE; i++) ref(i)*=c;

      return(*this);
      }

   //! div operator
   minidyna<Item,Minsize>& operator /= (double c)
      {
      unsigned int i;

      for (i=0; i<SIZE; i++) ref(i)/=c;

      return(*this);
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
      out << a[i];
      if (i+1<a.getsize()) out << ", ";
      }

   out << " ]";

   return(out);
   }

#endif
