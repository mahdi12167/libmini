#include "minibase.h"

#include "miniio.h"

#include "lunaparse.h"

int main(int argc,char *argv[])
   {
   unsigned char *code;
   unsigned int bytes;

   lunaparse parser;
   float value;

   if (argc!=2 && argc!=3)
      {
      printf("usage: %s <input.luna> [<input.value>]\n",argv[0]);
      exit(1);
      }

   code=readfile(argv[1],&bytes);

   if (bytes>0)
      {
      code[bytes-1]='\0';

      while (--bytes>0)
         if (code[bytes-1]=='\n' || code[bytes-1]=='\r') code[bytes-1]='\0';
         else break;
      }

   printf("parsing code:\n----\n%s\n----\n",code);

   parser.setLUNAcode((char *)code,bytes);
   parser.parseLUNA();

   printf("executing code...\n");

   value=0.0f;

   if (argc==3)
      if (sscanf(argv[2],"%f",&value)!=1) value=0.0f;

   parser.getcode()->init();
   parser.getcode()->pushvalue(value);
   parser.getcode()->setdebug(1);
   parser.getcode()->execute();

   value=parser.getcode()->popvalue();

   printf("...yields value: %g\n",value);

   free(code);

   return(0);
   }
