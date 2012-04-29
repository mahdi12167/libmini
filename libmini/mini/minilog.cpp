// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minilog.h"

ministring minilog::minilog_file="log.txt";
BOOLINT minilog::minilog_switch=FALSE;

void minilog::log(ministring msg)
   {
   static FILE *out=NULL;

   if (minilog_switch)
      {
      if (!out)
         if (!(out=fopen(minilog_file.c_str(),"w"))) ERRORMSG();

      fprintf(out,"%s\n",msg.c_str());
      fflush(out);
      }
   }
