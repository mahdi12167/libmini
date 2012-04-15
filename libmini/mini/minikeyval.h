// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIKEYVAL_H
#define MINIKEYVAL_H

#include <iostream>

#include "minibase.h"
#include "ministring.h"
#include "minisort.h"

//! templated key-value pair
template <class Item>
class minikeyval_pair
   {
   public:

   // constructors
   minikeyval_pair() {key=""; val=Item();}
   minikeyval_pair(const ministring &k,const Item &v) {key=k; val=v;}
   minikeyval_pair(const ministring &k,const Item &v,const ministrings &t) {key=k; val=v; tags=t;}

   // key-value pair
   ministring key;
   Item val;

   // optional pair tags
   ministrings tags;
   };

//! templated key-value pair comparison
template <class Item>
inline int operator < (const minikeyval_pair<Item> &a,const minikeyval_pair<Item> &b)
   {return(a.key<b.key);}

//! templated key-value pairs
template <class Item>
class minikeyval
   {
   protected:

   minidyna< minikeyval_pair<Item> > pairs;
   BOOLINT sorted;

   public:

   //! default constructor
   minikeyval() {sorted=TRUE;}

   //! destructor
   ~minikeyval() {}

   //! get number of pairs
   unsigned int get_pairs() const
      {return(pairs.getsize());}

   //! add key-value pair
   void add(const ministring &key,const Item &val)
      {
      pairs.append(minikeyval_pair<Item>(key,val));
      sorted=FALSE;
      }

   //! add key-value pair with tag
   void add(const ministring &key,const Item &val,const ministring &tag)
      {
      pairs.append(minikeyval_pair<Item>(key,val,ministrings(tag)));
      sorted=FALSE;
      }

   //! add key-value pair with tags
   void add(const ministring &key,const Item &val,const ministrings &tags)
      {
      pairs.append(minikeyval_pair<Item>(key,val,tags));
      sorted=FALSE;
      }

   //! remove key-value pair
   void remove(const ministring &key)
      {
      unsigned int idx;

      if (get_pair(key,idx))
         pairs.dispose(idx);
      }

   //! clear key-value pairs
   void clear()
      {pairs.clear();}

   //! get value reference from index
   Item *get(unsigned int i)
      {return(&pairs[i].val);}

   //! get value reference from key
   Item *get(const ministring &key)
      {
      unsigned int idx;

      if (!get_pair(key,idx)) return(NULL);

      return(&pairs[idx].val);
      }

   //! get tag reference from key
   ministrings *get_tags(const ministring &key)
      {
      unsigned int idx;

      if (!get_pair(key,idx)) return(NULL);

      return(&pairs[idx].tags);
      }

   //! get all item keys
   ministrings get_items()
      {
      ministrings keys;

      for (unsigned int i=0; i<pairs.getsize(); i++)
         keys.append(pairs[i].key);

      return(keys);
      }

   //! get keys of tagged items
   ministrings get_tagged_items(const ministring &tag)
      {
      ministrings keys;

      for (unsigned int i=0; i<pairs.getsize(); i++)
         for (unsigned int j=0; j<pairs[i].tags.getsize(); j++)
            if (pairs[i].tags[j]==tag)
               {
               keys.append(pairs[i].key);
               break;
               }

      return(keys);
      }

   //! get keys of tagged items
   ministrings get_tagged_items(const ministrings &tags)
      {
      ministrings keys;

      for (unsigned int i=0; i<pairs.getsize(); i++)
         if (tags/pairs[i].tags)
            keys.append(pairs[i].key);

      return(keys);
      }

   protected:

   //! get pair index from key
   BOOLINT get_pair(const ministring &key,unsigned int &idx)
      {
      unsigned int size;
      unsigned int left,right,mid;

      if (!sorted)
         {
         shellsort< minikeyval_pair<Item> >(pairs);
         sorted=TRUE;
         }

      size=pairs.getsize();

      if (key<pairs[0].key || pairs[size-1].key<key) return(FALSE);

      // binary search
      left=mid=0;
      right=size-1;
      while (left!=right)
         {
         mid=(left+right)/2;
         if (key<pairs[mid].key) right=mid;
         else left=mid;
         }

      idx=mid;

      return(TRUE);
      }

   };

#endif
