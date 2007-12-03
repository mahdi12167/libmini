// (c) by Stefan Roettger

#include "curlbase.h"

curlbase::MULTICURL_TYPE **curlbase::MULTICURL=NULL;

int curlbase::MAXMULTICURL=0;
int curlbase::NUMMULTICURL=0;

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

void curlbase::curlinit(int threads,int id,char *proxyname,char *proxyport,void *data)
   {
   int i;

   if (data!=NULL) ERRORMSG();

   initmulticurl(id);

   MULTICURL[id]->numthreads=threads;

   // allocate curl handles
   MULTICURL[id]->curl_handle=new CURLPTR[threads+2];

   for (i=0; i<threads+2; i++)
      {
      // init the curl session
      MULTICURL[id]->curl_handle[i]=curl_easy_init();

      // send all data to this function
      curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_WRITEFUNCTION,WriteMemoryCallback);

      // optionally set the proxy server address
      if (proxyname!=NULL)
         {
         curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_PROXY,proxyname);
         if (proxyport!=NULL) curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_PROXYPORT,proxyport);
         }

      // some servers don't like requests that are made without a user-agent
      curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_USERAGENT,"libMini-agent/1.0");

      if (i<threads+1)
         {
         // request zlib decompression
         curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_ENCODING,"deflate");
         }
      else
         {
         // request header only
         curl_easy_setopt(MULTICURL[id]->curl_handle[i],CURLOPT_NOBODY,1);
         }
      }
   }

void curlbase::curlexit(int id,void *data)
   {
   int i;

   if (data!=NULL) ERRORMSG();

   // clean-up curl sessions
   for (i=0; i<MULTICURL[id]->numthreads+2; i++) curl_easy_cleanup(MULTICURL[id]->curl_handle[i]);

   // free curl handles
   delete[] MULTICURL[id]->curl_handle;

   exitmulticurl(id);
   }

void curlbase::getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background,int id,void *data)
   {
   char *url;

   struct MemoryStruct chunk;

   if (data!=NULL) ERRORMSG();

   chunk.memory=NULL;
   chunk.size=0;

   url=strcct(src_url,strcct(src_id,src_file));

   // pass the chunk struct to the callback function
   curl_easy_setopt(MULTICURL[id]->curl_handle[background],CURLOPT_WRITEDATA,(void *)&chunk);

   // specify URL to get
   curl_easy_setopt(MULTICURL[id]->curl_handle[background],CURLOPT_URL,url);

   // get it!
   curl_easy_perform(MULTICURL[id]->curl_handle[background]);

   // write memory chunk to file
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

int curlbase::checkURL(char *src_url,char *src_id,char *src_file,int id,void *data)
   {
   char *url;
   int threads;
   long response;

   struct MemoryStruct chunk;

   if (data!=NULL) ERRORMSG();

   chunk.memory=NULL;
   chunk.size=0;

   threads=MULTICURL[id]->numthreads;

   url=strcct(src_url,strcct(src_id,src_file));

   // pass the chunk struct to the callback function
   curl_easy_setopt(MULTICURL[id]->curl_handle[threads+1],CURLOPT_WRITEDATA,(void *)&chunk);

   // specify URL to get
   curl_easy_setopt(MULTICURL[id]->curl_handle[threads+1],CURLOPT_URL,url);

   // get it!
   curl_easy_perform(MULTICURL[id]->curl_handle[threads+1]);

   // query response code
   curl_easy_getinfo(MULTICURL[id]->curl_handle[threads+1],CURLINFO_RESPONSE_CODE,&response);

   // free memory chunk
   if (chunk.memory) free(chunk.memory);

   free(url);

   return(response==200);
   }

void curlbase::initmulticurl(int id)
   {
   if (MAXMULTICURL==0)
      {
      // init curl
      curl_global_init(CURL_GLOBAL_ALL);

      MAXMULTICURL=id+1;
      if ((MULTICURL=(MULTICURL_TYPE **)malloc(MAXMULTICURL*sizeof(MULTICURL_TYPE *)))==NULL) ERRORMSG();
      }

   if (id>=MAXMULTICURL)
      {
      MAXMULTICURL=id+1;
      if ((MULTICURL=(MULTICURL_TYPE **)realloc(MULTICURL,MAXMULTICURL*sizeof(MULTICURL_TYPE *)))==NULL) ERRORMSG();
      }

   MULTICURL[id]=new MULTICURL_TYPE;

   NUMMULTICURL++;
   }

void curlbase::exitmulticurl(int id)
   {
   delete MULTICURL[id];

   NUMMULTICURL--;

   if (NUMMULTICURL==0)
      {
      free(MULTICURL);
      MAXMULTICURL=0;

      // clean-up curl
      curl_global_cleanup();
      }
   }
