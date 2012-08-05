/**<!-------------------------------------------------------------------->
   @file   SIMD.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Contains useful/common definitions for working with SSE intrinsics
   <!-------------------------------------------------------------------->**/

#ifndef SIMD_H_
#define SIMD_H_

#include <common/common.h>

#if MILTON_ENABLE_SSE

#include <xmmintrin.h> // SSE1
#include <emmintrin.h> // SSE2
#include <pmmintrin.h> // SSE3

namespace milton {

typedef __m64     m64_t;
typedef __m128    m128f_t;
typedef __m128i   m128i_t;
typedef __m128d   m128d_t;

// alignment utilities
#define SSE_ALIGN16_PRE       ALIGN_PRE(16)
#define SSE_ALIGN16_POST      ALIGN_POST(16)

#define DECLARE_ALIGNED_MEMORY_OPERATORS              \
   static inline void *operator new(size_t s) {       \
      return malloc_aligned((unsigned)s);             \
   }                                                  \
   static inline void *operator new[](size_t s) {     \
      return malloc_aligned((unsigned)s);             \
   }                                                  \
   static inline void operator delete(void* ptr) {    \
      free_aligned(ptr);                              \
   }                                                  \
   static inline void operator delete[](void* ptr) {  \
      free_aligned(ptr);                              \
   }

}

#include <memory>

namespace milton {

extern void *operator new  (size_t size) throw (std::bad_alloc);
extern void *operator new[](size_t size) throw (std::bad_alloc);

#ifdef __cplusplus
   extern "C" {  // turn off name mangling
#endif
      
      /**
       * @returns a pointer to a block of memory allocated with malloc which
       *    is aligned on a 16-byte boundary
       * @note returned memory should be freed with free_aligned
       * 
       * @see also _mm_malloc
       */
      extern void *malloc_aligned(unsigned n);
      
      /**
       * @brief
       *    frees the memory at the address given which is assumed to have 
       * been previously allocated with malloc_aligned
       * 
       * @see also _mm_free
       */
      extern void  free_aligned(void *ptr);
      
#ifdef __cplusplus
   }  // end of extern "C"
#endif


/**
 * @brief
 *    128-bit SSE (Streaming SIMD Extension) registers require 16-byte 
 * alignment which necessitates special care when allocating objects containing
 * SSE data types both on the stack and on the heap (via the new operator)
 * 
 * @note
 *    SSE has been <b>enabled</b> in this build of Milton
 */
struct MILTON_DLL_EXPORT SSE_ALIGN16_PRE SSEAligned {
   DECLARE_ALIGNED_MEMORY_OPERATORS
} SSE_ALIGN16_POST;

struct MILTON_DLL_EXPORT SSE_ALIGN16_PRE SimpleSSEVector : public SSEAligned {
   union {
      real_t  data[4];
      m128f_t vec;
      
      struct { real_t x, y, z, w; };
   };
   
   inline SimpleSSEVector(const m128f_t &v)
      : vec(v)
   { }
} SSE_ALIGN16_POST;


#define _mm_extract_epi32(x, imm)      \
   ((real_t)_mm_cvtsi128_si32(_mm_srli_si128((x), 4 * (imm))))

#define _mm_extract_f32i(vec, index)   \
   (SimpleSSEVector((vec)).data[(index)])

#define _mm_extract_f32(vec)           \
   (SimpleSSEVector((vec)).data[0])


#ifdef __cplusplus
   extern "C" {  // turn off name mangling
#endif
      
      /**
       * @returns fuzzy element-wise equality between @p a and @p b
       */
      static MILTON_DLL_EXPORT inline m128f_t SSE_EQ(const m128f_t &a, const m128f_t &b) {
         // could use the _mm_cmpeq_ps func here, but it has the same problems as 
         // comparing a float; so, see if a is roughly close to b... 
         const m128f_t &a_plus_a_bit = _mm_add_ps(a, _mm_set_ps1(EPSILON));
         const m128f_t &a_minus_a_bit = _mm_sub_ps(a, _mm_set_ps1(EPSILON));
         
         const m128f_t &b_lt_a = _mm_cmplt_ps(b, a_plus_a_bit);
         const m128f_t &b_gt_a = _mm_cmpgt_ps(b, a_minus_a_bit);
         
         return _mm_and_ps(b_lt_a, b_gt_a);
      }
      
      /**
       * @returns true iff @a and @b are approximately equal
       */
      static MILTON_DLL_EXPORT inline bool SSE_EQb(const m128f_t &a, const m128f_t &b) {
         const SimpleSSEVector v(_mm_cmpneq_ps(SSE_EQ(a, b), _mm_setzero_ps()));
         
         return (v.data[0] != 0 && v.data[1] != 0 && v.data[2] != 0 && v.data[3] != 0);
      }
      
      /**
       * @returns fuzzy element-wise inequality between @p a and @p b
       */
      static MILTON_DLL_EXPORT inline m128f_t SSE_NEQ(const m128f_t &a, const m128f_t &b) {
         // could use the _mm_cmpeq_ps func here, but it has the same problems as 
         // comparing a float; so, see if a is roughly close to b... 
         const m128f_t &a_plus_a_bit = _mm_add_ps(a, _mm_set_ps1(EPSILON));
         const m128f_t &a_minus_a_bit = _mm_sub_ps(a, _mm_set_ps1(EPSILON));
         
         const m128f_t &b_gt_a = _mm_cmpgt_ps(b, a_plus_a_bit);
         const m128f_t &b_lt_a = _mm_cmplt_ps(b, a_minus_a_bit);
         
         return _mm_or_ps(b_lt_a, b_gt_a);
      }
      
      /**
       * @returns true iff @a and @b are not approximately equal
       */
      static MILTON_DLL_EXPORT inline bool SSE_NEQb(const m128f_t &a, const m128f_t &b) {
         const SimpleSSEVector v(_mm_cmpneq_ps(SSE_EQ(a, b), _mm_setzero_ps()));
         
         return (v.data[0] == 0 || v.data[1] == 0 || v.data[2] == 0 || v.data[3] == 0);
      }
      
#ifdef __cplusplus
   }  // end of extern "C"
#endif

}

#else  // MILTON_ENABLE_SSE

namespace milton {

/**
 * @brief
 *    128-bit SSE (Streaming SIMD Extension) registers require 16-byte 
 * alignment which necessitates special care when allocating objects containing
 * SSE data types both on the stack and on the heap (via the new operator)
 * 
 * @note
 *    SSE has been <b>disabled</b> in this build of Milton, resulting in 
 * SSEAligned being just a dummy, placeholder class with no real functionality 
 * or effect
 */
struct MILTON_DLL_EXPORT SSEAligned { };

}

#endif // MILTON_ENABLE_SSE

#endif // SIMD_H_

