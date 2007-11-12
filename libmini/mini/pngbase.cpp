// (c) by Stefan Roettger

#include "minibase.h"

#include "pngbase.h"

namespace pngbase {

#include <stdio.h>

extern "C"
   {
   #include <png.h>
   }

struct PNG_USER_READ_STRUCT
   {
   unsigned char *data;
   unsigned int bytes;
   unsigned int ptr;
   };

typedef PNG_USER_READ_STRUCT PNG_USER_READ_PARAMS;

// user read function
void user_read_fn(png_structp png_ptr,png_bytep ptr,png_size_t size)
   {
   PNG_USER_READ_PARAMS *params=(PNG_USER_READ_PARAMS *)png_get_io_ptr(png_ptr);

   if (params->ptr+size>params->bytes) png_error(png_ptr,"read error");
   else
      {
      memcpy(ptr,&params->data[params->ptr],size);
      params->ptr+=size;
      }
   }

// decompress PNG image
unsigned char *decompressPNGimage(unsigned char *data,int bytes,int *width,int *height,int *components)
   {
   double gamma;

   // declare PNG pointers
   png_structp png_ptr;
   png_infop info_ptr;

   // declare PNG image parameters
   png_uint_32 image_width,image_height,image_channels;
   int bit_depth,color_type,interlace_type;

   // declare PNG image chunk
   unsigned char *image_data;

   // declare PNG user read parameters
   PNG_USER_READ_PARAMS png_user_read_params;

   // check PNG signature
   if (bytes<8) return(NULL);
   if (!png_sig_cmp(data,0,8)) return(NULL);

   // create PNG read struct
   png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,png_voidp_NULL,png_error_ptr_NULL,png_error_ptr_NULL);
   if (png_ptr==NULL) return(NULL);

   // create PNG info struct
   info_ptr=png_create_info_struct(png_ptr);
   if (info_ptr==NULL)
      {
      png_destroy_read_struct(&png_ptr,png_infopp_NULL,png_infopp_NULL);
      return(NULL);
      }

   // set PNG error handler
   if (setjmp(png_jmpbuf(png_ptr)))
      {
      png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);
      return(NULL);
      }

   // initialize PNG user read parameters
   png_user_read_params.data=data;
   png_user_read_params.bytes=bytes;
   png_user_read_params.ptr=8;

   // set PNG user read function
   png_set_read_fn(png_ptr,(void *)&png_user_read_params,user_read_fn);

   // tell PNG that the header has been read
   png_set_sig_bytes(png_ptr,8);

   // ask PNG to read the image header
   png_read_info(png_ptr,info_ptr);

   // get PNG image header
   png_get_IHDR(png_ptr,info_ptr,&image_width,&image_height,&bit_depth,&color_type,&interlace_type,int_p_NULL,int_p_NULL);

   // ask PNG to strip 16 bit colors down to 8 bits
   png_set_strip_16(png_ptr);

   // ask PNG to extract multiple pixels into separate bytes
   png_set_packing(png_ptr);

   // ask PNG to expand paletted colors into true RGB triplets
   if (color_type==PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);

   // ask PNG to expand grayscale images to full 8 bits
   if (color_type==PNG_COLOR_TYPE_GRAY && bit_depth<8) png_set_gray_1_2_4_to_8(png_ptr);

   // ask PNG to expand paletted or RGB images with transparency to full alpha channels
   if (png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

   // set PNG gamma to neutral value
   if (png_get_gAMA(png_ptr,info_ptr,&gamma)) png_set_gamma(png_ptr,1.0,gamma);
   else png_set_gamma(png_ptr,1.0,0.45455); // assume Windows gamma

   // update PNG info struct
   png_read_update_info(png_ptr, info_ptr);

   // declare PNG row pointers
   png_bytep row_pointers[image_height];
   png_uint_32 row,rowbytes;

   // get PNG bytes per row
   rowbytes=png_get_rowbytes(png_ptr, info_ptr);

   // get PNG number of channels
   image_channels=png_get_channels(png_ptr, info_ptr);

   // allocate the memory to hold the image
   if ((image_data=(unsigned char *)malloc(rowbytes*image_height))==NULL)
      {
      png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
      return(NULL);
      }

   // prepare PNG row pointers
   for (row=0; row<image_height; row++) row_pointers[row]=image_data+row*rowbytes;

   // finally read the PNG image
   png_read_image(png_ptr,row_pointers);

   // read the rest of the PNG image
   png_read_end(png_ptr,info_ptr);

   // clean up and free any memory allocated
   png_destroy_read_struct(&png_ptr,&info_ptr,png_infopp_NULL);

   *width=image_width;
   *height=image_height;
   *components=image_channels;

   return(image_data);
   }

void compressPNGimage(unsigned char *image,int width,int height,int components,unsigned char **data,unsigned int *bytes)
   {
   //!! not yet implemented
   }

}
