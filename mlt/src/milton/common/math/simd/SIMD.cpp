/**<!-------------------------------------------------------------------->
   @file   SIMD.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Contains useful/common definitions for working with SSE intrinsics
   <!-------------------------------------------------------------------->**/

#include "SIMD.h"

#if MILTON_ENABLE_SSE

#include <stdlib.h>

namespace milton {

void *malloc_aligned(unsigned n) {
   return _mm_malloc(n, 16);
   /*// allocate memory + 16 bytes incase we need to align
   unsigned char *_ptr = (unsigned char*)malloc(n + 16);
   
   // how many btyes do we need to offset the address by?
   const unsigned offset = 16U - (((unsigned)_ptr) & 15U);
   
   // offset by the required amount s.t. _ptr is now 16-byte aligned
   _ptr += offset;
   
   // in the byte before the aligned address, store the offset
   _ptr[-1] = (unsigned char)offset;
   return _ptr;*/
}

void free_aligned(void *ptr) {
   if (ptr) {
      _mm_free(ptr);
      /*unsigned char *_ptr = (unsigned char*)ptr;
      
      // _ptr[-1] gets the offset we stored earlier. We subtract that
      // from the address passed to this func to move back to the address
      // originally returned from malloc. 
      free(_ptr - _ptr[-1]);*/
   }
}

void *operator new(size_t size) throw (std::bad_alloc) {
   return malloc_aligned(size);
}

void *operator new[](size_t size) throw (std::bad_alloc) {
   return malloc_aligned(size);
}

}

#endif // MILTON_ENABLE_SSE

