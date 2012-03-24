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
   minikeyval_pair() {key=""; val=Item();}
   minikeyval_pair(const ministring &k,const Item &v) {key=k; val=v;}
   ministring key;
   Item val;
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

   //! get value reference from key
   Item *get(const ministring &key)
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

      return(&pairs[mid].val);
      }
   };

#endif
