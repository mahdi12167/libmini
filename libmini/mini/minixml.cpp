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
   tags_.clear();
   parser_=new lunaparse(FALSE);

   lunascan *scanner=parser_->getscanner();

   scanner->addtoken("<",XML_BRACKET_LEFT);
   scanner->addtoken(">",XML_BRACKET_RIGHT);
   scanner->addtoken("=",XML_EQUALS);
   scanner->addtoken("/",XML_SLASH);

   parser_->setcode(infos.to_string().c_str());

   while (scanner->gettoken()!=lunascan::LUNA_END)
      parse_tag();

   if (!tags_.empty())
      parser_->PARSERMSG("tag mismatch");

   if (parser_->geterrors()==0) infos.clear();
   }

// parse an xml tag
void minixml::parse_tag()
   {
   lunascan *scanner=parser_->getscanner();

   if (scanner->gettoken()==XML_BRACKET_LEFT)
      {
      scanner->next();

      // opening tag
      if (scanner->gettoken()!=XML_SLASH)
         {
         if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
            {
            scanner->addtoken(scanner->getstring(),XML_TAG);
            tags_.push_back(scanner->getstring());

            std::cout << tags_ << std::endl; //!!
            }
         else if (scanner->gettoken()==XML_TAG)
            tags_.push_back(scanner->getstring());
         else
            parser_->PARSERMSG("malformed tag");
         }
      // closing tag
      else
         {
         scanner->next();

         if (scanner->gettoken()==XML_TAG)
            {
            if (!tags_.empty())
               if (tags_.peek()==ministring(scanner->getstring()))
                  tags_.pop_back();
               else
                  parser_->PARSERMSG("unmatched closing tag");
            else
               parser_->PARSERMSG("tag mismatch");
            }
         else
            parser_->PARSERMSG("malformed tag");
         }

      scanner->next();

      while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
             scanner->gettoken()!=lunascan::LUNA_END) scanner->next();

      if (scanner->gettoken()==lunascan::LUNA_END)
         parser_->PARSERMSG("unmatched bracket");
      }
   else
      parser_->PARSERMSG("expected tag");

   scanner->next();
   }
