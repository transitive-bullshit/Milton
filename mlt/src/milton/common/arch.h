/**<!-------------------------------------------------------------------->
   @file   arch.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Contains architechure-specific definitions
   <!-------------------------------------------------------------------->**/

#ifndef ARCH_H_ 
#define ARCH_H_

// note:  the order of includes is important / delicate, especially on windows
#include <stdlib.h>

#if (defined(WIN32) || defined(_WIN32) || defined(_MSC_VER) || defined(_WINDOWS) || defined(_WINDOWS_) || defined(WINDOWS))
// this define reduces build times *significantly* on windows
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN (1)
#  endif
#  include <windows.h>
#endif

#include <boost/static_assert.hpp>
#include <limits>

//#define MILTON_SINGLE_PRECISION (1)
#define MILTON_DOUBLE_PRECISION (1)

//#define MILTON_ENABLE_SSE (1)

#ifdef INFINITY
#  undef  INFINITY
#endif

// define size_t type
#if !defined(_SIZE_T_DEFINED) && defined(__GNUC__)
#  include "stddef.h"
#endif


#if defined(__GNUC__)
#  define MILTON_ARCH_UNIX (1)
#  include <inttypes.h>
#  include <values.h>

   // g++ / Linux
//#  define ALIGN_PRE(n) __attribute__ ((aligned((n))))
#  define ALIGN_PRE(n)
#  define ALIGN_POST(n) __attribute__ ((aligned((n))))
#elif defined(_WIN16) || defined(__MSDOS__) || defined(_MSDOS)
#  define MILTON_ARCH_WINDOWS (1)

#  warning "untested..."
#  warning "alignment properties of this compiler unknown; SSE implementation " \
           "may be unstable or slow"
   
   // 16 bit systems use long int for 32 bit integer
   typedef   signed long int        int32_t;
   typedef unsigned long int        uint32_t;
    
#  define ALIGN_PRE(n)
#  define ALIGN_POST(n)
#elif defined(_MSC_VER) || defined(_WINDOWS_)
#  define MILTON_ARCH_WINDOWS (1)
   
   // Microsoft has their own builtin types
   typedef signed    __int32        int32_t;
   typedef unsigned  __int32        uint32_t;
   typedef signed    __int64        int64_t;
   typedef unsigned  __int64        uint64_t;
   
   // Visual C++ / Windows
#  define ALIGN_PRE(n)  __declspec(align((n)))
#  define ALIGN_POST(n)
#else
#  define MILTON_ARCH_UNKNOWN (1)
#  warning "alignment properties of this compiler unknown; SSE implementation " \
           "may be unstable or slow"

   // This works with most compilers
   typedef signed int               int32_t;
   typedef unsigned int             uint32_t;
   typedef long long                int64_t;
   typedef unsigned long long       uint64_t;
   
#  define ALIGN_PRE(n)
#  define ALIGN_POST(n)
#endif


namespace milton {


#if MILTON_SINGLE_PRECISION
#  if MILTON_DOUBLE_PRECISION
#     error "floating-point precision cannot be both single and double"
#  endif // MILTON_DOUBLE_PRECISION
   
   /// consistent floating-point type used pervasively throughout Milton
   typedef float  real_t;
   
#  define glVertex3real_t        glVertex3f
#  define glVertex3real_tv       glVertex3fv
#  define glColor3real_t         glColor3f
#  define glColor3real_tv        glColor3fv
#  define glNormal3real_t        glNormal3f
#  define glNormal3real_tv       glNormal3fv
#  define glTexCoord2real_t      glTexCoord2f
#  define glTexCoord2real_tv     glTexCoord2fv
#  define glMultMatrixreal_t     glMultMatrixf
#  define glLoadMatrixreal_t     glLoadMatrixf
   
//#  define INFINITY               (FLT_MAX)
#  define EPSILON                (1e-5f)

#elif MILTON_DOUBLE_PRECISION
   
   /// consistent floating-point type used pervasively throughout Milton
   typedef double real_t;
   
#  define glVertex3real_t        glVertex3d
#  define glVertex3real_tv       glVertex3dv
#  define glColor3real_t         glColor3d
#  define glColor3real_tv        glColor3dv
#  define glNormal3real_t        glNormal3d
#  define glNormal3real_tv       glNormal3dv
#  define glTexCoord2real_t      glTexCoord2d
#  define glTexCoord2real_tv     glTexCoord2dv
#  define glMultMatrixreal_t     glMultMatrixd
#  define glLoadMatrixreal_t     glLoadMatrixd
   
//#  define INFINITY               (DBL_MAX)
//#  define INFINITY               (HUGE_VAL)
//#  define INFINITY               (std::numeric_limits<real_t>::max())
#  define INFINITY               (std::numeric_limits<real_t>::infinity())
#  define EPSILON                (1e-7)
   
#  if MILTON_ENABLE_SSE
#     warning "SSE is currently only supported when compiled with " \
              "single-precision floating-point"
#     undef MILTON_ENABLE_SSE
#     define MILTON_ENABLE_SSE (0)
#  endif // MILTON_ENABLE_SSE
#else
#  error "floating-point precision must be either single or double"
#endif

#define create_real(x)   (static_cast<real_t>(x))

#define M_PI_DIV_2 (M_PI_2)
#define M_INV_PI   (M_1_PI)
#define M_INV_2PI  (M_2_PI)
#define M_2PI      (2.0 * M_PI)

// assumptions made throughout Milton
BOOST_STATIC_ASSERT(sizeof(unsigned int) == sizeof(int));
BOOST_STATIC_ASSERT(sizeof(unsigned int) == sizeof(float));

// 32-bit machine
BOOST_STATIC_ASSERT(std::numeric_limits<unsigned int>::digits == 32);
BOOST_STATIC_ASSERT(std::numeric_limits<unsigned long long>::digits == 64);


// TODO: how to get num processors / amount of RAM / etc..


#ifdef MILTON_ARCH_UNIX
#  define MILTON_DLL_EXPORT 
#else MILTON_ARCH_WINDOWS
#  ifdef MILTON_EXPORTS // building MiltonCore.dll
#     define MILTON_DLL_EXPORT __declspec(dllexport)
// disable warnings for DLL exports requiring that external code has a DLL interface
#     pragma warning(disable : 4251)
#  else
#     define MILTON_DLL_EXPORT __declspec(dllimport)
#  endif
#  define _USE_MATH_DEFINES

#  ifdef min
#     undef min
#  endif
#  ifdef max
#     undef max
#  endif

// disable warnings for conversions which are valid with g++
#  pragma warning(disable : 4244)

#endif

}

#include <common/math/simd/SIMD.h>

#endif // ARCH_H_
