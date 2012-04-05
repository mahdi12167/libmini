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
   minikeyval_pair(const ministring &k,const Item &v,const minidyna<ministring> &t) {key=k; val=v; tags=t;}

   // key-value pair
   ministring key;
   Item val;

   // optional pair tags
   minidyna<ministring> tags;
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

   //! add key-value pair
   void add(const ministring &key,const Item &val)
      {
      pairs.append(minikeyval_pair<Item>(key,val));
      sorted=FALSE;
      }

   //! add key-value pair with tags
   void add(const ministring &key,const Item &val,const minidyna<ministring> &tags)
      {
      pairs.append(minikeyval_pair<Item>(key,val,tags));
      sorted=FALSE;
      }

   //! get value reference from key
   Item *get(const ministring &key)
      {return(&get_pair(key)->val);}

   //! get tag reference from key
   minidyna<ministring> &get_tags(const ministring &key)
      {return(get_pair(key)->tags);}

   //! get list of tagged items
   minidyna<Item *> get_tagged_items(const ministring &tag)
      {
      minidyna<Item *> list;

      for (unsigned int i=0; i<pairs.getsize(); i++)
         for (unsigned int j=0; j<pairs[i].tags.getsize(); j++)
            if (pairs[i].tags[j]==tag)
               list.append(&pairs[i].val);

      return(list);
      }

   protected:

   //! get pair reference from key
   minikeyval_pair<Item> *get_pair(const ministring &key)
      {
      unsigned int size;
      unsigned int left,right,mid;

      if (!sorted)
         {
         shellsort< minikeyval_pair<Item> >(pairs);
         sorted=TRUE;
         }

      size=pairs.getsize();

      if (key<pairs[0].key || pairs[size-1].key<key) return(NULL);

      // binary search
      left=mid=0;
      right=size-1;
      while (left!=right)
         {
         mid=(left+right)/2;
         if (key<pairs[mid].key) right=mid;
         else left=mid;
         }

      return(&pairs[mid]);
      }

   };

#endif
