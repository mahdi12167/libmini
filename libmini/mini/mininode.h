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
   mininode(unsigned int id=0)
      {m_id=id;}

   //! destructor
   virtual ~mininode()
      {}

   //! get unique node id
   virtual unsigned int get_id()
      {return(m_id);}

   //! append as leave at leftmost edge
   virtual void append_left(mininode *node)
      {
      if (empty()) append(node);
      else (*first()).append_left(node);
      }

   //! append as leave at rightmost edge
   virtual void append_right(mininode *node)
      {
      if (empty()) append(node);
      else (*last()).append_right(node);
      }

   //! traverse [cycle-free] graph
   virtual void traverse()
      {
      unsigned int s=getsize();

      traverse_pre();

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get(i);
         child->traverse();

         if (i+1<s) traverse_past();
         }

      traverse_post();
      }

   //! traverse graph and serialize nodes with specific id
   virtual minidyna<mininode*> serialize(unsigned int id=0)
      {
      minidyna<mininode*> list;

      if (get_id()==id) list.append(this);

      for (unsigned int i=0; i<getsize(); i++)
         {
         mininode *child=get(i);
         list.append(child->serialize(id));
         }

      return(list);
      }

   virtual mininode *get_first(unsigned int id=0)
      {return(serialize(id).first());}

   virtual mininode *get_last(unsigned int id=0)
      {return(serialize(id).last());}

   protected:

   unsigned int m_id;

   virtual void traverse_pre() {}
   virtual void traverse_past() {}
   virtual void traverse_post() {}
   };

#endif
