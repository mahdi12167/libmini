// (c) by Stefan Roettger

#ifndef MINIDYNA_H
#define MINIDYNA_H

#include "minibase.h"

template <class Item>
class minidyna
   {
   protected:

   unsigned int SIZE,MAXSIZE;
   Item *ARRAY;

   public:

   //! default constructor
   minidyna()
      {
      SIZE=MAXSIZE=0;
      ARRAY=NULL;
      }

   //! copy constructor
   minidyna(const minidyna &a)
      {
      unsigned int i;

      SIZE=MAXSIZE=0;
      ARRAY=NULL;

      setsize(a.getsize());

      for (i=0; i<SIZE; i++) ARRAY[i]=a[i];
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

      if (size==0)
         {
         if (ARRAY!=NULL) delete[] ARRAY;

         SIZE=MAXSIZE=0;
         ARRAY=NULL;
         }
      else
         {
         for (s=1; s<size; s*=2);

         if (s!=MAXSIZE)
            {
            a=new Item[s];

            if (size<SIZE)
               for (i=0; i<size; i++) a[i]=ARRAY[i];
            else
               for (i=0; i<SIZE; i++) a[i]=ARRAY[i];

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
   void setv(const Item &v) {setsize(1); ARRAY[0]=v;}

   //! get single value
   Item getv(const unsigned int idx=0) const {return(ARRAY[idx]);}

   //! append item to array
   void append(const Item &v)
      {
      setsize(getsize()+1);
      ARRAY[getsize()-1]=v;
      }

   //! append array
   void append(const minidyna &a)
      {
      unsigned int i;

      setsize(getsize()+a.getsize());

      for (i=0; i<a.getsize(); i++) ARRAY[getsize()-i-1]=a[a.getsize()-i-1];
      }

   //! subscript operator for non-const objects returns modifiable lvalue
   Item &operator[](unsigned int idx)
      {
      if (idx>=SIZE) ERRORMSG();

      return(ARRAY[idx]);
      }

   //! subscript operator for const objects returns rvalue
   Item operator[](unsigned int idx) const
      {
      if (idx>=SIZE) ERRORMSG();

      return(ARRAY[idx]);
      }

   };

#endif
