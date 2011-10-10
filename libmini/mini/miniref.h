// (c) by Stefan Roettger

#ifndef MINIREF_H
#define MINIREF_H

#include "minibase.h"

//! templated ref-count item
template <class Item>
class miniitem
   {
   public:

   Item *item;
   unsigned int refcount;

   miniitem(Item *i=NULL)
      {
      item=i;
      refcount=0;
      }

   ~miniitem()
      {
      if (item!=NULL)
         delete item;
      }
   };

//! templated ref-count pointer
template <class Item>
class miniref
   {
   protected:

   miniitem<Item> *ref;

   public:

   //! default constructor
   miniref(Item *i=NULL)
      {
      if (i==NULL) ref=NULL;
      else
         {
         ref=new miniitem<Item>(i);
         ref->refcount++;
         }
      }

   //! copy constructor
   miniref(const miniref<Item> &r)
      {
      ref=r.ref;
      if (ref!=NULL) ref->refcount++;
      }

   //! destructor
   ~miniref()
      {
      if (ref!=NULL)
         {
         ref->refcount--;
         if (ref->refcount==0) delete ref;
         }
      }

   //! assignment operator
   miniref<Item>& operator =(const miniref<Item> &r)
      {
      if (ref!=NULL)
         {
         ref->refcount--;
         if (ref->refcount==0) delete ref;
         }

      ref=r.ref;
      if (ref!=NULL) ref->refcount++;

      return(*this);
      }

   //! assignment operator
   miniref<Item>& operator =(Item *r)
      {
      if (ref!=NULL)
         {
         ref->refcount--;
         if (ref->refcount==0) delete ref;
         }

      if (r==NULL) ref=NULL;
      else
         {
         ref=new miniitem<Item>(r);
         ref->refcount++;
         }

      return(*this);
      }

   //! comparison operator
   bool operator ==(const miniref<Item> &r) const
      {return(ref==r.ref);}

   //! comparison operator
   bool operator ==(const Item *r) const
      {
      if (ref==NULL && r==NULL) return(true);
      if (ref==NULL) return(false);
      return(ref->item==r);
      }

   //! item accessor
   Item& operator *() const
      {
      ERRORCHK(ref==NULL);
      return(*(ref->item));
      }

   //! item member accessor
   Item* operator ->() const
      {
      ERRORCHK(ref==NULL);
      return(ref->item);
      }

   //! cast to regular pointer
   operator Item*() const
      {
      ERRORCHK(ref==NULL);
      return(ref->item);
      }
   };

#endif
