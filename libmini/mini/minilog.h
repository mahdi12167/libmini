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

   static void log(ministring msg);

   protected:

   static BOOLINT minilog_switch;
   static ministring minilog_file;
   };

#endif
