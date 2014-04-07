// (c) by Stefan Roettger

#include "lunaparse.h"

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

   while (scanner->gettoken()!=lunascan::LUNA_END)
      if (scanner->gettoken()==XML_BRACKET_LEFT)
         {
         scanner->next();

         while (scanner->gettoken()!=XML_BRACKET_RIGHT)
            if (scanner->gettoken()==lunascan::LUNA_END)
               {
               parser.PARSERMSG("unmatched bracket");
               break;
               }
            else scanner->next();
         }

   if (parser.geterrors()==0) infos.clear();
   }
