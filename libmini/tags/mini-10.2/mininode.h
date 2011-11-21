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
      {m_id=id; m_dirty=FALSE;}

   //! destructor
   virtual ~mininode()
      {}

   //! get unique node id
   virtual unsigned int get_id() const
      {return(m_id);}

   //! get # of children
   unsigned int get_children() const
      {return(getsize());}

   //! get child #i
   virtual mininode *get_child(unsigned int i=0) const
      {return(get(i));}

   //! get reference to child #i
   miniref<mininode> get_childref(unsigned int i=0)
      {return(ref(i));}

   //! append child node
   miniref<mininode> append_child(const miniref<mininode> &v)
      {
      set_dirty();
      return(append(v));
      }

   //! remove child #i
   miniref<mininode> remove_child(unsigned int i=0)
      {
      miniref<mininode> child=remove(i);
      set_dirty();

      for (unsigned int i=0; i<child->get_children(); i++)
         append(child->get_childref(i));

      return(child);
      }

   //! traverse [cycle-free] graph
   virtual BOOLINT traverse()
      {
      BOOLINT dirty=FALSE;

      unsigned int s=get_children();

      traverse_pre();

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL)
            if (child->traverse()) dirty=TRUE;

         if (i+1<s) traverse_past();
         }

      traverse_post();

      if (dirty) set_dirty();

      return(is_dirty());
      }

   //! traverse graph and serialize nodes with specific id
   virtual minidyna<mininode*> serialize(unsigned int id=0)
      {
      unsigned int s=get_children();

      minidyna<mininode*> list;

      if (get_id()==id) list.append(this);

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL) list.append(child->serialize(id));
         }

      return(list);
      }

   virtual mininode *get_first(unsigned int id=0)
      {
      unsigned int s=get_children();

      if (get_id()==id) return(this);

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL)
            {
            mininode *first=child->get_first(id);
            if (first!=NULL) return(first);
            }
         }

      return(NULL);
      }

   virtual mininode *get_last(unsigned int id=0)
      {
      unsigned int s=get_children();

      if (get_id()==id) return(this);

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(s-1-i);
         if (child!=NULL)
            {
            mininode *first=child->get_first(id);
            if (first!=NULL) return(first);
            }
         }

      return(NULL);
      }

   void set_dirty()
      {m_dirty=TRUE;}

   BOOLINT is_dirty()
      {return(m_dirty);}

   void clear_dirty()
      {
      for (unsigned int i=0; i<get_children(); i++)
         if (get_child(i)->is_dirty()) get_child(i)->clear_dirty();

      if (is_dirty())
         {
         update();
         m_dirty=FALSE;
         }
      }

   protected:

   unsigned int m_id;

   virtual void traverse_pre() = 0;
   virtual void traverse_past() = 0;
   virtual void traverse_post() = 0;

   virtual void update() = 0;

   private:

   BOOLINT m_dirty;
   };

typedef miniref<mininode> mininoderef;

#endif