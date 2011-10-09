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

   virtual unsigned int get_id()
      {return(m_id);}

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

   virtual minidyna< miniref<mininode> > serialize(unsigned int id=0)
      {
      minidyna< miniref<mininode> > list;

      if (get_id()==id) list.append(this);

      for (unsigned int i=0; i<getsize(); i++)
         {
         mininode *child=get(i);
         list.append(child->serialize(id));
         }

      return(list);
      }

   protected:

   unsigned int m_id;

   virtual void traverse_pre() {}
   virtual void traverse_action() {}
   virtual void traverse_post() {}
   };

typedef miniref<mininode> mininoderef;
typedef minidyna< miniref<mininode> > mininodelist;

#endif
