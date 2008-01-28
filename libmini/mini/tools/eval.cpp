#include "minibase.h"

#include "miniio.h"

#include "lunaparse.h"

int main(int argc,char *argv[])
   {
   unsigned char *code;
   unsigned int bytes;

   lunaparse parser;
   float value;

   if (argc!=2)
      {
      printf("usage: %s <input>\n",argv[0]);
      exit(1);
      }

   code=readfile(argv[1],&bytes);

   printf("parsing code:\n----\n%s----\n",code);

   parser.setLUNAcode((char *)code,bytes);
   parser.parseLUNA();

   printf("executing code...\n");

   parser.getcode()->init();
   parser.getcode()->execute();

   value=parser.getcode()->popvalue();

   printf("...yields value: %g\n",value);

   free(code);

   return(0);
   }
