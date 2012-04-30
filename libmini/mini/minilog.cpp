// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minilog.h"

ministring minilog::minilog_filename="log.txt";
BOOLINT minilog::minilog_switch=TRUE;
FILE *minilog::minilog_file=NULL;

void minilog::logfile(ministring filename)
   {
   minilog_filename=filename;

   if (minilog_file!=NULL) fclose(minilog_file);
   minilog_file=NULL;
   }

void minilog::log(ministring message)
   {
   if (minilog_switch)
      if (minilog_filename!="")
         {
         if (!minilog_file)
            if (!(minilog_file=fopen(minilog_filename.c_str(),"w"))) ERRORMSG();

         fprintf(minilog_file,"%s\n",message.c_str());
         fflush(minilog_file);
         }
   }
