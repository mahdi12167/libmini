// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minilog.h"

ministring minilog::minilog_filename="minilog.txt";
#if !defined(LIBMINI_DEBUG) && !defined(_WIN32)
ministring minilog::minilog_dirname="/var/tmp";
#else
ministring minilog::minilog_dirname="";
#endif

BOOLINT minilog::minilog_switch=FALSE;
FILE *minilog::minilog_file=NULL;

void minilog::logfile(ministring filename)
   {
   minilog_filename=filename;
   close();
   }

void minilog::logdir(ministring dirname)
   {
   minilog_dirname=dirname;
   close();
   }

ministring minilog::get_logname()
   {
   if (!minilog_dirname.empty())
      if (minilog_dirname.last()!='/' && minilog_dirname.last()!='\\') minilog_dirname.append('/');

   return(minilog_dirname+
          minilog_filename);
   }

void minilog::log(ministring message)
   {
   if (minilog_switch)
      if (minilog_filename!="")
         {
         if (!minilog_file)
            if (!(minilog_file=fopen(get_logname().c_str(),"w"))) ERRORMSG();

         fprintf(minilog_file,"%s\n",message.c_str());
         fflush(minilog_file);
         }
   }

void minilog::close()
   {
   if (minilog_file!=NULL) fclose(minilog_file);
   minilog_file=NULL;
   }
