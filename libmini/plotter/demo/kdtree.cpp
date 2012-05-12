// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/minibase.h>
#include <mini/ministring.h>
#include <mini/minikdtree.h>
#include <mini/minicoord.h>

#include <plotter/plot.h>

minikdtree<ministring> kdtree;
double kdtree_minx,kdtree_maxx;
double kdtree_miny,kdtree_maxy;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);

   return(FALSE);
   }

BOOLINT mouse(float x,float y)
   {
   double utmx=(1.0-x)*kdtree_minx+x*kdtree_maxx;
   double utmy=(1.0-y)*kdtree_maxy+y*kdtree_miny;
   miniv3d v(utmx,utmy,0);

   const minikdtree<ministring>::Node *node=kdtree.search(v);

   if (node)
      std::cout << node->item << std::endl;

   return(FALSE);
   }

void render(double time) {}

void read()
   {
   const int max_line=1000;

   FILE *file;
   char line[max_line];

   kdtree_minx=MAXFLOAT;
   kdtree_maxx=-MAXFLOAT;
   kdtree_miny=MAXFLOAT;
   kdtree_maxy=-MAXFLOAT;

   file=fopen("DE.txt","r");
   assert(file);

   while ((fgets(line,max_line,file))!=NULL)
   {
      int column=0;

      double lat,lon;
      ministring name;

      const char *tok=strtok(line,"\t");

      bool success=false;

      while (tok)
         {
         switch (column)
            {
            case 3: name=ministring(tok); success=isalpha(*tok); break;
            case 4: success&=sscanf(tok,"%lf",&lat)==1; break;
            case 5: success&=sscanf(tok,"%lf",&lon)==1; break;
            }

         tok=strtok(NULL,"\t");
         column++;
         }

      if (column<10) success=false;

      if (lat<-90 || lat>90.0) success=false;
      if (lon<0 || lon>360.0) success=false;

      if (success)
         {
         minicoord coord(miniv3d(lon,lat,0),minicoord::MINICOORD_LLH);
         coord.convert2(minicoord::MINICOORD_UTM);
         miniv3d v=coord.vec;

         if (v.x<kdtree_minx) kdtree_minx=v.x;
         if (v.x>kdtree_maxx) kdtree_maxx=v.x;
         if (v.y<kdtree_miny) kdtree_miny=v.y;
         if (v.y>kdtree_maxy) kdtree_maxy=v.y;

         kdtree.insert(coord.vec,name);
         std::cout << lat << ";" << lon << ";" << name << std::endl;
      }
   }

   fclose(file);
}

int main(int argc,char *argv[])
   {
   read();

   plot_openwindow(&argc,argv,
                   512,512,
                   1.0f,1.0f,1.0f,
                   render,
                   keypress,
                   mouse,
                   TRUE);

   return(0);
   }
