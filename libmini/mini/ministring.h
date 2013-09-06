// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISTRING_H
#define MINISTRING_H

#include "minibase.h"
#include "minidyna.h"
#include "minisort.h"

#include "miniio.h"

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
   ministring(const ministring_base &a): ministring_base(a)
      {cstr=NULL;}

   //! copy constructor
   ministring(const ministring &a): ministring_base(a)
      {cstr=NULL;}

   //! constructor with copy from c-string
   ministring(const char *str): ministring_base()
      {
      unsigned int i,l;

      cstr=NULL;

      if (str!=NULL) l=strlen(str);
      else l=0;

      setsize(l);
      for (i=0; i<l; i++) set(i,str[i]);
      }

   //! constructor with initialization from char value
   ministring(char c,unsigned int size): ministring_base(c,size)
      {cstr=NULL;}

   //! constructor with initialization from floating-point value
   ministring(double v): ministring_base()
      {
      static const int len=32;
      static char str[len];

      if (isNAN(v)) strcpy(str,"NAN");
      else if (dabs(v-dtrc(v+0.5))<1E-7) snprintf(str,len,"%g",v);
      else snprintf(str,len,"%.7g",v);

      cstr=NULL;

      *this=ministring(str);
      }

   //! destructor
   ~ministring()
      {if (cstr!=NULL) free(cstr);}

   //! string length
   unsigned int length() const
      {return(getsize());}

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

   //! append double value
   void append_float(float v)
      {
      static const int len=32;
      static char str[len];

      if (v==0.0f) append("0");
      else if (v==1.0f) append("1");
      else if (v==-1.0f) append("-1");
      else
         {
         snprintf(str,len,"%.7e",v);
         append(str);
         }
      }

   //! append double value
   void append_double(double v)
      {
      static const int len=32;
      static char str[len];

      if (v==0.0) append("0");
      else if (v==1.0) append("1");
      else if (v==-1.0) append("-1");
      else
         {
         snprintf(str,len,"%.16e",v);
         append(str);
         }
      }

   //! append integer value
   void append_int(int v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%d",v);

      append(str);
      }

   //! append unsigned integer value
   void append_uint(unsigned int v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%u",v);

      append(str);
      }

   //! append uchar array
   void append_array(const unsigned char *array,unsigned int n)
      {
      unsigned int i;

      for (i=0; i<n; i++) append((char)array[i]);
      }

   //! append float array
   void append_array(const float *array,unsigned int n)
      {
      unsigned int i;

      union
         {
         unsigned char uchar[4];
         float float32;
         } float32;

      union
         {
         unsigned char uchar[2];
         unsigned short int int16;
         } int16;

      int16.int16=1;

      if (int16.uchar[0]!=0)
         for (i=0; i<n; i++)
            {
            float32.float32=array[i];

            append((char)float32.uchar[3]);
            append((char)float32.uchar[2]);
            append((char)float32.uchar[1]);
            append((char)float32.uchar[0]);
            }
      else
         for (i=0; i<n; i++)
            {
            float32.float32=array[i];

            append((char)float32.uchar[0]);
            append((char)float32.uchar[1]);
            append((char)float32.uchar[2]);
            append((char)float32.uchar[3]);
            }
      }

   //! check for existing sub-string and return first occurring index
   BOOLINT find(const ministring_base &sub,unsigned int &idx,
                unsigned int start=0) const
      {
      unsigned int i;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      for (idx=start; idx+s<=SIZE; idx++)
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
   BOOLINT find(const char *sub,unsigned int &idx,unsigned int start=0) const
      {return(find(ministring(sub),idx,start));}

   //! check for existing sub-string in reverse order and return first occurring index
   BOOLINT findr(const ministring_base &sub,unsigned int &idx,
                 unsigned int start=0) const
      {
      unsigned int i,j;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      if (start<s-1) start=s-1;

      for (i=start; i<SIZE; i++)
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
   BOOLINT findr(const char *sub,unsigned int &idx,unsigned int start=0) const
      {return(findr(ministring(sub),idx,start));}

   //! check for existing sub-string
   BOOLINT contains(const ministring_base &sub) const
      {
      unsigned int idx;
      return(find(sub,idx));
      }

   //! check for existing sub-c-string
   BOOLINT contains(const char *sub) const
      {
      unsigned int idx;
      return(find(ministring(sub),idx));
      }

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

   //! check for existing sub-string in reverse order and return remaining suffix
   ministring suffix(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!findr(sub,idx)) return(*this);

      for (i=idx+sub.getsize(); i<SIZE; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string in reverse order and return remaining suffix
   ministring suffix(const char *sub) const
      {return(suffix(ministring(sub)));}

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

   //! check for existing sub-string and return remaining prefix
   ministring prefix(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!find(sub,idx)) return(*this);

      for (i=0; i<idx; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string and return remaining prefix
   ministring prefix(const char *sub) const
      {return(prefix(ministring(sub)));}

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
   ministring& substitute(const ministring_base &sub,const ministring_base &with)
      {
      unsigned int i,j;
      unsigned int l,s,w;

      BOOLINT found;

      l=getsize();

      s=sub.getsize();
      w=with.getsize();

      if (s==0) return(*this);

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

      return(*this);
      }

   //! substitute c-string
   void substitute(const ministring_base &sub,const char *with)
      {substitute(sub,ministring(with));}

   //! substitute c-strings
   void substitute(const char *sub,const char *with)
      {substitute(ministring(sub),ministring(with));}

   //! copy to c-string
   const char *c_str()
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

   //! conversion to int value
   int value_int()
      {
      int v;

      if (sscanf(c_str(),"%i",&v)!=1) v=0;

      return(v);
      }

   //! conversion to unsigned int value
   unsigned int value_uint()
      {
      unsigned int v;

      if (sscanf(c_str(),"%u",&v)!=1) v=0;

      return(v);
      }

   //! extract uchar array
   void extract_array(unsigned char *array,unsigned int n) const
      {
      unsigned int i;

      for (i=0; i<n; i++) array[i]=(unsigned char)get(i);
      }

   //! extract float array
   void extract_array(float *array,unsigned int n) const
      {
      unsigned int i;

      union
         {
         unsigned char uchar[4];
         float float32;
         } float32;

      union
         {
         unsigned char uchar[2];
         unsigned short int int16;
         } int16;

      int16.int16=1;

      if (int16.uchar[0]!=0)
         for (i=0; i<n; i++)
            {
            float32.uchar[3]=get(4*i);
            float32.uchar[2]=get(4*i+1);
            float32.uchar[1]=get(4*i+2);
            float32.uchar[0]=get(4*i+3);

            array[i]=float32.float32;
            }
      else
         for (i=0; i<n; i++)
            {
            float32.uchar[0]=get(4*i);
            float32.uchar[1]=get(4*i+1);
            float32.uchar[2]=get(4*i+2);
            float32.uchar[3]=get(4*i+3);

            array[i]=float32.float32;
            }
      }

   //! unique checksum
   ministring checksum(unsigned int length=4) const
      {
      unsigned int i,j,k;

      static char symbols[64]={'-','a','b','c','d','e','f','g',
                               'h','i','j','k','l','m','n','o',
                               'p','q','r','s','t','u','v','w',
                               'x','y','z','A','B','C','D','E',
                               'F','G','H','I','J','K','L','M',
                               'N','O','P','Q','R','S','T','U',
                               'V','W','X','Y','Z','0','1','2',
                               '3','4','5','6','7','8','9','+'};

      unsigned int init=1;
      unsigned int checksum;

      unsigned int n=getsize();
      unsigned int chunk=(n+length-1)/length;

      ministring check;

      for (i=k=0; i<length; i++)
         {
         checksum=init;
         for (j=0; j<chunk; j++)
            {
            checksum=271*checksum;
            if (k<n) checksum+=get(k++);
            checksum^=checksum>>13;
            }
         init++;

         check.append(symbols[checksum&0x3f]);
         }

      return(check);
      }

   //! append uchar as hex
   void append_uchar_hex(unsigned char v)
      {
      static char symbols[16]={'0','1','2','3','4','5','6','7',
                               '8','9','A','B','C','D','E','F'};

      append(symbols[(v>>4)]);
      append(symbols[v&15]);
      }

   //! append uint as hex
   void append_uint_hex(unsigned int v)
      {
      append_uchar_hex((unsigned char)(v>>24));
      append_uchar_hex((unsigned char)(v>>16));
      append_uchar_hex((unsigned char)(v>>8));
      append_uchar_hex((unsigned char)v);
      }

   //! append random string
   void append_random(unsigned int length=4)
      {
      unsigned int i;

      static char symbols[64]={'-','a','b','c','d','e','f','g',
                               'h','i','j','k','l','m','n','o',
                               'p','q','r','s','t','u','v','w',
                               'x','y','z','A','B','C','D','E',
                               'F','G','H','I','J','K','L','M',
                               'N','O','P','Q','R','S','T','U',
                               'V','W','X','Y','Z','0','1','2',
                               '3','4','5','6','7','8','9','+'};

      int symbol;

      for (i=0; i<length; i++)
         {
         symbol=dtrc(sizeof(symbols)*minirand());
         symbol%=sizeof(symbols);

         append(symbols[symbol]);
         }
      }

   //! concatenate string (serialization)
   ministring to_string() const
      {return(*this);}

   //! deconcatenate string (deserialization)
   void from_string(const ministring &str)
      {*this=str;}

   //! assignment operator
   ministring& operator = (const ministring &a)
      {
      ministring_base::copy(a);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   //! add operator (concatenate strings)
   ministring& operator += (const ministring &a)
      {
      append(a);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   //! add operator (concatenate c-string)
   ministring& operator += (const char *c)
      {
      append(c);

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

   for (i=0; i<s; i++)
      {
      if (a[i]<b[i]) return(1);
      if (b[i]<a[i]) return(0);
      }

   return(0);
   }

//! add operator (concatenate strings)
inline ministring operator + (const ministring &a,const ministring &b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate c-string)
inline ministring operator + (const ministring &a,const char *b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate c-string)
inline ministring operator + (const char *a,const ministring &b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate floating-point value)
inline ministring operator + (const ministring &a,double b)
   {
   ministring str(a);

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
      else if (isprint(c)) out << c;
      }

   out << "\"";

   return(out);
   }

//! string list
class ministrings: public minidyna<ministring>
   {
   public:

   //! default constructor
   ministrings()
      : minidyna<ministring>()
      {}

   //! copy constructor
   ministrings(const minidyna<ministring> &strs)
      : minidyna<ministring>(strs)
      {}

   //! custom constructor
   ministrings(const ministring &str)
      : minidyna<ministring>(str)
      {}

   //! concatenate string list (serialization)
   ministring to_string(ministring separator="/") const
      {
      static const ministring escape("`");
      static const ministring escape0("`0");
      static const ministring escape1("`1");

      unsigned int i;

      ministring str;
      ministring s;

      if (separator==escape) ERRORMSG();

      for (i=0; i<getsize(); i++)
         {
         s=get(i);

         s.substitute(escape,escape0);
         s.substitute(separator,escape1);

         str += s;
         if (i<getsize()-1) str += separator;
         }

      return(str);
      }

   //! deconcatenate string list (deserialization)
   void from_string(const ministring &str,ministring separator="/")
      {
      static const ministring escape("`");
      static const ministring escape0("`0");
      static const ministring escape1("`1");

      unsigned int left,right;
      ministring s;

      if (separator==escape) ERRORMSG();

      left=0;

      while (str.find(separator,right,left))
         {
         s=str.range(left,right-1);

         s.substitute(escape1,separator);
         s.substitute(escape0,escape);

         append(s);

         left=right+separator.length();
         }

      if (left<str.length())
         {
         s=str.range(left,str.length()-1);

         s.substitute(escape1,separator);
         s.substitute(escape0,escape);

         append(s);
         }
      }

   //! serialize string list
   ministring serialize() const
      {return(to_string("\n"));}

   //! deserialize string list
   void deserialize(const ministring &str)
      {from_string(str,"\n");}

   //! save to file
   void save(ministring filename) const
      {writeministring(filename.c_str(),serialize());}

   //! load from file
   void load(ministring filename)
      {deserialize(readministring(filename.c_str()));}

   //! unique checksum
   ministring checksum(unsigned int length=4) const
      {return(serialize().checksum(length));}

   //! check for contained sub-string
   BOOLINT contains(const ministring &sub) const
      {
      unsigned int i;

      for (i=0; i<getsize(); i++)
         if (get(i).contains(sub)) return(TRUE);

      return(FALSE);
      }

   //! search for strings containing a sub-string
   ministrings search(const ministring &sub) const
      {
      unsigned int i;

      ministrings strs;

      for (i=0; i<getsize(); i++)
         if (get(i).contains(sub)) strs.append(get(i));

      return(strs);
      }

   //! add operator (string concatenation)
   ministrings& operator += (const ministring &a)
      {
      append(a);
      return(*this);
      }

   //! add operator (string list concatenation)
   ministrings& operator += (const ministrings &a)
      {
      append(a);
      return(*this);
      }

   //! sub operator (string removal)
   ministrings& operator -= (const ministring &a)
      {
      unsigned int i;

      for (i=getsize(); i>0; i--)
         if (get(i-1)==a) remove(i-1);

      return(*this);
      }

   //! sub operator (string list removal)
   ministrings& operator -= (const ministrings &a)
      {
      unsigned int i,j;

      for (j=0; j<a.getsize(); j++)
         for (i=getsize(); i>0; i--)
            if (get(i-1)==a[j]) remove(i-1);

      return(*this);
      }

   };

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

   for (i=0; i<s; i++)
      {
      if (s1[i]<s2[i]) return(1);
      if (s2[i]<s1[i]) return(0);
      }

   return(0);
   }

//! add operator (string concatenation)
inline ministrings operator + (const ministrings &a,const ministring &b)
   {
   ministrings strs(a);

   strs.append(b);

   return(strs);
   }

//! add operator (string list concatenation)
inline ministrings operator + (const ministrings &a,const ministrings &b)
   {
   ministrings strs(a);

   strs.append(b);

   return(strs);
   }

//! sub operator (string removal)
inline ministrings operator - (const ministrings &a,const ministring &b)
   {
   unsigned int i;

   ministrings strs(a);

   for (i=strs.getsize(); i>0; i--)
      if (strs[i-1]==b) strs.remove(i-1);

   return(strs);
   }

//! sub operator (string list removal)
inline ministrings operator - (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   ministrings strs(a);

   for (j=0; j<b.getsize(); j++)
      for (i=strs.getsize(); i>0; i--)
         if (strs[i-1]==b[j]) strs.remove(i-1);

   return(strs);
   }

//! div operator (string inclusion)
inline int operator / (const ministring &a,const ministrings &b)
   {
   unsigned int i;

   for (i=0; i<b.getsize(); i++)
      if (a==b[i]) return(1);

   return(0);
   }

//! div operator (string list inclusion)
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

#endif
