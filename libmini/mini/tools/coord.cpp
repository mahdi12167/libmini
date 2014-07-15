// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/minicoord.h>

int main(int argc,char *argv[])
   {
   if (argc!=1)
      {
      printf("usage: echo <coord> | %s\n",argv[0]);
      printf(" with coord = LL(lat,lon)\n");
      printf("   or coord = LLH(lat,lon,height)\n");
      printf("   or coord = ECEF(x,y,z)\n");
      printf("   or coord = UTM(x,y,height,zone)\n");
      exit(1);
      }

   std::string s;
   std::cin >> s;

   ministring t(s.c_str());

   minicoord c;
   c.from_string(t);

   if (!t.empty())
      {
      std::cout << minicoord() << std::endl;
      return(1);
      }

   if (c.type==minicoord::MINICOORD_UTM)
      c.convert2llh();
   else if (c.type==minicoord::MINICOORD_LLH)
      c.convert2ecef();
   else if (c.type==minicoord::MINICOORD_ECEF)
      c.convert2utm();

   std::cout << c.to_string().c_str() << std::endl;

   return(0);
   }
