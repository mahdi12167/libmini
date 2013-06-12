// (c) by Stefan Roettger, licensed under GPL 2+

#undef INFO
#undef OUTPUT
#define BALANCED
#define TIME

#include <mini/minibase.h>
#include <mini/minitime.h>
#include <mini/ministring.h>
#include <mini/minikdtree.h>
#include <mini/minicoord.h>
#include <mini/miniv3d.h>

#include <plotter/plot.h>

minikdtree<ministring> kdtree;

minikdtree<ministring>::ItemPoints itempoints;
const minikdtree<ministring>::Node *node=NULL;

BOOLINT treevis=FALSE;
BOOLINT linear=FALSE;
BOOLINT slowdown=FALSE;

BOOLINT keypress(unsigned char key,float x,float y)
   {
   if (tolower(key)=='x') exit(0);
   else if (tolower(key)=='v')
      {
      treevis=!treevis;
      return(TRUE);
      }
   else if (tolower(key)=='l')
      linear=!linear;
   else if (tolower(key)=='s')
      slowdown=!slowdown;

   return(FALSE);
   }

BOOLINT mouse(float x,float y)
   {
#ifdef TIME
   double time=gettime();
#endif

   for (int c=0; c<(slowdown?1000:1); c++)
      if (!linear)
         node=kdtree.search(kdtree.denormalize(miniv3d(x,1.0-y,0)));
      else
         {
         miniv3d p=kdtree.denormalize(miniv3d(x,1.0-y,0));
         miniv3d n=itempoints[0].point;
         for (unsigned int i=1; i<itempoints.getsize(); i++)
            if (minikdtree<ministring>::getDistance(itempoints[i].point,p)<
                minikdtree<ministring>::getDistance(n,p))
               n=itempoints[i].point;
         node=kdtree.search(n);
         }

#ifdef TIME
   time=gettime()-time;
   printf("elapsed time: %.2gms\n",1000*time);
#endif

   return(node!=NULL);
   }

void plot_kdtree(miniv3d bboxmin,miniv3d bboxmax,
                 const minikdtree<ministring>::Node *node,
                 int level=0)
   {
   static const int maxlevel=10;

   if (node)
      {
      miniv3d normal = minikdtree<ministring>::getNormal(node);
      miniv3d point = minikdtree<ministring>::getPosition(node);

      miniv3d m1 = point-bboxmin;
      miniv3d o1(normal.x*m1.x,normal.y*m1.y,normal.z*m1.z);

      miniv3d m2 = bboxmax-point;
      miniv3d o2(normal.x*m2.x,normal.y*m2.y,normal.z*m2.z);

      if (level<maxlevel)
         {
         miniv3d p1 = bboxmin+o1;
         miniv3d p2 = bboxmax-o2;

         if (node->plane.orientation!=minikdtree<ministring>::z_axis)
            {
            if (level<maxlevel/4) plot_color(0,1,0);
            else if (level<maxlevel/2) plot_color(0,0,1);
            else
               {
               float v = 1.0f/(maxlevel/2)*(level-maxlevel/2);
               plot_color(v,v,v);
               }

            miniv3d np1 = kdtree.normalize(p1);
            miniv3d np2 = kdtree.normalize(p2);

            plot_line(np1.x,np1.y,np2.x,np2.y);
            }

         plot_kdtree(bboxmin,p2,node->leftSpace,level+1);
         plot_kdtree(p1,bboxmax,node->rightSpace,level+1);
         }
      }
   }

void plot_kdtree_path(const miniv3d &point,const minikdtree<ministring>::Node *node,int level=0)
   {
   if (node)
      {
      miniv3d p = minikdtree<ministring>::getPosition(node);
      miniv3d np = kdtree.normalize(p);

      if (level==0)
         {
         plot_color(1,0,0);
         plot_from(np.x,np.y);
         }
      else
         plot_to(np.x,np.y);

      if (point!=p)
         if (minikdtree<ministring>::isInLeftHalfSpace(point,node))
            plot_kdtree_path(point,node->leftSpace,level+1);
         else
            plot_kdtree_path(point,node->rightSpace,level+1);
      }
   }

void render(double time)
   {
   unsigned int i;

   if (treevis)
      {
      miniv3d bmin(0.0),bmax(0.0);

      if (kdtree.getBBox(bmin,bmax))
         plot_kdtree(bmin,bmax,kdtree.getRoot());

      if (node)
         plot_kdtree_path(minikdtree<ministring>::getPosition(node),kdtree.getRoot());
      }

   plot_color(0.5f,0.5f,0.5f);

   for (i=0; i<itempoints.getsize(); i++)
      {
      miniv3d p = kdtree.normalize(itempoints[i].point);

      plot_point(p.x,p.y);
      }

   if (node)
      {
      miniv3d p = kdtree.normalize(minikdtree<ministring>::getPosition(node));
      ministring name = minikdtree<ministring>::getItem(node);

      plot_text(p.x,p.y,0.025f,0.0f,1.0f,1.0f,name.c_str());
      }
   }

void read()
   {
   const int max_line=1000;

   FILE *file;
   char line[max_line];

   file=fopen("DE.tab","r");
   if (!file) ERRORMSG();

   while ((fgets(line,max_line,file))!=NULL)
      {
      int column=0;

      double lat,lon;
      ministring name;

      const char *tok = strtok(line,"\t");

      bool success=true;

      while (tok)
         {
         switch (column)
            {
            case 2:
               name = ministring(tok);
               success &= (isalpha(name[0])!=0);
               break;
            case 3:
               success &= (isalpha(*tok)!=0);
               break;
            case 4:
               success &= sscanf(tok,"%lf",&lat)==1;
               break;
            case 5:
               success &= sscanf(tok,"%lf",&lon)==1;
               break;
            }

         tok = strtok(NULL,"\t");
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

#ifndef BALANCED
         kdtree.insert(v,name);
#endif

         itempoints.push(minikdtree<ministring>::ItemPoint(name,v));

#ifdef OUTPUT
         printf("%g;%g;%s\n",lat,lon,name.c_str());
#endif
         }
      }

   fclose(file);

#ifdef BALANCED
   kdtree.insert(itempoints);
#endif

#ifdef INFO
   std::cout << "maximum tree depth: " << kdtree.depth() << std::endl;
   std::cout << "average path length: " << kdtree.length() << std::endl;
#endif
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
