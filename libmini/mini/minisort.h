#ifndef MINISORT_H
#define MINISORT_H

#include "minibase.h"

#include "minidyna.h"

namespace minisort {

// Shellsort as proposed by Robert Sedgewick in "Algorithms":

// templated version for indexed arrays
template <class Item>
void shellsort(Item a[],const unsigned int n)
   {
   unsigned int i,j,h;

   Item v;

   for (h=1; h<=(n-1)/9; h=3*h+1);

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && v<a[j-h])
            {
            a[j]=a[j-h];
            j-=h;
            }
         a[j]=v;
         }
      h/=3;
      }
   }

// templated version for indexed pointer arrays
template <class Item>
void shellsort(Item *a[],const unsigned int n)
   {
   unsigned int i,j,h;

   Item *v;

   for (h=1; h<=(n-1)/9; h=3*h+1);

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && (*v)<(*a[j-h]))
            {
            a[j]=a[j-h];
            j-=h;
            }
         a[j]=v;
         }
      h/=3;
      }
   }

// templated version for minidyna arrays
template <class Item>
void shellsort(minidyna<Item> &a)
   {
   unsigned int i,j,h;

   unsigned int n;

   Item v;

   n=a.getsize();

   for (h=1; h<=(n-1)/9; h=3*h+1);

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && v<a[j-h])
            {
            a[j]=a[j-h];
            j-=h;
            }
         a[j]=v;
         }
      h/=3;
      }
   }

}

using namespace minisort;

#endif
