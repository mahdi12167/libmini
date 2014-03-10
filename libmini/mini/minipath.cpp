// (c) by Stefan Roettger

#include "minipath.h"

BOOLINT minipath::load(ministring filename)
   {
   BOOLINT success;

   ministrings path;

   path.load(filename);

   if (path.empty()) return(FALSE);

   if (path[0]=="[track]") success=read_trk_format(path);
   else success=FALSE;

   return(success);
   }

void minipath::save(ministring filename)
   {writeministring(filename.c_str(),to_string());}

BOOLINT minipath::read_trk_format(ministrings trk)
   {
   return(FALSE); //!!
   }
