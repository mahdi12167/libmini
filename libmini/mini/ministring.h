// (c) by Stefan Roettger

#ifndef MINISTRING_H
#define MINISTRING_H

#include "minibase.h"
#include "minidyna.h"

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
      for (i=0; i<l; i++) ref(i)=str[i];
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

      if (s==0) return(FALSE);

      for (idx=0; idx+s<=SIZE; idx++)
         {
         found=TRUE;

         for (i=0; i<s; i++)
            if (get(idx+i)!=a[i])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

   //! check for existing sub-string in reverse order and return first occurring index
   BOOLINT findr(const ministring_base &sub,unsigned int &idx) const
      {
      unsigned int i,j;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      if (s==0) return(FALSE);

      for (i=0; i<SIZE; i++)
         {
         idx=SIZE-1-i;

         found=TRUE;

         for (j=0; j<s; j++)
            if (get(idx+j)!=a[j])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

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
               for (j=i+s; j<l; j++) ref(j-s+w)=get(j);

            setsize(l-s+w);

            if (s<w)
               for (j=l-1; j>=i+s; j--) ref(j-s+w)=get(j);

            for (j=0; j<w; j++) ref(i+j)=with[j];

            l=getsize();
            i+=w;
            }
         else i++;
         }
      }

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

#endif
