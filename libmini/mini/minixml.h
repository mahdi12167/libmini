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
      XML_BRACKET_SLASH,
      XML_SLASH_BRACKET,
      XML_BRACKET_BRACKET,
      XML_EQUALS,
      XML_TAG
      };

   //! default constructor
   minixmlparser() {}

   //! constructor
   minixmlparser(ministring filename)
      {load(filename);}

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

   void parse_tag();

   virtual void tag();
   };

class minixml : public minixmlparser
   {
   public:

   //! default constructor
   minixml()
      : minixmlparser()
      {}

   //! constructor
   minixml(ministring filename)
      : minixmlparser(filename)
      {}

   //! destructor
   virtual ~minixml() {}

   protected:

   minikeyval<ministring> xml;

   virtual void tag();
   };

#endif
