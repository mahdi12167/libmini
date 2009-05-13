// (c) by Stefan Roettger

#ifndef MINISTRING_H
#define MINISTRING_H

#include "minibase.h"
#include "minidyna.h"

typedef minidyna<char,1<<4> ministring_base;

//! dynamic string
class ministring: public ministring_base
   {
   public:

   //! default constructor
   ministring(): ministring_base() {}

   //! copy constructor
   ministring(const ministring_base &a): ministring_base()
      {
      unsigned int i,l;

      l=a.getsize();

      setsize(l);
      for (i=0; i<l; i++) ref(i)=a[i];
      }

   //! constructor with copy from c-string
   ministring(const char *str): ministring_base()
      {
      unsigned int i,l;

      l=strlen(str);

      setsize(l);
      for (i=0; i<l; i++) ref(i)=str[i];
      }

   //! destructor
   ~ministring() {}

   //! append
   void append(const ministring_base &a)
      {ministring_base::append(a);}

   //! append from c-string
   void append(const char *str)
      {ministring_base::append(ministring(str));}

   //! copy to c-string
   char *cstr() const
      {
      unsigned int i,l;

      char *str;

      l=getsize();

      if ((str=(char *)malloc(l+1))==NULL) ERRORMSG();

      for (i=0; i<l; i++) str[i]=get(i);
      str[l]='\0';

      return(str);
      }

   };

//! cmp operator
inline int operator == (const ministring &a,const ministring &b)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   if (b.getsize()!=size) return(0);

   for (i=0; i<size; i++)
      if (a[i]!=b[i]) return(0);

   return(1);
   }

//! neq operator
inline int operator != (const ministring &a,const ministring &b)
   {
   unsigned int i;

   unsigned int size;

   size=a.getsize();

   if (b.getsize()!=size) return(1);

   for (i=0; i<size; i++)
      if (a[i]!=b[i]) return(1);

   return(0);
   }

//! add operator (concatenate)
inline ministring operator + (const ministring &a,const ministring &b)
   {
   ministring str;

   str=a;
   str.append(b);

   return(str);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const ministring &a)
   {
   unsigned int i;

   out << "\"";

   for (i=0; i<a.getsize(); i++) out << a[i];

   out << "\"";

   return(out);
   }

#endif
