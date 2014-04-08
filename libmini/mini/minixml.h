// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIXML_H
#define MINIXML_H

#include "minidyna.h"
#include "minikeyval.h"
#include "lunaparse.h"

class minixml : public minikeyval<ministring>
   {
   public:

   enum
      {
      // xml tokens
      XML_BRACKET_LEFT=lunaparse::LUNA_UNUSED_TOKENS,
      XML_BRACKET_RIGHT,
      XML_EQUALS,
      XML_SLASH,
      XML_TAG
      };

   //! default constructor
   minixml()
      : minikeyval<ministring>()
      {}

   //! constructor
   minixml(ministring filename)
      : minikeyval<ministring>()
      {load(filename);}

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
   };

#endif
