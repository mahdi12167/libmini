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

minikdtree<ministring>::ItemPoints itempoints;
const minikdtree<ministring>::Node *node=NULL;

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

   node=kdtree.search(v);

   return(node!=NULL);
   }

void render(double time)
   {
   unsigned int i;

   plot_color(0.5f,0.5f,0.5f);

   for (i=0; i<itempoints.getsize(); i++)
      {
      double utmx=itempoints[i].point.x;
      double utmy=itempoints[i].point.y;

      double x=(utmx-utm_minx)/(utm_maxx-utm_minx);
      double y=(utmy-utm_miny)/(utm_maxy-utm_miny);

      plot_point(x,y);
      }

   if (node)
      {
      double utmx=node->plane.point.x;
      double utmy=node->plane.point.y;

      double x=(utmx-utm_minx)/(utm_maxx-utm_minx);
      double y=(utmy-utm_miny)/(utm_maxy-utm_miny);

      ministring name=node->item;
      plot_text(x,y,0.025f,0.0f,1.0f,1.0f,name.c_str());
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
   if (!file) ERRORMSG();

   while ((fgets(line,max_line,file))!=NULL)
   {
      int column=0;

      double lat,lon;
      ministring name;

      const char *tok=strtok(line,"\t");

      bool success=true;

      while (tok)
         {
         switch (column)
            {
            case 2:
               name=ministring(tok);
               success&=isalpha(name[0]);
               break;
            case 3:
               success&=isalpha(*tok);
               break;
            case 4:
               success&=sscanf(tok,"%lf",&lat)==1;
               break;
            case 5:
               success&=sscanf(tok,"%lf",&lon)==1;
               break;
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

         minikdtree<ministring>::ItemPoint p;

         p.item = name;
         p.point = v;

         itempoints.push(p);

         std::cout << lat << ";" << lon << ";" << name << std::endl;
      }
   }

   fclose(file);

   kdtree.insert(itempoints);
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
                   FALSE);

   return(0);
   }
