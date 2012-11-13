// (c) by Stefan Roettger, licensed under LGPL 2.1

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
   Object *get(unsigned int i);
   Object *get(const ministring &key);
   ministrings *get_tags(const ministring &key);
   void add_tag(ministring key,ministring tag);
   void add_tags(ministring key,ministrings tags);
   void remove_tag(ministring key,ministring tag);
   BOOLINT has_tag(ministring key,ministring tag);
   ministrings list();
   ministrings list(const ministring &tag);
   ministrings list(const ministrings &tags);
   ministrings list_wo(const ministring &tag);
   ministrings list_wo(const ministrings &tags);
   void set_repo(const ministring &repo);
   void remove(const ministring &key);
   void remove(const ministrings &keys);
   void clear();

   private:

   static unsigned int key_number;
   };

#endif
