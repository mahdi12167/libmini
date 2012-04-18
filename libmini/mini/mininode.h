// (c) by Stefan Roettger, licensed under LGPL 2.1

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

   //! set unique node id
   virtual void set_id(unsigned int id=0)
      {m_id=id;}

   //! get unique node id
   virtual unsigned int get_id() const
      {return(m_id);}

   //! get number of all linked nodes
   unsigned int get_links() const
      {return(getsize());}

   //! get number of visible children
   virtual unsigned int get_children() const
      {return(getsize());}

   //! get child number i
   virtual mininode *get_child(unsigned int i=0) const
      {return(get(i));}

   //! get reference to child number i
   miniref<mininode> get_childref(unsigned int i=0)
      {return(ref(i));}

   //! append child node
   miniref<mininode> append_child(const miniref<mininode> &v)
      {
      set_dirty();
      return(append(v));
      }

   //! remove child number i
   miniref<mininode> remove_child(unsigned int i=0)
      {
      miniref<mininode> child=remove(i);
      set_dirty();

      for (unsigned int i=0; i<child->get_children(); i++)
         append(child->get_childref(i));

      return(child);
      }

   //! remove first depth-first occurance of node
   miniref<mininode> remove_node(mininode *node)
      {
      miniref<mininode> ref=NULL;

      for (unsigned int i=0; i<get_children() && ref==NULL; i++)
         {
         mininode *child=get_child(i);

         if (child!=NULL)
            if (child==node) ref=remove_child(i);
            else ref=child->remove_node(node);
         }

      return(ref);
      }

   //! traverse [cycle-free] graph
   virtual BOOLINT traverse(unsigned int level=0)
      {
      BOOLINT dirty=FALSE;

      if (level==0) traverse_init();

      traverse_pre();

      unsigned int s=get_children();

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL)
            if (child->traverse(level+1)) dirty=TRUE;

         if (i+1<s) traverse_past();
         }

      traverse_post();

      if (level==0) traverse_exit();

      if (dirty) set_dirty();

      return(is_dirty());
      }

   //! traverse graph and serialize nodes with specific id
   virtual minidyna<mininode*> serialize(unsigned int id=0)
      {
      minidyna<mininode*> list;

      if (get_id()==id) list.append(this);

      unsigned int s=get_children();

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL) list.append(child->serialize(id));
         }

      return(list);
      }

   //! get first node with specific id
   virtual mininode *get_first(unsigned int id=0)
      {
      if (get_id()==id) return(this);

      unsigned int s=get_children();

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

   //! get last node with specific id
   virtual mininode *get_last(unsigned int id=0)
      {
      if (get_id()==id) return(this);

      unsigned int s=get_children();

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

   //! flag as dirty
   void set_dirty()
      {m_dirty=TRUE;}

   //! check dirty flag
   BOOLINT is_dirty() const
      {return(m_dirty);}

   // clear dirty flag via recursive update
   void clear_dirty()
      {
      unsigned int s=get_children();

      for (unsigned int i=0; i<s; i++)
         {
         mininode *child=get_child(i);
         if (child!=NULL)
            if (child->is_dirty()) child->clear_dirty();
         }

      if (is_dirty())
         {
         update_dirty();
         m_dirty=FALSE;
         }
      }

   protected:

   unsigned int m_id;

   virtual void traverse_init() = 0;
   virtual void traverse_pre() = 0;
   virtual void traverse_past() = 0;
   virtual void traverse_post() = 0;
   virtual void traverse_exit() = 0;

   virtual void update_dirty() = 0;

   private:

   BOOLINT m_dirty;
   };

typedef miniref<mininode> mininode_ref;

#endif
