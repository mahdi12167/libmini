// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDYNA_H
#define MINIDYNA_H

#include <iostream>

#include "minibase.h"

//! templated dynamic array (vector)
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
   minidyna(const Item &v,unsigned int size=1)
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
   unsigned int size() const {return(SIZE);}

   //! set array size
   void setsize(unsigned int size)
      {
      unsigned int i,s;

      Item *a;

      if (size<=MINSIZE)
         {
         if (ARRAY!=NULL)
            {
            delete[] ARRAY;
            ARRAY=NULL;
            }

         SIZE=MAXSIZE=size;
         }
      else
         {
         if (size<=MAXSIZE/2 || size>MAXSIZE)
            {
            for (s=2; s<size;) s*=2;

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

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=c;
         else ARRAY[i-MINSIZE]=c;
      }

   //! set array size and copy from array
   void setarray(unsigned int size,const Item *c)
      {
      unsigned int i;

      setsize(size);

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=c[i];
         else ARRAY[i-MINSIZE]=c[i];
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
      unsigned int i,s;

      s=SIZE;
      growsize(size);

      for (i=s; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=c;
         else ARRAY[i-MINSIZE]=c;
      }

   //! grow array size with expansion of existing part
   void expandsize(unsigned int size)
      {if (SIZE>0) growsize(size,get(SIZE-1));}

   //! grow array size with initialization or expansion of existing part
   void expandsize(unsigned int size,const Item &c)
      {growsize(size,(SIZE>0)?get(SIZE-1):c);}

   //! grow array size with expansion of first part
   void expandfirst(unsigned int size)
      {
      Item l=pop_back();
      if (SIZE>0) growsize(size,get(SIZE-1));
      push_back(l);
      }

   //! truncate array at index
   void truncate(unsigned int idx)
      {if (idx<SIZE) setsize(idx);}

   //! clear with value
   void clear(const Item &c)
      {
      unsigned int i;

      for (i=0; i<SIZE; i++)
         if (i<MINSIZE) MINARRAY[i]=c;
         else ARRAY[i-MINSIZE]=c;
      }

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
         if (get(idx=SIZE-1-i)==v) return(TRUE);

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
   const Item& get(const unsigned int idx) const
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! at method
   const Item& at(const unsigned int idx) const
      {return(get(idx));}

   //! get reference to item
   Item& ref(const unsigned int idx)
      {
      ERRORCHK(idx>=SIZE);

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! get first item
   const Item& first() const
      {
      ERRORCHK(SIZE==0);

      return(MINARRAY[0]);
      }

   //! get first item
   const Item& head() const
      {return(first());}

   //! get last item
   const Item& last() const
      {
      ERRORCHK(SIZE==0);

      if (SIZE<=MINSIZE) return(MINARRAY[SIZE-1]);
      else return(ARRAY[SIZE-1-MINSIZE]);
      }

   //! get last item
   const Item& tail() const
      {return(last());}

   //! set first item
   void setfirst(const Item &v)
      {
      ERRORCHK(SIZE==0);

      MINARRAY[0]=v;
      }

   //! set first item
   void sethead(const Item &v)
      {setfirst(v);}

   //! set last item
   void setlast(const Item &v)
      {
      ERRORCHK(SIZE==0);

      if (SIZE<=MINSIZE) MINARRAY[SIZE-1]=v;
      else ARRAY[SIZE-1-MINSIZE]=v;
      }

   //! set last item
   void settail(const Item &v)
      {setlast(v);}

   //! append item to array
   Item& append(const Item &v)
      {
      unsigned int s;

      s=SIZE;
      setsize(SIZE+1);

      if (s<MINSIZE) MINARRAY[s]=v;
      else ARRAY[s-MINSIZE]=v;

      if (SIZE<=MINSIZE) return(MINARRAY[SIZE-1]);
      else return(ARRAY[SIZE-1-MINSIZE]);
      }

   //! append item array
   void append(const minidyna<Item,Minsize> &a)
      {
      unsigned int i,s;

      s=a.getsize();
      setsize(SIZE+s);

      for (i=1; i<=s; i++) set(SIZE-i,a.get(s-i));
      }

   //! insert item by shifting all trailing items
   Item& insert(const Item &v,const unsigned int idx)
      {
      unsigned int i;

      setsize(SIZE+1);

      for (i=SIZE-1; i>idx; i--) set(i,get(i-1));
      set(idx,v);

      return(ref(idx));
      }

   //! prepend item to array
   Item& prepend(const Item &v)
      {return(insert(v,0));}

   //! remove indexed item by moving the last item
   Item remove(const unsigned int idx)
      {
      Item v;

      ERRORCHK(idx>=SIZE);

      v=get(idx);
      if (idx!=SIZE-1) set(idx,get(SIZE-1));
      setsize(SIZE-1);

      return(v);
      }

   //! dispose indexed item by shifting all trailing items
   Item dispose(const unsigned int idx)
      {
      unsigned int i;

      Item v;

      ERRORCHK(idx>=SIZE);

      v=get(idx);
      for (i=idx; i<SIZE-1; i++) set(i,get(i+1));
      setsize(SIZE-1);

      return(v);
      }

   //! push item on stack
   void push(const Item &v)
      {append(v);}

   //! duplicate item on stack
   void dup()
      {append(last());}

   //! peek at top of stack
   const Item &peek()
      {return(last());}

   //! pop item from stack
   Item pop()
      {return(remove(SIZE-1));}

   //! push item at front of array
   void push_front(const Item &v)
      {insert(v);}

   //! push item at back of array
   void push_back(const Item &v)
      {append(v);}

   //! pop item from front of array
   Item pop_front()
      {return(dispose());}

   //! pop item from back of array
   Item pop_back()
      {return(pop());}

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

   //! copy items from index range
   minidyna<Item,Minsize> range(unsigned int begin,unsigned int end) const
      {
      unsigned int i;

      minidyna<Item,Minsize> sub;

      if (SIZE>0)
         {
         if (end>=SIZE) end=SIZE-1;

         for (i=begin; i<=end; i++)
            sub.append(get(i));
         }

      return(sub);
      }

   //! head before index (inclusive)
   minidyna<Item,Minsize> rangeto(unsigned int index) const
      {return(range(0,index));}

   //! tail after index (inclusive)
   minidyna<Item,Minsize> rangefrom(unsigned int index) const
      {return(range(index,getsize()-1));}

   //! interpolate item array
   Item interpolate(double t) const
      {
      unsigned int i;

      if (SIZE==0) return(Item());
      if (SIZE==1) return(get(0));

      if (t<=0.0) return(get(0));
      if (t>=1.0) return(get(SIZE-1));

      t=t*(SIZE-1);
      i=floor(t);
      t=t-i;

      return((1.0-t)*get(i)+t*get(i+1));
      }

   //! interpolate item array using cubic bezier curve
   Item interpolate_cubic(double t) const
      {
      static const double c=4.0/3*(sqrt(2.0)-1.0);

      unsigned int i;

      Item v0,v1,v2,v3;

      Item h1,h2;
      Item hh1,hh2,hh3;
      Item hhh1,hhh2;
      Item hhhh;

      if (SIZE==0) return(Item());
      if (SIZE==1) return(get(0));

      if (t<=0.0) return(get(0));
      if (t>=1.0) return(get(SIZE-1));

      t=t*(SIZE-1);
      i=floor(t);
      t=t-i;

      // left continuation
      if (i>0) v0=get(i-1);
      else v0=2.0*get(i)-get(i+1);

      // evaluation domain
      v1=get(i);
      v2=get(i+1);

      // right continuation
      if (i<SIZE-2) v3=get(i+2);
      else v3=2.0*get(i+1)-get(i);

      // control points
      h1=0.5*(v2-v0)*c+v1;
      h2=0.5*(v1-v3)*c+v2;

      // de Casteljau step 1
      hh1=(1.0-t)*v1+t*h1;
      hh2=(1.0-t)*h1+t*h2;
      hh3=(1.0-t)*h2+t*v2;

      // de Casteljau step 2
      hhh1=(1.0-t)*hh1+t*hh2;
      hhh2=(1.0-t)*hh2+t*hh3;

      // de Casteljau step 3
      hhhh=(1.0-t)*hhh1+t*hhh2;

      return(hhhh);
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

   //! append operator
   minidyna<Item,Minsize>& operator += (const Item &a)
      {
      append(a);
      return(*this);
      }

   //! add operator
   minidyna<Item,Minsize>& operator += (const minidyna<Item,Minsize> &a)
      {
      unsigned int i,s;

      s=a.getsize();
      setsize((SIZE<s)?SIZE:s);

      for (i=0; i<SIZE; i++) ref(i)+=a[i];

      return(*this);
      }

   //! sub operator
   minidyna<Item,Minsize>& operator -= (const minidyna<Item,Minsize> &a)
      {
      unsigned int i,s;

      s=a.getsize();
      setsize((SIZE<s)?SIZE:s);

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
   unsigned int i,s;

   s=a.getsize();

   if (b.getsize()!=s) return(0);

   for (i=0; i<s; i++)
      if (a[i]!=b[i]) return(0);

   return(1);
   }

//! neq operator
template <class Item,const unsigned int Minsize>
inline int operator != (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i,s;

   s=a.getsize();

   if (b.getsize()!=s) return(1);

   for (i=0; i<s; i++)
      if (a[i]!=b[i]) return(1);

   return(0);
   }

//! add operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator + (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;
   unsigned int sa,sb,s;

   minidyna<Item,Minsize> array;

   sa=a.getsize();
   sb=b.getsize();

   s=(sa<sb)?sa:sb;

   array.setsize(s);

   for (i=0; i<s; i++) array[i]=a[i]+b[i];

   return(array);
   }

//! sub operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator - (const minidyna<Item,Minsize> &a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i;
   unsigned int sa,sb,s;

   minidyna<Item,Minsize> array;

   sa=a.getsize();
   sb=b.getsize();

   s=(sa<sb)?sa:sb;

   array.setsize(s);

   for (i=0; i<s; i++) array[i]=a[i]-b[i];

   return(array);
   }

//! neg operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator - (const minidyna<Item,Minsize> &v)
   {
   unsigned int i,s;

   minidyna<Item,Minsize> array;

   s=v.getsize();
   array.setsize(s);

   for (i=0; i<s; i++) array[i]=-v[i];

   return(array);
   }

//! mul operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator * (const double a,const minidyna<Item,Minsize> &b)
   {
   unsigned int i,s;

   minidyna<Item,Minsize> array;

   s=b.getsize();
   array.setsize(s);

   for (i=0; i<s; i++) array[i]=a*b[i];

   return(array);
   }

//! mul operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator * (const minidyna<Item,Minsize> &a,const double b)
   {
   unsigned int i,s;

   minidyna<Item,Minsize> array;

   s=a.getsize();
   array.setsize(s);

   for (i=0; i<s; i++) array[i]=a[i]*b;

   return(array);
   }

//! div operator
template <class Item,const unsigned int Minsize>
inline minidyna<Item,Minsize> operator / (const minidyna<Item,Minsize> &a,const double b)
   {
   unsigned int i,s;

   minidyna<Item,Minsize> array;

   s=a.getsize();
   array.setsize(s);

   for (i=0; i<s; i++) array[i]=a[i]/b;

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
