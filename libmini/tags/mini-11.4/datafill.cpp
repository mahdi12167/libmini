// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"

#include "datafill.h"

// grow boundary by extrapolation
unsigned int datafill::grow_by_extrapolation(int radius)
   {
   int r;

   unsigned int count;

   count=0;

   for (r=0; r<radius; r++) count+=fillin(0);

   return(count);
   }

// grow boundary by extrapolation and replace the rest
unsigned int datafill::grow_by_extrapolation_and_replace(int radius,float value)
   {
   unsigned int count;

   count=grow_by_extrapolation(radius);
   count+=replacenodata(value);

   return(count);
   }

// fill-in no-data values by region growing
unsigned int datafill::fillin_by_regiongrowing(int radius_stop,int radius_start)
   {
   int r;

   unsigned int count;

   unsigned int sum;

   sum=0;

   for (r=radius_start; r<=radius_stop; r++)
      do
         {
         count=fillin(r);
         sum+=count;
         }
      while (count>0);

   return(sum);
   }

// fill-in no-data values by region growing and replace the rest
unsigned int datafill::fillin_by_regiongrowing_and_replace(int radius_stop,int radius_start,float value)
   {
   unsigned int count;

   count=fillin_by_regiongrowing(radius_stop,radius_start);
   count+=replacenodata(value);

   return(count);
   }

// fill-in algorithm
// replaces no-data values by region growing
// smoothly extrapolates the filled-in value via partial derivatives
// restricts the fill-in operation to concavities with a diameter of less than radius^2+1 pixels
// a radius of zero disables the concavity detection
unsigned int datafill::fillin(int radius)
   {
   unsigned int count;

   int i,j,k,t;
   int m,n,o;

   databuf buf;
   databuf cnt;
   databuf tmp;

   int size;
   int sizex,sizey,sizez;
   int sizedx,sizedy,sizedz;
   int thres;

   int cells;

   float v1,v2;
   float dx,dy,dz;
   int dxnum,dynum,dznum;
   float val,weight,sum;

   if (type!=DATABUF_TYPE_FLOAT) ERRORMSG();

   count=0;

   // check for no-data values
   if (checknodata()!=0)
      {
      // copy working buffer
      buf.duplicate(this);

      // allocate counting buffer
      cnt.alloc(xsize,ysize,zsize,tsteps,DATABUF_TYPE_BYTE);
      tmp.alloc(xsize,ysize,zsize,tsteps,DATABUF_TYPE_BYTE);

      // calculate foot print size
      size=max(2*radius+1,1);
      size=min(size,15);

      // calculate foot print size in x/y/z-direction
      if (xsize<2)
         {
         sizex=1;
         sizey=1;
         sizez=1;
         }
      else if (ysize<2)
         {
         sizex=size;
         sizey=1;
         sizez=1;
         }
      else if (zsize<2)
         {
         sizex=size;
         sizey=size;
         sizez=1;
         }
      else
         {
         sizex=size;
         sizey=size;
         sizez=size;
         }

      // calculate derivative foot print size
      sizedx=max(sizex,3);
      sizedy=max(sizey,3);
      sizedz=max(sizez,3);

      // calculate growing threshold
      thres=(sizex*sizey*sizez+1)/2;

      // count no-data values if growing threshold is greater than one
      if (thres>1)
         {
         // clear counting buffer
         cnt.clear();

         // search for no-data values
         for (t=0; t<(int)tsteps; t++)
            for (i=0; i<(int)xsize; i++)
               for (j=0; j<(int)ysize; j++)
                  for (k=0; k<(int)zsize; k++)
                     if (checkval(getval(i,j,k,t))) cnt.setval(i,j,k,t,1);

         // accumulate no-data values in x-direction
         if (xsize>1)
            for (t=0; t<(int)tsteps; t++)
               for (j=0; j<(int)ysize; j++)
                  for (k=0; k<(int)zsize; k++)
                     {
                     cells=0;

                     for (i=-sizex/2; i<(int)xsize; i++)
                        {
                        if (i-sizex/2-1>=0) cells-=ftrc(cnt.getval(i-sizex/2-1,j,k,t)+0.5f);
                        if (i+sizex/2<(int)xsize) cells+=ftrc(cnt.getval(i+sizex/2,j,k,t)+0.5f);

                        if (i>=0) tmp.setval(i,j,k,t,cells);
                        }
                     }

         // copy counting buffer back
         cnt.copy(&tmp);

         // accumulate no-data values in y-direction
         if (ysize>1)
            for (t=0; t<(int)tsteps; t++)
               for (i=0; i<(int)xsize; i++)
                  for (k=0; k<(int)zsize; k++)
                     {
                     cells=0;

                     for (j=-sizey/2; j<(int)ysize; j++)
                        {
                        if (j-sizey/2-1>=0) cells-=ftrc(cnt.getval(i,j-sizey/2-1,k,t)+0.5f);
                        if (j+sizey/2<(int)ysize) cells+=ftrc(cnt.getval(i,j+sizey/2,k,t)+0.5f);

                        if (j>=0) tmp.setval(i,j,k,t,cells);
                        }
                     }

         // copy counting buffer back
         cnt.copy(&tmp);

         // accumulate no-data values in z-direction
         if (zsize>1)
            for (t=0; t<(int)tsteps; t++)
               for (i=0; i<(int)xsize; i++)
                  for (j=0; j<(int)ysize; j++)
                     {
                     cells=0;

                     for (k=-sizez/2; k<(int)zsize; k++)
                        {
                        if (k-sizez/2-1>=0) cells-=ftrc(cnt.getval(i,j,k-sizez/2-1,t)+0.5f);
                        if (k+sizez/2<(int)zsize) cells+=ftrc(cnt.getval(i,j,k+sizez/2,t)+0.5f);

                        if (k>=0) tmp.setval(i,j,k,t,cells);
                        }
                     }

         // copy counting buffer back
         cnt.copy(&tmp);
         }
      else
         {
         // clear counting buffer
         cnt.clear(1.0f);
         }

      // search for no-data values
      for (t=0; t<(int)tsteps; t++)
         for (i=0; i<(int)xsize; i++)
            for (j=0; j<(int)ysize; j++)
               for (k=0; k<(int)zsize; k++)
                  if (checknodata(getval(i,j,k,t)))
                     // check number of foot print cells against growing threshold
                     if (ftrc(cnt.getval(i,j,k,t)+0.5f)>=thres)
                        {
                        dx=dy=dz=0.0f;
                        dxnum=dynum=dznum=0;

                        // average partial derivatives
                        for (m=-sizedx/2; m<=sizedx/2; m++)
                           for (n=-sizedy/2; n<=sizedy/2; n++)
                              for (o=-sizedz/2; o<=sizedz/2; o++)
                                 if (i+m>=0 && i+m<(int)xsize && j+n>=0 && j+n<(int)ysize && k+o>=0 && k+o<(int)zsize)
                                    {
                                    v1=getval(i+m,j+n,k+o,t);

                                    if (checkval(v1))
                                       {
                                       if (i+m-1>=0 && m>-sizedx/2)
                                          {
                                          v2=getval(i+m-1,j+n,k+o,t);

                                          if (checkval(v2))
                                             {
                                             dx+=v1-v2;
                                             dxnum++;
                                             }
                                          }

                                       if (j+n-1>=0 && n>-sizedy/2)
                                          {
                                          v2=getval(i+m,j+n-1,k+o,t);

                                          if (checkval(v2))
                                             {
                                             dy+=v1-v2;
                                             dynum++;
                                             }
                                          }

                                       if (k+o-1>=0 && o>-sizedz/2)
                                          {
                                          v2=getval(i+m,j+n,k+o-1,t);

                                          if (checkval(v2))
                                             {
                                             dz+=v1-v2;
                                             dznum++;
                                             }
                                          }
                                       }
                                    }

                        if (dxnum>0) dx/=dxnum;
                        if (dynum>0) dy/=dynum;
                        if (dznum>0) dz/=dznum;

                        val=0.0f;
                        sum=0.0f;

                        // extrapolate partial derivatives
                        for (m=-sizedx/2; m<=sizedx/2; m++)
                           for (n=-sizedy/2; n<=sizedy/2; n++)
                              for (o=-sizedz/2; o<=sizedz/2; o++)
                                 if (i+m>=0 && i+m<(int)xsize && j+n>=0 && j+n<(int)ysize && k+o>=0 && k+o<(int)zsize)
                                    {
                                    v1=getval(i+m,j+n,k+o,t);

                                    if (checkval(v1))
                                       {
                                       v2=v1-m*dx-n*dy-o*dz;
                                       weight=m*m+n*n+o*o;

                                       if (weight>0.0f)
                                          {
                                          val+=v2/weight;
                                          sum+=1.0f/weight;
                                          }
                                       }
                                    }

                        // fill-in extrapolated value
                        if (sum>0.0f)
                           {
                           buf.setval(i,j,k,t,val/sum);
                           if (!checknodata(getval(i,j,k,t))) count++;
                           }
                        }

      // copy working buffer back
      copy(&buf);

      // free working buffer
      buf.release();

      // free counting buffer
      cnt.release();
      tmp.release();
      }

   return(count);
   }
