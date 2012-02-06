// (c) by Stefan Roettger

#include "miniio.h"

#include "minidds.h"

#define DDS_MAXSTR (256)

#define DDS_BLOCKSIZE (1<<20)
#define DDS_INTERLEAVE (1<<24)

#define DDS_RL (7)

#define DDS_ISINTEL (*((unsigned char *)(&DDS_INTEL)+1)==0)

char DDS_ID[]="DDS v3d\n";
char DDS_ID2[]="DDS v3e\n";

unsigned char *DDS_cache;
unsigned int DDS_cachepos,DDS_cachesize;

unsigned int DDS_buffer;
unsigned int DDS_bufsize;

unsigned short int DDS_INTEL=1;

// helper functions for DDS:

inline unsigned int DDS_shiftl(const unsigned int value,const unsigned int bits)
   {return((bits>=32)?0:value<<bits);}

inline unsigned int DDS_shiftr(const unsigned int value,const unsigned int bits)
   {return((bits>=32)?0:value>>bits);}

inline void DDS_swapuint(unsigned int *x)
   {
   unsigned int tmp=*x;

   *x=((tmp&0xff)<<24)|
      ((tmp&0xff00)<<8)|
      ((tmp&0xff0000)>>8)|
      ((tmp&0xff000000)>>24);
   }

void DDS_initbuffer()
   {
   DDS_buffer=0;
   DDS_bufsize=0;
   }

inline void DDS_clearbits()
   {
   DDS_cache=NULL;
   DDS_cachepos=0;
   DDS_cachesize=0;
   }

inline void DDS_writebits(unsigned int value,unsigned int bits)
   {
   value&=DDS_shiftl(1,bits)-1;

   if (DDS_bufsize+bits<32)
      {
      DDS_buffer=DDS_shiftl(DDS_buffer,bits)|value;
      DDS_bufsize+=bits;
      }
   else
      {
      DDS_buffer=DDS_shiftl(DDS_buffer,32-DDS_bufsize);
      DDS_bufsize-=32-bits;
      DDS_buffer|=DDS_shiftr(value,DDS_bufsize);

      if (DDS_cachepos+4>DDS_cachesize)
         if (DDS_cache==NULL)
            {
            if ((DDS_cache=(unsigned char *)malloc(DDS_BLOCKSIZE))==NULL) MEMERROR();
            DDS_cachesize=DDS_BLOCKSIZE;
            }
         else
            {
            if ((DDS_cache=(unsigned char *)realloc(DDS_cache,DDS_cachesize+DDS_BLOCKSIZE))==NULL) MEMERROR();
            DDS_cachesize+=DDS_BLOCKSIZE;
            }

      if (DDS_ISINTEL) DDS_swapuint(&DDS_buffer);
      *((unsigned int *)&DDS_cache[DDS_cachepos])=DDS_buffer;
      DDS_cachepos+=4;

      DDS_buffer=value&(DDS_shiftl(1,DDS_bufsize)-1);
      }
   }

inline void DDS_flushbits()
   {
   unsigned int bufsize;

   bufsize=DDS_bufsize;

   if (bufsize>0)
      {
      DDS_writebits(0,32-bufsize);
      DDS_cachepos-=(32-bufsize)/8;
      }
   }

inline void DDS_savebits(unsigned char **data,unsigned int *size)
   {
   *data=DDS_cache;
   *size=DDS_cachepos;
   }

inline void DDS_loadbits(unsigned char *data,unsigned int size)
   {
   DDS_cache=data;
   DDS_cachesize=size;

   if ((DDS_cache=(unsigned char *)realloc(DDS_cache,DDS_cachesize+4))==NULL) MEMERROR();
   *((unsigned int *)&DDS_cache[DDS_cachesize])=0;

   DDS_cachesize=4*((DDS_cachesize+3)/4);
   if ((DDS_cache=(unsigned char *)realloc(DDS_cache,DDS_cachesize))==NULL) MEMERROR();
   }

inline unsigned int DDS_readbits(unsigned int bits)
   {
   unsigned int value;

   if (bits<DDS_bufsize)
      {
      DDS_bufsize-=bits;
      value=DDS_shiftr(DDS_buffer,DDS_bufsize);
      }
   else
      {
      value=DDS_shiftl(DDS_buffer,bits-DDS_bufsize);

      if (DDS_cachepos>=DDS_cachesize) DDS_buffer=0;
      else
         {
         DDS_buffer=*((unsigned int *)&DDS_cache[DDS_cachepos]);
         if (DDS_ISINTEL) DDS_swapuint(&DDS_buffer);
         DDS_cachepos+=4;
         }

      DDS_bufsize+=32-bits;
      value|=DDS_shiftr(DDS_buffer,DDS_bufsize);
      }

   DDS_buffer&=DDS_shiftl(1,DDS_bufsize)-1;

   return(value);
   }

inline int DDS_code(int bits)
   {return(bits>1?bits-1:bits);}

inline int DDS_decode(int bits)
   {return(bits>=1?bits+1:bits);}

// deinterleave a byte stream
void DDS_deinterleave(unsigned char *data,unsigned int bytes,
                      unsigned int skip,unsigned int block=0,
                      BOOLINT restore=FALSE)
   {
   unsigned int i,j,k;

   unsigned char *data2,*ptr;

   if (skip<=1) return;

   if (block==0)
      {
      if ((data2=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

      if (!restore)
         for (ptr=data2,i=0; i<skip; i++)
            for (j=i; j<bytes; j+=skip) *ptr++=data[j];
      else
         for (ptr=data,i=0; i<skip; i++)
            for (j=i; j<bytes; j+=skip) data2[j]=*ptr++;

      memcpy(data,data2,bytes);
      }
   else
      {
      if ((data2=(unsigned char *)malloc((bytes<skip*block)?bytes:skip*block))==NULL) ERRORMSG();

      if (!restore)
         {
         for (k=0; k<bytes/skip/block; k++)
            {
            for (ptr=data2,i=0; i<skip; i++)
               for (j=i; j<skip*block; j+=skip) *ptr++=data[k*skip*block+j];

            memcpy(data+k*skip*block,data2,skip*block);
            }

         for (ptr=data2,i=0; i<skip; i++)
            for (j=i; j<bytes-k*skip*block; j+=skip) *ptr++=data[k*skip*block+j];

         memcpy(data+k*skip*block,data2,bytes-k*skip*block);
         }
      else
         {
         for (k=0; k<bytes/skip/block; k++)
            {
            for (ptr=data+k*skip*block,i=0; i<skip; i++)
               for (j=i; j<skip*block; j+=skip) data2[j]=*ptr++;

            memcpy(data+k*skip*block,data2,skip*block);
            }

         for (ptr=data+k*skip*block,i=0; i<skip; i++)
            for (j=i; j<bytes-k*skip*block; j+=skip) data2[j]=*ptr++;

         memcpy(data+k*skip*block,data2,bytes-k*skip*block);
         }
      }

   free(data2);
   }

// interleave a byte stream
void DDS_interleave(unsigned char *data,unsigned int bytes,
                    unsigned int skip,unsigned int block=0)
   {DDS_deinterleave(data,bytes,skip,block,TRUE);}

// encode a Differential Data Stream
void DDS_encode(unsigned char *data,unsigned int bytes,
                unsigned int skip,unsigned int strip,
                unsigned char **chunk,unsigned int *size,
                unsigned int block)
   {
   int i;

   unsigned char lookup[256];

   unsigned char *ptr1,*ptr2;

   int pre1,pre2,
       act1,act2,
       tmp1,tmp2;

   unsigned int cnt,cnt1,cnt2;
   int bits,bits1,bits2;

   if (bytes<1) ERRORMSG();

   if (skip<1 || skip>4) skip=1;
   if (strip<1 || strip>65536) strip=1;

   DDS_deinterleave(data,bytes,skip,block);

   for (i=-128; i<128; i++)
      {
      if (i<=0)
         for (bits=0; (1<<bits)/2<-i; bits++);
      else
         for (bits=0; (1<<bits)/2<=i; bits++);

      lookup[i+128]=bits;
      }

   DDS_initbuffer();

   DDS_clearbits();

   DDS_writebits(skip-1,2);
   DDS_writebits(strip-1,16);

   ptr1=ptr2=data;
   pre1=pre2=0;

   cnt=cnt1=cnt2=0;
   bits=bits1=bits2=0;

   while (cnt++<bytes)
      {
      tmp1=*ptr1;
      if (strip==1 || ptr1-strip<=data) act1=tmp1-pre1;
      else act1=tmp1-pre1-*(ptr1-strip)+*(ptr1-strip-1);
      pre1=tmp1;
      ptr1++;

      while (act1<-128) act1+=256;
      while (act1>127) act1-=256;

      bits=lookup[act1+128];

      bits=DDS_decode(DDS_code(bits));

      if (cnt1==0)
         {
         cnt1++;
         bits1=bits;
         continue;
         }

      if (cnt1<(1<<DDS_RL)-1 && bits==bits1)
         {
         cnt1++;
         continue;
         }

      if (cnt1+cnt2<(1<<DDS_RL) && (cnt1+cnt2)*max(bits1,bits2)<cnt1*bits1+cnt2*bits2+DDS_RL+3)
         {
         cnt2+=cnt1;
         if (bits1>bits2) bits2=bits1;
         }
      else
         {
         DDS_writebits(cnt2,DDS_RL);
         DDS_writebits(DDS_code(bits2),3);

         while (cnt2-->0)
            {
            tmp2=*ptr2;
            if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
            else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
            pre2=tmp2;
            ptr2++;

            while (act2<-128) act2+=256;
            while (act2>127) act2-=256;

            DDS_writebits(act2+(1<<bits2)/2,bits2);
            }

         cnt2=cnt1;
         bits2=bits1;
         }

      cnt1=1;
      bits1=bits;
      }

   if (cnt1+cnt2<(1<<DDS_RL) && (cnt1+cnt2)*max(bits1,bits2)<cnt1*bits1+cnt2*bits2+DDS_RL+3)
      {
      cnt2+=cnt1;
      if (bits1>bits2) bits2=bits1;
      }
   else
      {
      DDS_writebits(cnt2,DDS_RL);
      DDS_writebits(DDS_code(bits2),3);

      while (cnt2-->0)
         {
         tmp2=*ptr2;
         if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
         else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
         pre2=tmp2;
         ptr2++;

         while (act2<-128) act2+=256;
         while (act2>127) act2-=256;

         DDS_writebits(act2+(1<<bits2)/2,bits2);
         }

      cnt2=cnt1;
      bits2=bits1;
      }

   if (cnt2!=0)
      {
      DDS_writebits(cnt2,DDS_RL);
      DDS_writebits(DDS_code(bits2),3);

      while (cnt2-->0)
         {
         tmp2=*ptr2;
         if (strip==1 || ptr2-strip<=data) act2=tmp2-pre2;
         else act2=tmp2-pre2-*(ptr2-strip)+*(ptr2-strip-1);
         pre2=tmp2;
         ptr2++;

         while (act2<-128) act2+=256;
         while (act2>127) act2-=256;

         DDS_writebits(act2+(1<<bits2)/2,bits2);
         }
      }

   DDS_flushbits();
   DDS_savebits(chunk,size);

   DDS_interleave(data,bytes,skip,block);
   }

// decode a Differential Data Stream
void DDS_decode(unsigned char *chunk,unsigned int size,
                unsigned char **data,unsigned int *bytes,
                unsigned int block)
   {
   unsigned int skip,strip;

   unsigned char *ptr1,*ptr2;

   unsigned int cnt,cnt1,cnt2;
   int bits,act;

   DDS_initbuffer();

   DDS_clearbits();
   DDS_loadbits(chunk,size);

   skip=DDS_readbits(2)+1;
   strip=DDS_readbits(16)+1;

   ptr1=ptr2=NULL;
   cnt=act=0;

   while ((cnt1=DDS_readbits(DDS_RL))!=0)
      {
      bits=DDS_decode(DDS_readbits(3));

      for (cnt2=0; cnt2<cnt1; cnt2++)
         {
         if (strip==1 || cnt<=strip) act+=DDS_readbits(bits)-(1<<bits)/2;
         else act+=*(ptr2-strip)-*(ptr2-strip-1)+DDS_readbits(bits)-(1<<bits)/2;

         while (act<0) act+=256;
         while (act>255) act-=256;

         if ((cnt&(DDS_BLOCKSIZE-1))==0)
            if (ptr1==NULL)
               {
               if ((ptr1=(unsigned char *)malloc(DDS_BLOCKSIZE))==NULL) MEMERROR();
               ptr2=ptr1;
               }
            else
               {
               if ((ptr1=(unsigned char *)realloc(ptr1,cnt+DDS_BLOCKSIZE))==NULL) MEMERROR();
               ptr2=&ptr1[cnt];
               }

         *ptr2++=act;
         cnt++;
         }
      }

   if (ptr1!=NULL)
      if ((ptr1=(unsigned char *)realloc(ptr1,cnt))==NULL) MEMERROR();

   DDS_interleave(ptr1,cnt,skip,block);

   *data=ptr1;
   *bytes=cnt;
   }

// write a Differential Data Stream
void writeDDSfile(const char *filename,
                  unsigned char *data,unsigned int bytes,
                  unsigned int skip,unsigned int strip,
                  BOOLINT nofree)
   {
   int version=1;

   FILE *file;

   unsigned char *chunk;
   unsigned int size;

   if (bytes<1) ERRORMSG();

   if (bytes>DDS_INTERLEAVE) version=2;

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   fprintf(file,"%s",(version==1)?DDS_ID:DDS_ID2);

   DDS_encode(data,bytes,skip,strip,&chunk,&size,version==1?0:DDS_INTERLEAVE);

   if (chunk!=NULL)
      {
      if (fwrite(chunk,size,1,file)!=1) IOERROR();
      free(chunk);
      }

   fclose(file);

   if (!nofree) free(data);
   }

// read a Differential Data Stream
unsigned char *readDDSfile(const char *filename,unsigned int *bytes)
   {
   int version=1;

   FILE *file;

   int cnt;

   unsigned char *chunk,*data;
   unsigned int size;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   for (cnt=0; DDS_ID[cnt]!='\0'; cnt++)
      if (fgetc(file)!=DDS_ID[cnt])
         {
         fclose(file);
         version=0;
         break;
         }

   if (version==0)
      {
      if ((file=fopen(filename,"rb"))==NULL) return(NULL);

      for (cnt=0; DDS_ID2[cnt]!='\0'; cnt++)
         if (fgetc(file)!=DDS_ID2[cnt])
            {
            fclose(file);
            return(NULL);
            }

      version=2;
      }

   if ((chunk=readfiled(file,&size))==NULL) IOERROR();

   fclose(file);

   DDS_decode(chunk,size,&data,bytes,version==1?0:DDS_INTERLEAVE);

   free(chunk);

   return(data);
   }
