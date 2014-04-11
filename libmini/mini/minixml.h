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

   //! save xml to file
   void save(ministring filename);

   //! serialization
   ministrings to_strings();

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
   virtual void question(ministring name);
   virtual void tag(ministring name);
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

   protected:

   minikeyval<ministring> xml_;

   minidyna<ministring> keys_;
   minidyna<ministring> vals_;

   virtual void begin();
   virtual void question(ministring name);
   virtual void tag(ministring name);
   virtual void pair(ministring name,ministring value);
   virtual void finish();
   };

#endif
