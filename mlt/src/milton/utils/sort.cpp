/* ---------------------------------------------- *\
   file: sort.cpp
   auth: Travis Fischer
   acct: tfischer
   date: 3/18/2007
         9/17/2008
   
      Implementations of radix sort and randomized 
   quicksort, which are templated to handle sorting 
   arrays of builtin signed / unsigned ints, floats, 
   and doubles.
      Quicksort should handle sorting arrays of any 
   type implementing the inequality operator (<=).
      The radix sort routines assume standard sizes 
   for builtin types on a a 32-bit platform.  
   Specifically, they assume 32-bit floats / ints / 
   longs and 64-bit doubles / long longs.
      Radix sort runs in O(n), whereas randomized 
   quicksort runs in O(nlog n) but is more generic.
\* ---------------------------------------------- */

#include "sort.h"
#include <common.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <boost/static_assert.hpp>

// disable warnings for valid hacks
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(disable : 4146)
#endif

namespace milton {

BOOST_STATIC_ASSERT(sizeof(unsigned int) == sizeof(int));
BOOST_STATIC_ASSERT(sizeof(unsigned int) == sizeof(float));
BOOST_STATIC_ASSERT(sizeof(unsigned long long) == sizeof(double));

BOOST_STATIC_ASSERT(std::numeric_limits<unsigned int>::digits == 32);
BOOST_STATIC_ASSERT(std::numeric_limits<unsigned long long>::digits == 64);

/// @see http://www.stereopsis.com/radix.html for notes which inspired these 
/// floating point radix sort implementations

// assumes 32 bit floats and unsigned ints
#define FLIP_FLOAT(f)                              \
   (f) ^ (-((f) >> 31) | 0x80000000)

#define FLIP_FLOAT_INVERSE(f)                      \
   (f) ^ ((((f) >> 31) - 1) | 0x80000000)

// assumes 64 bit doubles and unsigned long longs
#define FLIP_DOUBLE(d)                             \
   (d) ^ (-((d) >> 63) | 0x800000000000000ULL)

#define FLIP_DOUBLE_INVERSE(d)                     \
   (d) ^ ((((d) >> 63) - 1) | 0x800000000000000ULL)

template <typename T>
static inline void stableSort(const T *a, T *b, 
                              const unsigned int n, const T mask, 
                              const unsigned int shift);
template <typename T>
static inline void stableSortIndices(
                              const T *a, 
                              unsigned int *indices, 
                              unsigned int *b, 
                              const unsigned int n, const T mask, 
                              const unsigned int shift);
// For randomized quicksort
static inline unsigned int randNum(unsigned int limit);
static inline int randNo(int low, int high);

#define SWAP(x, y) \
   do {            \
      (x) ^= (y);  \
      (y) ^= (x);  \
      (x) ^= (y);  \
   } while(0)

// Expected O(nlog n).  Worse-Case O(n^2)
// p and q are inclusive
// Ex:  quickSort(array, 0, size - 1);
template <typename T>
void quickSort(T *a, const int p, const int q) {
   if (p >= q)
      return;
   
   int pivot = randNo(p, q);
   const T pivotVal = a[pivot];
   int left = p, right = q;
  
   // ensure pivot appears in left-most position
   if (pivot != left)
      SWAP(a[pivot], a[left]);
   
   ++left;
   
   // swap elements appearing in wrong pos w/ relation to pivotVal
   while(left < right) {
      while(a[left] <= pivotVal && left < right)
         ++left;
      
      while(a[right] > pivotVal)// && left <= right)
         --right;
      
      if (left < right)
         SWAP(a[left], a[right]);
   }
   
   if (a[right] < a[p])
      SWAP(a[right], a[p]);
   
   quickSort<T>(a, p, right - 1);
   quickSort<T>(a, right + 1, q);
}

// generic radix sort for 32-bit unsigned integers
template <>
void radixSort(unsigned int *a, unsigned int *b, const unsigned int n) {
   // temporary scratch buffer
   const unsigned int mask = 0xff;
   unsigned int i;
   
   for(i = 0; i < sizeof(int) << 3; i += 8) {
      stableSort<unsigned int>(a, b, n, mask << i, i);
      
      unsigned int *temp = a;
      a = b;
      b = temp;
   }
}

// radix sort specialized for 32-bit signed integers
template <>
void radixSort(int *a, int *b, const unsigned int n) {
   // temporary scratch buffer
   unsigned int *c = (unsigned int*)a;
   const unsigned int mask = 0xff;
   unsigned int i;
   
   // always flip sign bit; search 2's complement online for more info
   for(i = n; i--;)
      c[i] ^= 0x80000000;
   
   for(i = 0; i < sizeof(int) << 3; i += 8) {
      stableSort<unsigned int>((unsigned int*)a, (unsigned int*)b, 
                               n, mask << i, i);
      
      int *temp = a;
      a = b;
      b = temp;
   }
   
   // flip sign bit back
   for(i = n; i--;)
      c[i] ^= 0x80000000;
}

// radix sort specialized for single precision floating point numbers
template <>
void radixSort(float *a, float *b, const unsigned int n) {
   // temporary scratch buffer
   unsigned int *c = (unsigned int*)a;
   const unsigned int mask = 0xff;
   unsigned int i;
   
   for(i = n; i--;)
      c[i] = FLIP_FLOAT(c[i]);
   
   for(i = 0; i < sizeof(float) << 3; i += 8) {
      stableSort<unsigned int>((unsigned int*)a, (unsigned int*)b, 
                               n, mask << i, i);
      
      float *temp = a;
      a = b;
      b = temp;
   }
   
   for(i = n; i--;)
      c[i] = FLIP_FLOAT_INVERSE(c[i]);
}

// radix sort specialized for double precision floating point numbers
template <>
void radixSort(double *a, double *b, const unsigned int n) {
   // temporary scratch buffer
   unsigned long long *c = (unsigned long long*)a;
   const unsigned long long mask = 0xff;
   unsigned int i;
   
   for(i = n; i--;)
      c[i] = FLIP_DOUBLE(c[i]);
   
   for(i = 0; i < sizeof(double) << 3; i += 8) {
      stableSort<unsigned long long>((unsigned long long*)a, 
                                     (unsigned long long*)b, 
                                     n, mask << i, i);
      
      double *temp = a;
      a = b;
      b = temp;
   }
   
   for(i = n; i--;)
      c[i] = FLIP_DOUBLE_INVERSE(c[i]);
}

// Stable counting sort used as a subroutine in radix sort
// returns a sorted in b with respect to the bits specified 
//    by (mask << shift)
template <typename T>
static inline void stableSort(const T *a, T *b, 
                              const unsigned int n, const T mask, 
                              const unsigned int shift)
{
   static unsigned int c[256];
   unsigned int i;
   
   memset(c, 0, sizeof(unsigned int) * 256);
   for(i = 0; i < n; ++i)
      ++c[(a[i] & mask) >> shift];
   
   // check if input array is already sorted with respect to the current mask
   /*for(i = 0; i < n; ++k) {
      if (c[(a[i] & mask) >> shift] == n) {
         memcpy(a, b, sizeof(T) * n);
         return;
      }
   }*/
   
   // compute partial sums
   for(i = 1; i < 256; ++i)
      c[i] += c[i - 1];
   
   // place elements of a into b in sorted order
   // by knowing the number of elements less than a[i], 
   // we know the index of where a[i] needs to appear in b
   for(i = n; i--;) {
      const T temp = a[i];
      
      // decrement c to accomodate for duplicates
      b[--c[(temp & mask) >> shift]] = temp;
   }
}

// For randomized quicksort
static inline unsigned int randNum(unsigned int limit) {
   if (limit <= 0)
      return 0;

   return rand() % limit;
}

// For randomized quicksort
static inline int randNo(int low, int high) { // inclusive
   return (low + randNum(high - low));
}


// radix sort specialized for unsigned integers
template <>
void radixSortIndices(unsigned int *a, unsigned int *indices, unsigned int *b, 
                     const unsigned int n)
{
   unsigned int *c = (unsigned int*)a;
   const unsigned long long mask = 0xff;
   unsigned int i;
   
   for(i = 0; i < sizeof(double) << 3; i += 8) {
      stableSortIndices<unsigned int>(c, indices, b, n, mask << i, i);

      unsigned int *temp = b;
      b = indices;
      indices = temp;
   }
}

// radix sort specialized for double precision floating point numbers
template <>
void radixSortIndices(double *a, unsigned int *indices, unsigned int *b, 
                     const unsigned int n)
{
   unsigned long long *c = (unsigned long long*)a;
   const unsigned long long mask = 0xff;
   unsigned int i;
   
   for(i = n; i--;) {
      c[i] = FLIP_DOUBLE(c[i]);
      indices[i] = i; // TODO: take out?
   }
   
   for(i = 0; i < sizeof(double) << 3; i += 8) {
      stableSortIndices<unsigned long long>(c, indices, b, n, mask << i, i);
      
      unsigned int *temp = b;
      b = indices;
      indices = temp;
   }
   
   for(i = n; i--;)
      c[i] = FLIP_DOUBLE_INVERSE(c[i]);
}

// Stable counting sort used as a subroutine in radix sort
// returns a sorted in b with respect to the bits specified 
//    by (mask << shift)
template <typename T>
static inline void stableSortIndices(const T *a, 
                                     unsigned int *indices, 
                                     unsigned int *b, 
                                     const unsigned int n, const T mask, 
                                     const unsigned int shift)
{
   static unsigned int c[256];
   unsigned int i;
   
   memset(c, 0, sizeof(unsigned int) * 256);
   for(i = 0; i < n; ++i)
      ++c[(a[i] & mask) >> shift];
   
   // compute partial sums
   for(i = 1; i < 256; ++i)
      c[i] += c[i - 1];
   
   // place elements of a into b in sorted order.
   // by knowing the number of elements less than a[i], 
   // we know the index of where a[i] needs to appear in b
   for(i = n; i--;) {
      const unsigned int oldIndex = indices[i];
      
      // decrement c to accomodate for duplicates
      b[--c[(a[oldIndex] & mask) >> shift]] = oldIndex;
   }
}

}

