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
   minikeyval_pair() : key(""), val() {}
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
   unsigned int idxnum;
   BOOLINT sorted;

   public:

   //! default constructor
   minikeyval()
      {
      idxnum=0;
      sorted=TRUE;
      }

   //! custom constructor
   minikeyval(const minidyna<Item> &vals)
      {
      idxnum=0;
      for (unsigned int i=0; i<vals.getsize(); i++) add_at(i,vals[i]);
      sorted=TRUE;
      }

   // cast operator
   operator minidyna<Item>()
      {return(get_values());}

   //! destructor
   ~minikeyval() {}

   //! get number of pairs
   unsigned int get_pairs() const
      {return(pairs.getsize());}

   //! get number of indexable items
   unsigned int get_num() const
      {return(idxnum);}

   //! get key at index
   ministring get_key(unsigned int idx)
      {
      ministring key;

      key=".0x";
      key.append_uint_hex(idx);

      return(key);
      }

   //! add key-value pair
   BOOLINT add(const ministring &key,const Item &val)
      {
      unsigned int idx=0;

      if (get_pair(key,idx)) return(FALSE);

      pairs.append(minikeyval_pair<Item>(key,val));
      sorted=FALSE;

      return(TRUE);
      }

   //! add key-value pair with tag
   BOOLINT add(const ministring &key,const Item &val,const ministring &tag)
      {
      unsigned int idx=0;

      if (get_pair(key,idx)) return(FALSE);

      pairs.append(minikeyval_pair<Item>(key,val,ministrings(tag)));
      sorted=FALSE;

      return(TRUE);
      }

   //! add key-value pair with tags
   BOOLINT add(const ministring &key,const Item &val,const ministrings &tags)
      {
      unsigned int idx=0;

      if (get_pair(key,idx)) return(FALSE);

      pairs.append(minikeyval_pair<Item>(key,val,tags));
      sorted=FALSE;

      return(TRUE);
      }

   //! add indexable value
   BOOLINT add_at(unsigned int idx,const Item &val)
      {
      BOOLINT ok;

      ok=add(get_key(idx),val);
      if (ok) idxnum++;

      return(ok);
      }

   //! remove key-value pair
   void remove(const ministring &key)
      {
      unsigned int idx=0;

      if (get_pair(key,idx))
         pairs.dispose(idx);
      }

   //! remove key-value pairs
   void remove(const ministrings &keys)
      {
      unsigned int i;

      for (i=0; i<keys.getsize(); i++)
         remove(keys[i]);
      }

   //! clear key-value pairs
   void clear()
      {pairs.clear();}

   //! get value reference from index
   Item *idx(unsigned int i)
      {
      if (i<pairs.getsize()) return(&pairs[i].val);
      return(NULL);
      }

   //! get value reference from key
   Item *get(const ministring &key)
      {
      unsigned int idx=0;

      if (get_pair(key,idx)) return(&pairs[idx].val);

      return(NULL);
      }

   //! get tag reference from key
   ministrings *get_tags(const ministring &key)
      {
      unsigned int idx=0;

      if (get_pair(key,idx)) return(&pairs[idx].tags);

      return(NULL);
      }

   //! add tag to key-value pair
   void tag(const ministring &key,const ministring &tag)
      {
      unsigned int idx=0;

      if (!get_pair(key,idx)) return;

      pairs[idx].tags += tag;
      }

   //! add tags to key-value pair
   void tag(const ministring &key,const ministrings &tags)
      {
      unsigned int idx=0;

      if (!get_pair(key,idx)) return;

      pairs[idx].tags += tags;
      }

   //! remove tag from key-value pair
   void untag(const ministring &key,const ministring &tag)
      {
      unsigned int idx=0;

      if (!get_pair(key,idx)) return;

      pairs[idx].tags -= tag;
      }

   //! key-value pair has tag?
   BOOLINT has_tag(const ministring &key,const ministring &tag)
      {
      unsigned int idx=0;

      if (!get_pair(key,idx)) return(FALSE);

      return(pairs[idx].tags.has(tag));
      }

   //! get all item keys
   ministrings get_items()
      {
      ministrings keys;

      sort();

      for (unsigned int i=0; i<pairs.getsize(); i++)
         keys.append(pairs[i].key);

      return(keys);
      }

   //! get keys of tagged items
   ministrings get_items(const ministring &tag)
      {
      ministrings keys;

      sort();

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
   ministrings get_items(const ministrings &tags)
      {
      ministrings keys;

      sort();

      for (unsigned int i=0; i<pairs.getsize(); i++)
         if (tags/pairs[i].tags)
            keys.append(pairs[i].key);

      return(keys);
      }

   //! get keys of items /wo tag
   ministrings get_items_wo(const ministring &tag)
      {
      ministrings keys;

      sort();

      for (unsigned int i=0; i<pairs.getsize(); i++)
         if (!(tag/pairs[i].tags))
            keys.append(pairs[i].key);

      return(keys);
      }

   //! get keys of items /wo tags
   ministrings get_items_wo(const ministrings &tags)
      {
      ministrings keys;

      BOOLINT found;

      sort();

      for (unsigned int i=0; i<pairs.getsize(); i++)
         {
         found=FALSE;

         for (unsigned int j=0; j<tags.getsize(); j++)
            if (tags[j]/pairs[i].tags)
               {
               found=TRUE;
               break;
               }

         if (!found)
            keys.append(pairs[i].key);
         }

      return(keys);
      }

   //! get all item values
   minidyna<Item> get_values()
      {
      minidyna<Item> vals;

      sort();

      for (unsigned int i=0; i<pairs.getsize(); i++)
         vals.append(pairs[i].val);

      return(vals);
      }

   //! get value at index
   Item *at(unsigned int idx)
      {return(get(get_key(idx)));}

   //! get tags at index
   ministrings *at_tags(unsigned int idx)
      {return(get_tags(get_key(idx)));}

   //! concatenate pair list (serialization)
   ministring to_string(ministring separator="/") const
      {
      unsigned int i;

      ministring str;

      sort();

      for (i=0; i<pairs.getsize(); i++)
         {
         str += "\""+pairs[i].key+"\",";
         str += "\""+pairs[i].val.to_string()+"\",";
         str += pairs[i].tags.to_string(";");

         if (i<pairs.getsize()-1) str += separator;
         }

      return(str);
      }

   //! deconcatenate pair list (deserialization)
   void from_string(const ministring &str,ministring separator="/")
      {
      unsigned int left,right;
      ministring sub;

      ministring key,val;
      ministrings tags;

      left=0;

      while (str.find(separator,right,left))
         {
         sub=str.range(left,right-1);

         sub=sub.tail("\"");
         key=sub.prefix("\",\"");
         sub=sub.tail("\",\"");
         val=sub.prefix("\",");
         sub=sub.tail("\",");
         tags.from_string(sub,";");

         add(key,val,tags);

         left=right+separator.length();
         }

      if (left<str.length())
         {
         sub=str.range(left,str.length()-1);

         sub=sub.tail("\"");
         key=sub.prefix("\",\"");
         sub=sub.tail("\",\"");
         val=sub.prefix("\",");
         sub=sub.tail("\",");
         tags.from_string(sub,";");

         add(key,val,tags);
         }
      }

   //! serialize pair list
   ministring serialize() const
      {return(to_string("\n"));}

   //! deserialize pair list
   void deserialize(const ministring &str)
      {from_string(str,"\n");}

   protected:

   //! sort keys
   void sort()
      {
      if (!sorted)
         {
         shellsort< minikeyval_pair<Item> >(pairs);
         sorted=TRUE;
         }
      }

   //! get pair index from key
   BOOLINT get_pair(const ministring &key,unsigned int &idx)
      {
      unsigned int size;
      unsigned int left,right,mid;

      sort();

      size=pairs.getsize();
      if (size==0) return(FALSE);

      // binary search
      left=mid=0;
      right=size-1;
      while (left+1<right)
         {
         mid=(left+right)/2;
         if (key<pairs[mid].key) right=mid;
         else left=mid;
         }

      // compare left
      if (key==pairs[left].key)
         {
         idx=left;
         return(TRUE);
         }

      // compare right
      if (key==pairs[right].key)
         {
         idx=right;
         return(TRUE);
         }

      return(FALSE);
      }

   };

#endif
