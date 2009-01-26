#include "minibase.h"

#include "pnmbase.h"

#define MAXSTR (256)

float nextitem(FILE *file)
   {
   int ch;
   int i,j;
   char str[MAXSTR];

   i=0;
   while (isspace(ch=toupper(fgetc(file))));
   if (ch!=EOF) str[i++]=ch;

   while (!isspace(ch=toupper(fgetc(file))) && ch!=EOF)
      {
      if (i>=MAXSTR-1) break;
      if ((str[i++]=(ch=='D')?'E':ch)=='E') break;
      }

   if (ch=='D' || ch=='E')
      for (j=0; j<3; j++)
         if (!isspace(ch=toupper(fgetc(file))) && ch!=EOF)
            {
            if (i>=MAXSTR-1) break;
            str[i++]=ch;
            }
         else break;

   str[i]='\0';

   if (i==0) return(0.0f);
   return(atof(str));
   }

int main(int argc,char *argv[])
   {
   int i;

   FILE *file;

   char dem_description[81];
   int coord_sys,coord_zone,coord_datum;
   int coord_units,scaling_units;
   float coord_SW_x,coord_SW_y;
   float coord_NW_x,coord_NW_y;
   float coord_NE_x,coord_NE_y;
   float coord_SE_x,coord_SE_y;
   float cell_size_x,cell_size_y;
   float vertical_scaling;
   int missing_value;

   char *comment;

   unsigned char *image;

   int width,height,
       row,column;

   int elev;

   if (argc!=3 && argc!=4)
      {
      printf("usage: %s <input.dem> <output.pgm> [<missing value>]\n",argv[0]);
      printf("note: guaranteed to work for USGS 1:250,000 DEM quads only\n");
      exit(1);
      }

   if ((file=fopen(argv[1],"rb"))==NULL) exit(1);

   // DEM descriptor
   for (i=0; i<80; i++) dem_description[i]=fgetc(file);
   dem_description[80]='\0';

   for (i=0; i<29; i++) fgetc(file); // skip blank filler
   for (i=0; i<26; i++) fgetc(file); // skip geographic corner
   for (i=0; i<2; i++) fgetc(file); // skip process code
   for (i=0; i<3; i++) fgetc(file); // skip sectional indicator
   for (i=0; i<4; i++) fgetc(file); // skip origin code
   for (i=0; i<6; i++) fgetc(file); // skip DEM level code
   for (i=0; i<6; i++) fgetc(file); // skip elevation pattern

   // reference coordinate system
   coord_sys=ftrc(nextitem(file));
   coord_zone=ftrc(nextitem(file));
   coord_datum=0;

   if (coord_sys!=0) exit(1); // only 0=lat/lon supported

   for (i=0; i<15; i++) nextitem(file); // skip projection

   // reference units
   coord_units=ftrc(nextitem(file));
   scaling_units=ftrc(nextitem(file));

   // check units
   if (coord_units<0 || coord_units>3) exit(1);
   if (scaling_units<1 || scaling_units>2) exit(1);
   if (coord_units==3) coord_units=4;

   nextitem(file); // skip polygon sides

   // coordinates of corners
   coord_SW_x=nextitem(file);
   coord_SW_y=nextitem(file);
   coord_NW_x=nextitem(file);
   coord_NW_y=nextitem(file);
   coord_NE_x=nextitem(file);
   coord_NE_y=nextitem(file);
   coord_SE_x=nextitem(file);
   coord_SE_y=nextitem(file);

   nextitem(file); // skip minimum elevation
   nextitem(file); // skip maximum elevation

   // spatial resolution
   fseek(file,817,SEEK_SET);
   cell_size_x=nextitem(file);
   cell_size_y=nextitem(file);
   vertical_scaling=nextitem(file);

   // missing value
   missing_value=-9999;
   if (argc==4) if (sscanf(argv[3],"%d",&missing_value)!=1) exit(1);

   // DEM width
   if (ftrc(nextitem(file))!=1) exit(1);
   width=ftrc(nextitem(file));

   // actual row/column/height of DEM profile
   row=ftrc(nextitem(file));
   column=ftrc(nextitem(file));
   height=ftrc(nextitem(file));

   if ((image=(unsigned char *)malloc(2*width*height))==NULL) exit(1);

   while (row==1 && column>0 && column<=width)
      {
      for (i=0; i<6; i++) nextitem(file); // skip profile position

      // read elevations
      while (row++<=height)
         {
         elev=ftrc(nextitem(file));
         if (elev<0) elev=65536+elev;

         image[2*(column-1+(height-row+1)*height)]=elev/256;
         image[2*(column-1+(height-row+1)*height)+1]=elev%256;
         }

      // next profile
      row=ftrc(nextitem(file));
      column=ftrc(nextitem(file));

      nextitem(file); // ignore number of samples
      }

   fclose(file);

   comment=putPNMparams(dem_description,
                        coord_sys,coord_zone,coord_datum,
                        coord_units,
                        coord_SW_x,coord_SW_y,
                        coord_NW_x,coord_NW_y,
                        coord_NE_x,coord_NE_y,
                        coord_SE_x,coord_SE_y,
                        cell_size_x,cell_size_y,
                        scaling_units,
                        vertical_scaling,
                        missing_value);

   writePNMimage(argv[2],image,
                 width,height,2,
                 comment);

   free(comment);

   return(0);
   }
