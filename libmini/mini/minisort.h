// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISORT_H
#define MINISORT_H

#include "minibase.h"

#include "minidyna.h"

namespace minisort {

// Shellsort as proposed by Robert Sedgewick in "Algorithms"
//  the algorithm sorts the elements in ascending order
//  the < operator needs to be overloaded for two elements
//  runtime is O(n^1.5), not stable

// templated version for indexed arrays
template <class Item>
void shellsort(Item a[],const unsigned int n)
   {
   unsigned int i,j,h;

   Item v;

   if (n==0) return;

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

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

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

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

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

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

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

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

// templated version for dynamic arrays with less comparison function
template <class Item>
void shellsort(minidyna<Item> &a,BOOLINT (*less)(const Item &a,const Item &b))
   {
   unsigned int i,j,h;

   unsigned int n;

   Item v;

   n=a.getsize();

   if (n==0) return;

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && less(v,a[j-h]))
            {
            a[j]=a[j-h];
            j-=h;
            }
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

// templated version for dynamic pointer arrays with less comparison function
template <class Item>
void shellsort(minidyna<Item *> &a,BOOLINT (*less)(const Item &a,const Item &b))
   {
   unsigned int i,j,h;

   unsigned int n;

   Item *v;

   n=a.getsize();

   if (n==0) return;

   for (h=1; h<=(n-1)/9; h=3*h+1) ;

   while (h>0)
      {
      for (i=h; i<n; i++)
         {
         j=i;
         v=a[i];
         while (j>=h && less((*v),(*a[j-h])))
            {
            a[j]=a[j-h];
            j-=h;
            }
         if (j!=i) a[j]=v;
         }
      h/=3;
      }
   }

// Mergesort as proposed by John von Neumann in 1945
//  the algorithm sorts the elements in ascending order
//  the < operator needs to be overloaded for two elements
//  runtime is O(nlogn), stable

// merge two halves
template<class Item>
void merge(minidyna<Item *> &a,
           unsigned int begin, unsigned int middle, unsigned int end,
           minidyna<Item *> &tmp)
   {
   unsigned int b=begin, m=middle, t=begin;

   while (b<middle && m<end)
      if (*(a[b])<*(a[m])) tmp[t++] = a[b++];
      else tmp[t++] = a[m++];

   while (b<middle) tmp[t++] = a[b++];
   while (m<end) tmp[t++] = a[m++];

   for (t=begin; t<end; t++) a[t] = tmp[t];
   }

// mergesort by recursively merging two halves
template<class Item>
void mergesort(minidyna<Item *> &a,
               unsigned int begin, unsigned int end,
               minidyna<Item *> &tmp)
   {
   unsigned int s = end-begin;

   if (s>1)
      {
      unsigned int middle = begin+s/2;

      mergesort(a, begin, middle, tmp);
      mergesort(a, middle, end, tmp);

      merge(a, begin, middle, end, tmp);
      }
   }

// mergesort
template <class Item>
void mergesort(minidyna<Item *> &a)
   {
   minidyna<Item *> tmp;
   tmp.setsize(a.getsize());

   mergesort(a, 0, a.getsize(), tmp);
   }

}

using namespace minisort;

#endif
