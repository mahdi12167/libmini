// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISTRING_H
#define MINISTRING_H

#include "minibase.h"
#include "minidyna.h"
#include "minisort.h"

//! string base class
typedef minidyna<char,16> ministring_base;

//! dynamic string
class ministring: public ministring_base
   {
   public:

   //! default constructor
   ministring(): ministring_base()
      {cstr=NULL;}

   //! copy constructor
   ministring(const ministring &a): ministring_base(a)
      {cstr=NULL;}

   //! constructor with copy from c-string
   ministring(const char *str): ministring_base()
      {
      unsigned int i,l;

      cstr=NULL;

      l=strlen(str);

      setsize(l);
      for (i=0; i<l; i++) set(i,str[i]);
      }

   //! constructor with initialization from floating-point value
   ministring(double v): ministring_base()
      {
      static const int len=32;
      char str[len];

      if (dabs(v-dtrc(v+0.5))<1E-7) snprintf(str,len,"%g",v);
      else snprintf(str,len,"%.7f",v);

      cstr=NULL;

      *this=ministring(str);
      }

   //! destructor
   ~ministring()
      {if (cstr!=NULL) free(cstr);}

   //! append char
   void append(const char c)
      {ministring_base::append(c);}

   //! append string
   void append(const ministring_base &a)
      {ministring_base::append(a);}

   //! append from c-string
   void append(const char *str)
      {ministring_base::append(ministring(str));}

   //! append floating-point value
   void append(double v)
      {append(ministring(v));}

   //! check for existing sub-string and return first occurring index
   BOOLINT find(const ministring_base &sub,unsigned int &idx) const
      {
      unsigned int i;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      for (idx=0; idx+s<=SIZE; idx++)
         {
         found=TRUE;

         for (i=0; i<s; i++)
            if (get(idx+i)!=sub[i])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

   //! check for existing sub-c-string and return first occurring index
   BOOLINT find(const char *sub,unsigned int &idx) const
      {return(find(ministring(sub),idx));}

   //! check for existing sub-string in reverse order and return first occurring index
   BOOLINT findr(const ministring_base &sub,unsigned int &idx) const
      {
      unsigned int i,j;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      for (i=0; i<SIZE; i++)
         {
         idx=SIZE-1-i;

         found=TRUE;

         for (j=0; j<s; j++)
            if (get(idx+j)!=sub[j])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

   //! check for existing sub-c-string in reverse order and return first occurring index
   BOOLINT findr(const char *sub,unsigned int &idx) const
      {return(findr(ministring(sub),idx));}

   //! check for existing sub-string and return remaining tail
   ministring tail(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!find(sub,idx)) return(*this);

      for (i=idx+sub.getsize(); i<SIZE; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string and return remaining tail
   ministring tail(const char *sub) const
      {return(tail(ministring(sub)));}

   //! check for existing sub-string in reverse order and return remaining head
   ministring head(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!findr(sub,idx)) return(*this);

      for (i=0; i<idx; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string in reverse order and return remaining head
   ministring head(const char *sub) const
      {return(head(ministring(sub)));}

   //! check for prefix
   BOOLINT startswith(const ministring_base &prefix) const
      {
      unsigned int idx;

      if (find(prefix,idx))
         if (idx==0) return(TRUE);

      return(FALSE);
      }

   //! check for prefix
   BOOLINT startswith(const char *prefix) const
      {return(startswith(ministring(prefix)));}

   //! check for suffix
   BOOLINT endswith(const ministring_base &suffix) const
      {
      unsigned int idx;

      if (findr(suffix,idx))
         if (idx==getsize()-suffix.getsize()) return(TRUE);

      return(FALSE);
      }

   //! check for suffix
   BOOLINT endswith(const char *suffix) const
      {return(endswith(ministring(suffix)));}

   //! substitute sub-strings
   void substitute(const ministring_base &sub,const ministring_base &with)
      {
      unsigned int i,j;
      unsigned int l,s,w;

      BOOLINT found;

      l=getsize();

      s=sub.getsize();
      w=with.getsize();

      if (s==0) return;

      for (i=0; i+s<=l;)
         {
         found=TRUE;

         for (j=0; j<s; j++)
            if (get(i+j)!=sub[j])
               {
               found=FALSE;
               break;
               }

         if (found)
            {
            if (s>w)
               for (j=i+s; j<l; j++) set(j-s+w,get(j));

            setsize(l-s+w);

            if (s<w)
               for (j=l-1; j>=i+s; j--) set(j-s+w,get(j));

            for (j=0; j<w; j++) set(i+j,with[j]);

            l=getsize();
            i+=w;
            }
         else i++;
         }
      }

   //! substitute c-string
   void substitute(const ministring_base &sub,const char *with)
      {substitute(sub,ministring(with));}

   //! substitute c-strings
   void substitute(const char *sub,const char *with)
      {substitute(ministring(sub),ministring(with));}

   //! copy to c-string
   char *c_str()
      {
      unsigned int i,l;

      if (cstr!=NULL) free(cstr);

      l=getsize();

      if ((cstr=(char *)malloc(l+1))==NULL) MEMERROR();

      for (i=0; i<l; i++) cstr[i]=get(i);
      cstr[l]='\0';

      return(cstr);
      }

   //! conversion to double value
   double value()
      {
      double v;

      if (sscanf(c_str(),"%lg",&v)!=1) v=NAN;

      return(v);
      }

   //! assignment operator
   ministring& operator = (const ministring &a)
      {
      ministring_base::copy(a);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   private:

   char *cstr;
   };

//! cmp operator
inline int operator == (const ministring &a,const ministring &b)
   {return(ministring_base(a)==ministring_base(b));}

//! neq operator
inline int operator != (const ministring &a,const ministring &b)
   {return(ministring_base(a)!=ministring_base(b));}

//! less than operator (alpha-numerical comparison)
inline int operator < (const ministring &a,const ministring &b)
   {
   unsigned int i;

   unsigned int s;

   s=a.getsize();

   if (s<b.getsize()) return(1);
   if (s>b.getsize()) return(0);

   for (i=0; i+1<s; i++)
      if (b[i]<a[i]) return(0);

   return(a[i]<b[i]);
   }

//! add operator (concatenate strings)
inline ministring operator + (const ministring &a,const ministring &b)
   {
   ministring str;

   str=a;
   str.append(b);

   return(str);
   }

//! add operator (concatenate c-string)
inline ministring operator + (const ministring &a,const char *b)
   {
   ministring str;

   str=a;
   str.append(b);

   return(str);
   }

//! add operator (concatenate floating-point value)
inline ministring operator + (const ministring &a,double b)
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

   char c;

   out << "\"";

   for (i=0; i<a.getsize(); i++)
      {
      c=a[i];

      if (c=='\n' || c=='\r' || c=='\t') out << ' ';
      else out << c;
      }

   out << "\"";

   return(out);
   }

//! string list
typedef minidyna<ministring> ministrings;

//! cmp operator (compare string lists)
inline int operator == (const ministrings &a,const ministrings &b)
   {
   unsigned int i;

   if (a.getsize()!=b.getsize()) return(0);

   for (i=0; i<a.getsize(); i++)
      if (a[i]!=b[i]) return(0);

   return(1);
   }

//! less than operator (alpha-numerical comparison of string lists)
inline int operator < (const ministrings &a,const ministrings &b)
   {
   unsigned int i;

   ministrings s1(a),s2(b);
   unsigned int s;

   shellsort<ministring>(s1);
   shellsort<ministring>(s2);

   s=s1.getsize();

   if (s<s2.getsize()) return(1);
   if (s>s2.getsize()) return(0);

   for (i=0; i+1<s; i++)
      if (s2[i]<s1[i]) return(0);

   return(s1[i]<s2[i]);
   }

//! add operator (union of string lists)
//!  elements of left hand operand are assumed to be unique
inline ministrings operator + (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   ministrings strs;

   BOOLINT duplicate;

   strs=a;

   for (j=0; j<b.getsize(); j++)
      {
      duplicate=FALSE;

      for (i=0; i<strs.getsize(); i++)
         if (strs[i]==b[j])
            {
            duplicate=TRUE;
            continue;
            }

      if (!duplicate) strs.append(b[j]);
      }

   return(strs);
   }

//! sub operator (remove union of string lists)
//!  elements of left hand operand are assumed to be unique
inline ministrings operator - (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   ministrings strs;

   strs=a;

   for (j=0; j<b.getsize(); j++)
      for (i=strs.getsize(); i>0; i--)
         if (strs[i-1]==b[j]) strs.remove(i-1);

   return(strs);
   }

//! div operator (inclusion of string lists)
inline int operator / (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   BOOLINT inclusion;

   for (i=0; i<a.getsize(); i++)
      {
      inclusion=FALSE;

      for (j=0; j<b.getsize(); j++)
         if (a[i]==b[j])
            {
            inclusion=TRUE;
            break;
            }

      if (!inclusion) return(0);
      }

   return(1);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const ministrings &a)
   {
   unsigned int i;

   for (i=0; i<a.getsize(); i++)
      {
      out << a[i];
      if (i<a.getsize()-1) out << ";";
      }

   return(out);
   }

// logging:

static BOOLINT minilog_switch=FALSE;

inline void minilog_on()
   {minilog_switch=TRUE;}

inline void minilog_off()
   {minilog_switch=FALSE;}

inline void minilog(ministring msg)
   {
   static FILE *out=NULL;

   if (minilog_switch)
      {
      if (!out)
         if (!(out=fopen("log.txt","w"))) ERRORMSG();

      fprintf(out,"%s\n",msg.c_str());
      fflush(out);
      }
   }

#endif
