// (c) by Stefan Roettger

#include "curlbase.h"

int curlbase::numthreads;

curlbase::CURLPTR *curlbase::curl_handle;

void *curlbase::myrealloc(void *ptr,size_t size)
   {
   if (ptr) return realloc(ptr,size);
   else return malloc(size);
   }

size_t curlbase::WriteMemoryCallback(void *ptr,size_t size,size_t nmemb,void *data)
   {
   size_t realsize=size*nmemb;
   struct MemoryStruct *mem=(struct MemoryStruct *)data;

   mem->memory=(char *)myrealloc(mem->memory,mem->size+realsize+1);

   if (mem->memory)
      {
      memcpy(&(mem->memory[mem->size]),ptr,realsize);
      mem->size+=realsize;
      mem->memory[mem->size]=0;
      }

   return(realsize);
   }

void curlbase::curlinit(int threads,char *proxyname,char *proxyport)
   {
   int i;

   numthreads=threads;

   curl_handle=new CURLPTR[numthreads+2];

   /* init curl */
   curl_global_init(CURL_GLOBAL_ALL);

   for (i=0; i<numthreads+2; i++)
      {
      /* init the curl session */
      curl_handle[i]=curl_easy_init();

      /* send all data to this function */
      curl_easy_setopt(curl_handle[i],CURLOPT_WRITEFUNCTION,WriteMemoryCallback);

      /* optionally set the proxy server address */
      if (proxyname!=NULL)
         {
         curl_easy_setopt(curl_handle[i],CURLOPT_PROXY,proxyname);
         if (proxyport!=NULL) curl_easy_setopt(curl_handle[i],CURLOPT_PROXYPORT,proxyport);
         }

      /* some servers don't like requests that are made without a user-agent */
      curl_easy_setopt(curl_handle[i],CURLOPT_USERAGENT,"libMini-agent/1.0");

      if (i<numthreads+1)
         {
         /* request zlib decompression */
         curl_easy_setopt(curl_handle[i],CURLOPT_ENCODING,"deflate");
         }
      else
         {
         /* request header only */
         curl_easy_setopt(curl_handle[i],CURLOPT_NOBODY,1);
         }
      }
   }

void curlbase::curlexit()
   {
   int i;

   /* cleanup curl stuff */
   for (i=0; i<numthreads+2; i++) curl_easy_cleanup(curl_handle[i]);

   delete[] curl_handle;
   }

char *curlbase::concat(char *str1,char *str2,char *str3)
   {
   char *str;

   if (str1==NULL || str2==NULL || str3==NULL) ERRORMSG();

   str=(char *)malloc(strlen(str1)+strlen(str2)+strlen(str3)+1);

   memcpy(str,str1,strlen(str1));
   memcpy(str+strlen(str1),str2,strlen(str2));
   memcpy(str+strlen(str1)+strlen(str2),str3,strlen(str3)+1);

   return(str);
   }

void curlbase::getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background)
   {
   char *url;

   struct MemoryStruct chunk;

   chunk.memory=NULL;
   chunk.size=0;

   url=concat(src_url,src_id,src_file);

   /* we pass our chunk struct to the callback function */
   curl_easy_setopt(curl_handle[background],CURLOPT_WRITEDATA,(void *)&chunk);

   /* specify URL to get */
   curl_easy_setopt(curl_handle[background],CURLOPT_URL,url);

   /* get it! */
   curl_easy_perform(curl_handle[background]);

   /* write to file */
   if (chunk.memory)
      {
      FILE *file;

      if ((file=fopen(dst_file,"wb"))==NULL) exit(1);
      fwrite(chunk.memory,1,chunk.size,file);
      fclose(file);

      free(chunk.memory);
      }

   free(url);
   }

int curlbase::checkURL(char *src_url,char *src_id,char *src_file)
   {
   char *url;
   long response;

   struct MemoryStruct chunk;

   chunk.memory=NULL;
   chunk.size=0;

   url=concat(src_url,src_id,src_file);

   /* we pass our chunk struct to the callback function */
   curl_easy_setopt(curl_handle[numthreads+1],CURLOPT_WRITEDATA,(void *)&chunk);

   /* specify URL to get */
   curl_easy_setopt(curl_handle[numthreads+1],CURLOPT_URL,url);

   /* get it! */
   curl_easy_perform(curl_handle[numthreads+1]);

   /* query response code */
   curl_easy_getinfo(curl_handle[numthreads+1],CURLINFO_RESPONSE_CODE,&response);

   /* free memory chunk */
   if (chunk.memory) free(chunk.memory);

   free(url);

   return(response==200);
   }
