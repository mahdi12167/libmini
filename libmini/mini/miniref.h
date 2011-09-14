// (c) by Stefan Roettger

#ifndef MINIREF_H
#define MINIREF_H

//! templated ref-count item
template <class Item>
class miniitem
   {
   public:

   Item item;
   unsigned int refcount;

   miniitem(const Item *i=NULL)
      {
      if (i!=NULL) item=*i;
      refcount=0;
      }

   ~miniitem() {}
   };

//! templated ref-count pointer
template <class Item>
class miniref
   {
   protected:

   miniitem<Item> *ref;

   public:

   //! default constructor
   miniref(const Item *i=NULL)
      {
      ref=new miniitem<Item>(i);
      if (ref!=NULL) ref->refcount++;
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

   //! item accessor
   Item& operator *()
      {
      if (ref==NULL) ERRORMSG();
      return(ref->item);
      }
   };

#endif
