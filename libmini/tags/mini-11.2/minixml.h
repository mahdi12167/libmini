// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIXML_H
#define MINIXML_H

#include "minidyna.h"
#include "minikeyval.h"
#include "lunaparse.h"

class minixmlparser
   {
   public:

   enum
      {
      // xml tokens
      XML_BRACKET_LEFT=lunaparse::LUNA_UNUSED_TOKENS,
      XML_BRACKET_RIGHT,
      XML_SLASH,
      XML_QUESTION,
      XML_XMARK,
      XML_BRACKET_BRACKET,
      XML_COLON,
      XML_EQUAL
      };

   //! default constructor
   minixmlparser() {}

   //! destructor
   virtual ~minixmlparser() {}

   //! load xml from file
   BOOLINT load(ministring filename);

   //! deserialization
   void from_strings(ministrings &infos);

   protected:

   lunaparse *parser_;
   minidyna<ministring> tags_;
   minidyna<unsigned int> count_;
   BOOLINT popped_;

   void parse_tag();
   ministring parse_name();
   void parse_pair();

   void clear();
   void pushname(ministring name);
   BOOLINT checkname();
   ministring peekname();
   void popname();

   ministring getname();

   virtual void begin();
   virtual void pair(ministring name,ministring value);
   virtual void finish();
   };

class minixml : public minixmlparser
   {
   public:

   //! default constructor
   minixml()
      : minixmlparser()
      {}

   //! destructor
   virtual ~minixml() {}

   //! get xml value from key
   ministring get(ministring key)
      {
      ministring *val;

      val=xml_.get(key);
      if (val!=NULL) return(*val);

      return("");
      }

   //! get number of xml pairs from index
   unsigned int get_pairs()
      {return(xml_.get_pairs());}

   //! get xml pair from index
   const minikeyval_pair<ministring> *get(unsigned int i)
      {return(xml_.pair(i));}

   protected:

   minikeyval<ministring> xml_;
   minidyna< minikeyval_pair<ministring> > list_;

   virtual void begin();
   virtual void pair(ministring name,ministring value);
   virtual void finish();
   };

#endif
