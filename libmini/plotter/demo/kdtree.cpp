// (c) by Stefan Roettger, licensed under GPL 2+

#include <mini/minibase.h>
#include <mini/ministring.h>
#include <mini/minikdtree.h>
#include <mini/minicoord.h>
#include <mini/miniv3f.h>

#include <plotter/plot.h>

minikdtree<ministring> kdtree;
double utm_minx,utm_maxx;
double utm_miny,utm_maxy;
minidyna<miniv3f> points;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);

   return(FALSE);
   }

BOOLINT mouse(float x,float y)
   {
   double utmx=(1.0-x)*utm_minx+x*utm_maxx;
   double utmy=(1.0-y)*utm_maxy+y*utm_miny;
   miniv3d v(utmx,utmy,0);

   const minikdtree<ministring>::Node *node=kdtree.search(v);

   if (node)
      std::cout << node->item << std::endl;

   return(FALSE);
   }

void render(double time)
   {
   unsigned int i;

   for (i=0; i<points.getsize(); i++)
      {
      double utmx=points[i].x;
      double utmy=points[i].y;
      
      double x=(utmx-utm_minx)/(utm_maxx-utm_minx);
      double y=(utmy-utm_miny)/(utm_maxy-utm_miny);

      plot_point(x,y);
      }
   }

void read()
   {
   const int max_line=1000;

   FILE *file;
   char line[max_line];

   utm_minx=MAXFLOAT;
   utm_maxx=-MAXFLOAT;
   utm_miny=MAXFLOAT;
   utm_maxy=-MAXFLOAT;

   file=fopen("DE.tab","r");
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

         if (v.x<utm_minx) utm_minx=v.x;
         if (v.x>utm_maxx) utm_maxx=v.x;
         if (v.y<utm_miny) utm_miny=v.y;
         if (v.y>utm_maxy) utm_maxy=v.y;

         kdtree.insert(v,name);
         points.push(v);

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
