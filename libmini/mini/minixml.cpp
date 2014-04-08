// (c) by Stefan Roettger

#include "minixml.h"

// load xml from file
BOOLINT minixml::load(ministring filename)
   {
   ministrings infos;

   infos.load(filename);
   from_strings(infos);

   return(infos.empty());
   }

// save xml to file
void minixml::save(ministring filename)
   {to_strings().save(filename);}

// serialization
ministrings minixml::to_strings()
   {
   ministrings infos;

   CODEERROR();

   return(infos);
   }

// deserialization
void minixml::from_strings(ministrings &infos)
   {
   lunaparse parser(FALSE);
   lunascan *scanner=parser.getscanner();

   scanner->addtoken("<",XML_BRACKET_LEFT);
   scanner->addtoken(">",XML_BRACKET_RIGHT);
   scanner->addtoken("=",XML_EQUALS);
   scanner->addtoken("/",XML_SLASH);

   parser.setcode(infos.to_string().c_str());

   while (scanner->gettoken()!=lunascan::LUNA_END) parse_tag(&parser);

   if (parser.geterrors()==0) infos.clear();
   }

// parse an xml tag
void minixml::parse_tag(lunaparse *parser)
   {
   lunascan *scanner=parser->getscanner();

   if (scanner->gettoken()==XML_BRACKET_LEFT)
      {
      scanner->next();

      if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
         scanner->addtoken(scanner->getstring(),XML_TAG);
      else if (scanner->gettoken()!=XML_TAG)
         parser->PARSERMSG("malformed tag");

      scanner->next();

      while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
             scanner->gettoken()!=lunascan::LUNA_END) scanner->next();

      if (scanner->gettoken()==lunascan::LUNA_END)
         parser->PARSERMSG("unmatched bracket");
      }
   else
      parser->PARSERMSG("expected tag");

   scanner->next();
   }
