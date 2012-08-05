/* ---------------------------------------------- *\
   file: sort.h
   auth: Travis Fischer
   acct: tfischer
   date: 3/18/2007
\* ---------------------------------------------- */

#ifndef SORT_H_
#define SORT_H_

namespace milton {

// Note: 'b' is a scratch array of size 'n' and should be allocated by the 
//        caller
// Note: 'indices' in radixSortIndices is expected to be initialized properly 
//        by the caller

template <typename T>
void radixSort(T *a, T *b, const unsigned int n);

template <typename T>
void radixSortIndices(T *a, unsigned int *indices, unsigned int *b, const unsigned int n);

template <typename T>
void quickSort(T *a, const int p, const int q);

}

#endif // SORT_H_

