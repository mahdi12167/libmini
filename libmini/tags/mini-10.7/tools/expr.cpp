// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/lunaparse.h>

int main(int argc,char *argv[])
   {
   lunaparse parser;
   float value;

   BOOLINT sw_debug;

   if (argc<2 || argc>3)
      {
      printf("usage: %s <expression> [-d]\n",argv[0]);
      exit(1);
      }

   sw_debug=FALSE;

   if (argc==3)
      if (strcmp(argv[2],"-d")==0) sw_debug=TRUE;

   printf("parsing expression:\n----\n%s\n----\n",argv[1]);

   parser.setpath(argv[0],"include/");
   parser.include("std.luna");
   parser.parseEXPR(argv[1]);

   if (sw_debug)
      {
      printf("compiled code:\n----\n");
      parser.getcode()->print();
      printf("----\n");
      }

   printf("executing code...\n");

   parser.getcode()->init();
   parser.getcode()->setdebug(sw_debug);
   parser.getcode()->execute();

   value=parser.getcode()->popvalue();

   printf("...yields value: %g\n",value);

   return(ftrc(value+0.5f));
   }
