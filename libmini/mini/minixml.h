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
      XML_EQUALS,
      XML_TAG,
      XML_QTAG
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

   minikeyval<int> count_;

   void parse_tag();
   void parse_pair();

   void pushname(ministring name);
   ministring getname();

   virtual void question(ministring name);
   virtual void tag(ministring name);
   virtual void pair(ministring name,ministring value);
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

   virtual void question(ministring name);
   virtual void tag(ministring name);
   virtual void pair(ministring name,ministring value);
   };

#endif
