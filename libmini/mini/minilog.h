// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINILOG_H
#define MINILOG_H

#include "ministring.h"

//! logging
class minilog
   {
   public:

   static void on()
      {minilog_switch=TRUE;}

   static void off()
      {minilog_switch=FALSE;}

   static void logfile(ministring filename="");

   static void log(ministring message);

   protected:

   static ministring minilog_filename;
   static BOOLINT minilog_switch;
   static FILE *minilog_file;
   };

#define MINILOG(message) minilog::log(message)

#endif
