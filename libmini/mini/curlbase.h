// (c) by Stefan Roettger

#ifndef CURLBASE_H
#define CURLBASE_H

#include "minibase.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

class curlbase
   {
   public:

   static void curlinit(int threads=1,char *proxyname=NULL,char *proxyport=NULL);
   static void curlexit();

   static void getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background);
   static int checkURL(char *src_url,char *src_id,char *src_file);

   private:

   struct MemoryStruct
      {
      char *memory;
      size_t size;
      };

   typedef CURL *CURLPTR;

   static int numthreads;

   static CURLPTR *curl_handle;

   static void *myrealloc(void *ptr,size_t size);
   static size_t WriteMemoryCallback(void *ptr,size_t size,size_t nmemb,void *data);
   };

#endif
