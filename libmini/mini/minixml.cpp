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
   clear();

   parser_=new lunaparse(FALSE);

   lunascan *scanner=parser_->getscanner();

   // begin xml parsing
   begin();

   // register xml tokens
   scanner->addtoken("<",XML_BRACKET_LEFT);
   scanner->addtoken(">",XML_BRACKET_RIGHT);
   scanner->addtoken("/",XML_SLASH);
   scanner->addtoken("?",XML_QUESTION);
   scanner->addtoken("!",XML_XMARK);
   scanner->addtoken("><",XML_BRACKET_BRACKET);
   scanner->addtoken(":",XML_COLON);
   scanner->addtoken("=",XML_EQUAL);

   // pass xml
   parser_->setcode(infos.to_string("\n").c_str());

   // skim xml
   while (scanner->gettoken()!=lunascan::LUNA_END)
      {
      // skim tags
      if (scanner->gettoken()==XML_BRACKET_LEFT)
         {
         parse_tag();

         while (scanner->gettoken()==XML_BRACKET_BRACKET)
            parse_tag();
         }

      // skim text
      scanner->gettext('<');
      ministring text=scanner->getstring();
      text.remove_leading_white_space();
      text.remove_trailing_white_space();

      if (!text.empty()) pair(getname(),text); // track pairs

      scanner->next();
      }

   if (checkname())
      parser_->PARSERMSG("tag mismatch");

   // finish xml parsing
   finish();

   // parser success is indicated by consuming xml
   if (parser_->geterrors()==0) infos.clear();

   delete parser_;
   }

// parse an xml tag
void minixmlparser::parse_tag()
   {
   BOOLINT open,quest,xmark;

   lunascan *scanner=parser_->getscanner();

   // opening bracket
   if (scanner->gettoken()==XML_BRACKET_LEFT ||
       scanner->gettoken()==XML_BRACKET_BRACKET)
      {
      open=TRUE;
      quest=xmark=FALSE;

      scanner->next();

      if (scanner->gettoken()==XML_SLASH)
         {
         open=FALSE;
         scanner->next();
         }
      else if (scanner->gettoken()==XML_QUESTION)
         {
         quest=TRUE;
         scanner->next();
         }

      // opening tag
      if (open)
         {
         if (!quest)
            {
            if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
               {
               ministring name=parse_name();

               pushname(name);
               }
            else if (scanner->gettoken()==XML_XMARK)
               {
               xmark=TRUE;
               scanner->next();
               }
            else
               parser_->PARSERMSG("malformed tag");
            }
         else
            {
            if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
               {
               ministring name=parse_name();

               pushname(name);
               }
            else
               parser_->PARSERMSG("malformed question");
            }
         }
      // closing tag
      else
         if (!quest)
            {
            if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
               {
               ministring name=parse_name();

               if (checkname())
                  if (name==peekname())
                     popname();
                  else
                     parser_->PARSERMSG("unmatched tag");
               else
                  parser_->PARSERMSG("tag mismatch");
               }
            else
               parser_->PARSERMSG("malformed tag");
            }
         else
            parser_->PARSERMSG("malformed question");

      // closing bracket
      if (!quest)
         {
         // name=value pairs
         if (open)
            if (!xmark)
               while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
                      scanner->gettoken()!=XML_SLASH &&
                      scanner->gettoken()!=XML_BRACKET_BRACKET &&
                      scanner->gettoken()!=lunascan::LUNA_END)
                  parse_pair();
            else
               while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
                      scanner->gettoken()!=XML_BRACKET_BRACKET &&
                      scanner->gettoken()!=lunascan::LUNA_END)
                  scanner->next();

         if (scanner->gettoken()==XML_SLASH)
            {
            popname();
            scanner->next();

            if (scanner->gettoken()!=XML_BRACKET_RIGHT &&
                scanner->gettoken()!=XML_BRACKET_BRACKET)
               parser_->PARSERMSG("missing bracket");
            }
         else if (scanner->gettoken()==XML_BRACKET_BRACKET)
            {
            // double token
            }
         else if (scanner->gettoken()!=XML_BRACKET_RIGHT)
            parser_->PARSERMSG("missing bracket");
         }
      else
         {
         while (scanner->gettoken()!=XML_BRACKET_RIGHT &&
                scanner->gettoken()!=XML_QUESTION &&
                scanner->gettoken()!=XML_BRACKET_BRACKET &&
                scanner->gettoken()!=lunascan::LUNA_END)
            parse_pair();

         if (scanner->gettoken()==XML_QUESTION)
            {
            scanner->next();

            if (scanner->gettoken()!=XML_BRACKET_RIGHT &&
                scanner->gettoken()!=XML_BRACKET_BRACKET)
               parser_->PARSERMSG("missing bracket");
            }
         else if (scanner->gettoken()==XML_BRACKET_BRACKET)
            {
            // double token
            }
         else if (scanner->gettoken()!=XML_BRACKET_RIGHT)
            parser_->PARSERMSG("missing bracket");

         popname();
         }
      }
   else
      parser_->PARSERMSG("expected tag");
   }

// parse an xml name
ministring minixmlparser::parse_name()
   {
   lunascan *scanner=parser_->getscanner();

   ministring name;

   if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
      {
      name=ministring(scanner->getstring());

      scanner->next();

      while (scanner->gettoken()==XML_COLON)
         {
         scanner->next();

         if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
            name+=ministring(scanner->getstring());
         else
            parser_->PARSERMSG("expected name");

         scanner->next();
         }
      }

   return(name);
   }

// parse an xml name=value pair
void minixmlparser::parse_pair()
   {
   lunascan *scanner=parser_->getscanner();

   if (scanner->gettoken()==lunascan::LUNA_UNKNOWN)
      {
      ministring name=parse_name();

      if (scanner->gettoken()!=XML_EQUAL)
         parser_->PARSERMSG("equals sign expected");

      scanner->next();

      if (scanner->gettoken()==lunascan::LUNA_VALUE)
         {
         double value=scanner->getvalue();

         pair(getname()+"."+name,ministring(value)); // track pairs
         }
      else if (scanner->gettoken()==lunascan::LUNA_STRING)
         {
         ministring value=scanner->getstring();

         pair(getname()+"."+name,value); // track pairs
         }
      else
         parser_->PARSERMSG("value expected");
      }
   else
      parser_->PARSERMSG("pair expected");

   scanner->next();
   }

// clear name stack
void minixmlparser::clear()
   {
   tags_.clear();
   count_.clear();
   popped_=FALSE;
   }

// push tag name
void minixmlparser::pushname(ministring name)
   {
   unsigned int i;

   ministring tag;
   unsigned int size,count;

   size=tags_.getsize();
   if (popped_) size--;

   if (!popped_)
      {
      tags_.push_back(name);
      count_.push_back(1);
      }
   else
      {
      if (tags_[size]==name) count_[size]++;
      else
         {
         tags_[size]=name;
         count_[size]=1;
         }

      popped_=FALSE;
      size++;
      }

   for (i=0; i<size; i++)
      {
      if (i>0) tag+=".";
      tag+=tags_[i];

      count=count_[i];
      if (count>1) tag+="#"+ministring(count);
      }
   }

// check tag name
BOOLINT minixmlparser::checkname()
   {
   unsigned int size;

   size=tags_.getsize();
   if (popped_) size--;

   return(size>0);
   }

// peek tag name
ministring minixmlparser::peekname()
   {
   unsigned int size;

   size=tags_.getsize();
   if (popped_) size--;

   return(tags_[size-1]);
   }

// pop tag name
void minixmlparser::popname()
   {
   if (!popped_) popped_=TRUE;
   else
      {
      tags_.pop_back();
      count_.pop_back();
      }
   }

// get tag name
ministring minixmlparser::getname()
   {
   unsigned int i;

   ministring tag;
   unsigned int size,count;

   size=tags_.getsize();
   if (popped_) size--;

   for (i=0; i<size; i++)
      {
      if (i>0) tag+=".";
      tag+=tags_[i];

      count=count_[i];
      if (count>1) tag+="#"+ministring(count);
      }

   return(tag);
   }

// begin xml
void minixmlparser::begin()
   {}

// found an xml pair
void minixmlparser::pair(ministring name,ministring value)
   {std::cout << name << "=" << value << std::endl;}

// finished xml
void minixmlparser::finish()
   {}

// begin xml
void minixml::begin()
   {
   xml_.clear();
   list_.clear();
   }

// found an xml pair
void minixml::pair(ministring name,ministring value)
   {
   list_.append(minikeyval_pair<ministring>(name,value));

   std::cout << name << "=" << value << std::endl;
   }

// finished xml
void minixml::finish()
   {xml_.add(list_);}

// get xml pairs with prefix
minidyna< minikeyval_pair<ministring> > minixml::get_prefix(ministring prefix)
   {
   unsigned int i;

   minidyna< minikeyval_pair<ministring> > list;

   for (i=0; i<xml_.get_num(); i++)
      {
      const minikeyval_pair<ministring> *item=xml_.pair(i);
      if (item->key.startswith(prefix)) list.append(*item);
      }

   return(list);
   }
