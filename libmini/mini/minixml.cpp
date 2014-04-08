// (c) by Stefan Roettger

#include "minixml.h"

// load xml from file
BOOLINT minixmlparser::load(ministring filename)
   {
   ministrings infos;

   infos.load(filename);
   from_strings(infos);

   return(infos.empty());
   }

// save xml to file
void minixmlparser::save(ministring filename)
   {to_strings().save(filename);}

// serialization
ministrings minixmlparser::to_strings()
   {
   ministrings infos;

   CODEERROR();

   return(infos);
   }

// deserialization
void minixmlparser::from_strings(ministrings &infos)
   {
   tags_.clear();
   parser_=new lunaparse(FALSE);

   lunascan *scanner=parser_->getscanner();

   scanner->addtoken("<",XML_BRACKET_LEFT);
   scanner->addtoken(">",XML_BRACKET_RIGHT);
   scanner->addtoken("/",XML_SLASH);
   scanner->addtoken("</",XML_BRACKET_SLASH);
   scanner->addtoken("/>",XML_SLASH_BRACKET);
   scanner->addtoken("><",XML_BRACKET_BRACKET);
   scanner->addtoken("=",XML_EQUALS);

   parser_->setcode(infos.to_string().c_str());

   while (scanner->gettoken()!=lunascan::LUNA_END)
      if (scanner->gettoken()==XML_BRACKET_LEFT ||
          scanner->gettoken()==XML_BRACKET_SLASH)
         {
         parse_tag();

         while (scanner->gettoken()==XML_BRACKET_BRACKET)
            parse_tag();

         scanner->next();
         }
      else
         parser_->PARSERMSG("expected tag");

   if (!tags_.empty())
      parser_->PARSERMSG("tag mismatch");

   if (parser_->geterrors()==0) infos.clear();

   delete parser_;
   }

// parse an xml tag
void minixmlparser::parse_tag()
   {
   BOOLINT open;

   lunascan *scanner=parser_->getscanner();

   if (scanner->gettoken()==XML_BRACKET_LEFT ||
       scanner->gettoken()==XML_BRACKET_SLASH ||
       scanner->gettoken()==XML_BRACKET_BRACKET)
      {
      open=FALSE;

      if (scanner->gettoken()==XML_BRACKET_LEFT ||
          scanner->gettoken()==XML_BRACKET_BRACKET)
         {
         open=TRUE;
         scanner->next();

         if (scanner->gettoken()==XML_SLASH)
            {
            open=FALSE;
            scanner->next();
            }
         }
      else
         scanner->next();

      // opening tag
      if (open)
         {
         if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
            {
            scanner->addtoken(scanner->getstring(),XML_TAG);
            tags_.push_back(scanner->getstring());

            tag(); // track xml hierarchy
            }
         else if (scanner->gettoken()==XML_TAG)
            {
            tags_.push_back(scanner->getstring());

            tag(); // track xml hierarchy
            }
         else
            parser_->PARSERMSG("malformed tag");
         }
      // closing tag
      else
         {
         if (scanner->gettoken()==XML_TAG)
            {
            if (!tags_.empty())
               if (tags_.peek()==ministring(scanner->getstring()))
                  tags_.pop_back();
               else
                  parser_->PARSERMSG("unmatched tag");
            else
               parser_->PARSERMSG("tag mismatch");
            }
         else
            parser_->PARSERMSG("malformed tag");
         }

      scanner->next();

      while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
             scanner->gettoken()!=XML_SLASH &&
             scanner->gettoken()!=XML_SLASH_BRACKET &&
             scanner->gettoken()!=XML_BRACKET_BRACKET &&
             scanner->gettoken()!=lunascan::LUNA_END) scanner->next();

      if (scanner->gettoken()==XML_SLASH)
         {
         tags_.pop_back();
         scanner->next();

         if (scanner->gettoken()!=XML_BRACKET_RIGHT &&
             scanner->gettoken()!=XML_BRACKET_BRACKET)
            parser_->PARSERMSG("missing bracket");
         }
      else if (scanner->gettoken()==XML_SLASH_BRACKET)
         {
         tags_.pop_back();
         }
      else if (scanner->gettoken()==XML_BRACKET_BRACKET)
         {
         // double token
         }
      else if (scanner->gettoken()==lunascan::LUNA_END)
         parser_->PARSERMSG("missing bracket");
      }
   else
      parser_->PARSERMSG("expected tag");
   }

// found an xml tag
void minixmlparser::tag()
   {std::cout << tags_ << std::endl;}

// found an xml tag
void minixml::tag()
   {
   unsigned int i;

   ministring tag;

   if (!tags_.empty())
      {
      tag=tags_[0];

      for (i=1; i<tags_.getsize(); i++)
         tag+="."+tags_[i];

      xml.add(tag,"");

      std::cout << tag << std::endl;
      }
   }
