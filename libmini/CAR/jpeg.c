#include "jpegP.h"

unsigned char *read_JPEG_file(char *filename,int width,int height)
   {
   int i;

   unsigned char *image,**tab;

   FILE *infile;

   struct jpeg_error_mgr error;

   struct jpeg_decompress_struct info;

   if ((image=malloc(width*height*3))==NULL) ERROR();

   if ((tab=malloc(height*sizeof(unsigned char *)))==NULL) ERROR();
   for (i=0; i<height; i++) tab[i]=&image[i*width*3];

   if ((infile=fopen(filename,"r"))==NULL) ERROR();

   info.err=jpeg_std_error(&error);
   jpeg_create_decompress(&info);
   jpeg_stdio_src(&info,infile);

   jpeg_read_header(&info,TRUE);
   if (info.image_width!=width ||
       info.image_height!=height ||
       info.num_components!=3) ERROR();

   jpeg_start_decompress(&info);
   while (info.output_scanline<info.output_height)
      jpeg_read_scanlines(&info,&tab[info.output_scanline],1);

   jpeg_finish_decompress(&info);
   jpeg_destroy_decompress(&info);
   if (error.num_warnings!=0) ERROR();

   fclose(infile);

   free(tab);

   return(image);
   }
