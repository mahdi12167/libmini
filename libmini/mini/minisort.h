#ifndef MINISORT_H
#define MINISORT_H

#include "minibase.h"

#include "minidyna.h"

namespace minisort {

// Shellsort as proposed by Robert Sedgewick in "Algorithms"
//  the algorithm sorts the elements in ascending order
//  the < operator needs to be defined for two elements

// templated version for indexed arrays
template <class Item>
void shellsort(Item a[],const unsigned int n)
   {
   unsigned int i,j,h;

   Item v;

   if (n==0) return;

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
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

// templated version for pointer arrays
template <class Item>
void shellsort(Item *a[],const unsigned int n)
   {
   unsigned int i,j,h;

   Item *v;

   if (n==0) return;

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
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

// templated version for dynamic arrays
template <class Item>
void shellsort(minidyna<Item> &a)
   {
   unsigned int i,j,h;

   unsigned int n;

   Item v;

   n=a.getsize();

   if (n==0) return;

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
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

// templated version for dynamic pointer arrays
template <class Item>
void shellsort(minidyna<Item *> &a)
   {
   unsigned int i,j,h;

   unsigned int n;

   Item *v;

   n=a.getsize();

   if (n==0) return;

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
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

}

using namespace minisort;

#endif
