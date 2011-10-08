// (c) by Stefan Roettger

#ifndef MININODE_H
#define MININODE_H

#include <iostream>

#include "minibase.h"

#include "minidyna.h"
#include "miniref.h"

//! graph node
class mininode: public minidyna< miniref<mininode> >
   {
   public:

   //! default constructor
   mininode()
      {}

   //! destructor
   virtual ~mininode()
      {}

   virtual void traverse()
      {
      traverse_action();

      for (unsigned int i=0; i<getsize(); i++)
         {
         mininode *child=get(i);
         child->traverse_pre();
         child->traverse();
         child->traverse_post();
         }
      }

   protected:

   virtual void traverse_pre() {}
   virtual void traverse_action() {}
   virtual void traverse_post() {}
   };

typedef miniref<mininode> mininoderef;

#endif
