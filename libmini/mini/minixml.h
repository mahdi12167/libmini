// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIXML_H
#define MINIXML_H

#include "minikeyval.h"

class minixml : public minikeyval<ministring>
   {
   public:

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
   void from_strings(ministrings &info);
   };

#endif
