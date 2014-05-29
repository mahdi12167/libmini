// (c) by Stefan Roettger, licensed under GPL 2+

#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <mini/mini_generic.h>

#include "object.h"
#include "objects.h"

//! object container
class Objects: public minikeyval<Object *>
   {
   public:

   //! default constructor
   Objects();

   //! default destructor
   ~Objects();

   static ministring newkey();
   int add(const ministring &key,Object *obj,const ministring &tag);
   int add(const ministring &key,Object *obj,const ministrings &tags);
   unsigned int get_num() const;
   Object *idx(unsigned int i);
   Object *get(const ministring &key);
   ministrings *get_tags(const ministring &key);
   void add_tag(const ministring &key,const ministring &tag);
   void add_tags(const ministring &key,const ministrings &tags);
   void remove_tag(const ministring &key,const ministring &tag);
   BOOLINT has_tag(const ministring &key,const ministring &tag);
   ministrings list();
   ministrings list(const ministring &tag);
   ministrings list(const ministrings &tags);
   ministrings list_wo(const ministring &tag);
   ministrings list_wo(const ministrings &tags);
   void set_repo(const ministring &repo);
   void show(const ministring &key,BOOLINT yes=TRUE);
   void focus(const ministring &key);
   void mark(const ministring &key,BOOLINT yes=TRUE);
   void move(const ministring &key,const minicoord &pos0,const minicoord &pos1);
   void rotate(const ministring &key,const minicoord &pos0,const minicoord &pos1);
   void scale_ds(const ministring &key,const minicoord &pos0,const minicoord &pos1);
   void scale_dt(const ministring &key,const minicoord &pos0,const minicoord &pos1);
   void remove(const ministring &key);
   void remove(const ministrings &keys);
   void clear();

   private:

   static unsigned int key_number;
   };

#endif
