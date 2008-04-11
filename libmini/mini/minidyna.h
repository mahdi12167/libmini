// (c) by Stefan Roettger

#ifndef MINIDYNA_H
#define MINIDYNA_H

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
   ~minidyna()
      {if (ARRAY!=NULL) delete[] ARRAY;}

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

            if (size<SIZE)
               for (i=MINSIZE; i<size; i++) a[i-MINSIZE]=ARRAY[i-MINSIZE];
            else
               for (i=MINSIZE; i<SIZE; i++) a[i-MINSIZE]=ARRAY[i-MINSIZE];

            delete[] ARRAY;
            ARRAY=a;

            MAXSIZE=s;
            }

         SIZE=size;
         }
      }

   //! set null!
   void setnull() {setsize(0);}

   //! is null?
   BOOLINT isnull() {return(SIZE==0);}

   //! set single value
   void set(const Item &v)
      {
      setsize(1);
      MINARRAY[0]=v;
      }

   //! set single value
   void set(const unsigned int idx,const Item &v)
      {
      if (idx>=SIZE) ERRORMSG();

      if (idx<MINSIZE) MINARRAY[idx]=v;
      else ARRAY[idx-MINSIZE]=v;
      }

   //! get single value
   Item get(const unsigned int idx=0) const
      {
      if (idx>=SIZE) ERRORMSG();

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! get a reference to single value
   Item &ref(const unsigned int idx=0)
      {
      if (idx>=SIZE) ERRORMSG();

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

   //! subscript operator for non-const objects returns modifiable lvalue
   Item &operator[](unsigned int idx)
      {
      if (idx>=SIZE) ERRORMSG();

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   //! subscript operator for const objects returns rvalue
   Item operator[](const unsigned int idx) const
      {
      if (idx>=SIZE) ERRORMSG();

      if (idx<MINSIZE) return(MINARRAY[idx]);
      else return(ARRAY[idx-MINSIZE]);
      }

   };

#endif
