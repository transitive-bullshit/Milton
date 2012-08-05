/**<!-------------------------------------------------------------------->
   @file   utilities.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Summer 2008
   
   @brief
      Contains utility definitions used throughout all of my projects
   @note
      Some of these definitions assume the availability of GCC preprocessor 
   extensions
   <!-------------------------------------------------------------------->**/

#ifndef UTILITIES_H_
#define UTILITIES_H_

// Common includes
#include <string.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <string>

// -------------
// STL utilities
// -------------

/// Declare typedefs along with iterators and const_iterators
/// Ex:
///    DECLARE_STL_TYPEDEF(std::vector<Shape>, ShapeList);
/// =>
///    typedef std::vector<Shape>         ShapeList;
///    typedef ShapeList::iterator        ShapeListIter;
///    typedef ShapeList::connst_iterator ShapeListConnstIter;
#define DECLARE_STL_TYPEDEF(type, def)                \
   typedef type                    def;               \
   typedef def::iterator           def##Iter;         \
   typedef def::const_iterator     def##ConstIter

/// Use for stl types which contain a comma (commas are not allowed 
/// otherwise in preprocessor macro invocations)
#define DECLARE_STL_TYPEDEF2(type1, type2, def)       \
   typedef type1,type2             def;               \
   typedef def::iterator           def##Iter;         \
   typedef def::const_iterator     def##ConstIter

/// Use for stl types which contain two commas (commas are not allowed 
/// otherwise in preprocessor macro invocations)
#define DECLARE_STL_TYPEDEF3(typeP, type2, type3, def)\
   typedef type2,type2,type3       def;               \
   typedef def::iterator           def##Iter;         \
   typedef def::const_iterator     def##ConstIter

/// Iterate over all elements in an stl container
#define FOREACH(iterator_type, container, iter)       \
    for(iterator_type (iter) = (container).begin();   \
        (iter) != (container).end(); ++(iter))


// ---------------------------
// Error / debugging utilities
// ---------------------------

#ifdef MILTON_ARCH_UNIX
   // Debug printf; use just like printf
#  define dprintf(__s_, ...) {                                       \
      fprintf(stderr, "%s::%s (%d): ", __FILE__, __func__, __LINE__); \
      fprintf(stderr, (__s_), ## __VA_ARGS__);                        \
   }
#else
    // Debug printf; use just like printf
#  define dprintf(__s_, ...) {                                       \
      fprintf(stderr, "%s::%s (%d): ", __FILE__, __FUNCTION__, __LINE__); \
      fprintf(stderr, (__s_), ## __VA_ARGS__);                        \
   }
#endif

// Ensure debugging is enabled unless specifically turned off via NDEBUG
#if ((!defined(NDEBUG) && !defined(DEBUG)) || defined(_DEBUG))
#  define DEBUG (1)
#endif

#if (defined(DEBUG) || !defined(NDEBUG))
#  include <assert.h>
// Assertion macro which is only evaluated during debugging
#  define ASSERT(exp)                                    \
      do {                                               \
         if ((exp) == false) {                           \
            fprintf(stderr, "\n");                       \
            dprintf("Assertion FAILED: '" #exp "'\n\n"); \
            abort();                                     \
         }                                               \
      } while(0)
// Macro for not implemented functions
#  define NYI()                                          \
   do {                                                  \
      dprintf("Not Yet Implemented!");                   \
      ASSERT(0);                                         \
   } while(0)
#else
#  define ASSERT(exp)
#  define NYI()
#endif


// ----------------------
// Deallocation Utilities
// ----------------------

#define safeFree(p)  \
   do {              \
      if ((p)) {     \
         free((p));  \
         (p) = NULL; \
      }              \
   } while(0)

#define safeDelete(p)   \
   do {                 \
      if ((p)) {        \
         delete ((p));  \
         (p) = NULL;    \
      }                 \
   } while(0)

#define safeDeleteArray(p) \
   do {                    \
      if ((p)) {           \
         delete[] ((p));   \
         (p) = NULL;       \
      }                    \
   } while(0)


// ---------------------
// Common math utilities
// ---------------------

#ifdef MIN
   #undef MIN
#endif

#ifdef MAX
   #undef MAX
#endif

#ifdef MIN3
   #undef MIN3
#endif

#ifdef MAX3
   #undef MAX3
#endif

#ifdef ABS
   #undef ABS
#endif

#ifdef __GNUC__

#define MIN(x, y) ({                \
   const typeof (x) x_ = (x);       \
   const typeof (y) y_ = (y);       \
   ((x_ < y_) ? x_ : y_);           \
})

#define MAX(x, y) ({                \
   const typeof (x) _x_ = (x);      \
   const typeof (y) _y_ = (y);      \
   ((_x_ > _y_) ? _x_ : _y_);       \
})

#define MIN3(x, y, z) ({            \
   const typeof (x) x__ = (x);      \
   const typeof (y) y__ = (y);      \
   const typeof (z) z__ = (z);      \
   ((x__ < y__) ? (x__ < z__ ? x__ : z__) : (y__ < z__ ? y__ : z__)); \
})

#define MAX3(x, y, z) ({            \
   const typeof (x) __x = (x);      \
   const typeof (y) __y = (y);      \
   const typeof (z) __z = (z);      \
   ((__x > __y) ? (__x > __z ? __x: __z) : (__y > __z ? __y : __z)); \
})

//#define MAXN(a, b, ...)   MAXN(MAX(a, b), ## __VA_ARGS__)

#define ABS(x) ({                   \
   register const typeof(x) ___tx = (x);  \
   (___tx >= 0 ? ___tx : -___tx);   \
})

// clamps val within the inclusive interval (min, max), assuming max >= min
#define CLAMP(val, min, max) ({             \
   const typeof (val) val__ = (val);      \
   const typeof (min) min__ = (min);      \
   const typeof (max) max__ = (max);      \
   ((val__ >= max__) ? max__ : (val__ <= min__ ? min__ : val__)); \
})

#else

#define MIN(x, y) (                         \
   std::min((x), (y))                       \
)

#define MAX(x, y) (                         \
   std::max((x), (y))                       \
)

#define MIN3(x, y, z) (                     \
   std::min(std::min((x), (y)), (z))        \
)

#define MAX3(x, y, z) (                     \
   std::max(std::max((x), (y)), (z))        \
)

#define ABS(x) (                            \
   std::abs((x))                            \
)

// TODO:  "optimize CLAMP on Windows"

// clamps val within the inclusive interval (min, max), assuming max >= min
#define CLAMP(val, min, max) (              \
   ((val >= max) ? max : (val <= min ? min : val)) \
)

#endif

#define CLAMPMAX(val,max)    CLAMP((val), 0, (max))
#define CLAMPMIN(val,min)    CLAMP((val), (min), 1)
#define CLAMP01(val)         CLAMP((val), 0, 1)


//#define EQ(a, b)  (fabs((a) - (b)) < EPSILON)
// check absolute error as well as relative error
#define EQ(a, b)  (fabs((a) - (b)) < EPSILON || (fabs((a) - (b) / ((fabs((b)) > fabs((a))) ? b : a)) <= EPSILON))
#define NEQ(a, b) (!(EQ(a, b)))

#define DEGREES_TO_RADIANS(d) ((d) * (M_PI / 180.0))
#define RADIANS_TO_DEGREES(r) ((r) * (180.0 / M_PI))


//--------------------------------------
// Utilities geared towards optimization
//--------------------------------------

/**
 * A pure function is a function that has no side effects, whose 
 * return value depends soley on the values of its arguments.
 * Multiple calls to a pure function with the same arguments can 
 * be optimized in subexpressions because they are guaranteed to 
 * produce the same result. (available on Linux only)
 * 
 * "Optimizing Software in C++", Agner Fog, 2008
 */
#ifdef __GNUC__
#  define PURE_FUNCTION __attribute__((const))
#else
#  define PURE_FUNCTION
#endif


//----------------------------------------
// Helper macros to build member functions
//----------------------------------------

/* Ex usage: GET_SET(int, error, Error) ->
 *    void setError(int);
 *    int getError();
 *    int m_error;
 */
#define GET_SET(type, l, L)  \
   GET(type, l, L)           \
   SET(type, l, L)           \
   MEMBER(type, l)

/* Ex Usage: GET_SET_BOOL(is, Working) ->
 *    void setWorking(bool);
 *    bool getWorking();
 *    bool m_isWorking;
 */
#define GET_SET_BOOL(pre, L) \
   GET_BOOL(pre, L)          \
   SET_BOOL(pre, L)          \
   MEMBER(bool, pre ## L)

#define MEMBER(type, l)  \
   protected:            \
      type m_##l;        \
   public:

#define GET(type, l, L)  \
   GET_GENERIC(type, l, get##L)

#define SET(type, l, L)  \
   SET_GENERIC(type, l, L)

#define GET_BOOL(pre, L) \
   GET_GENERIC(bool, pre ## L, pre ## L)

#define SET_BOOL(pre, L) \
   SET_GENERIC(bool, pre ## L, L)

#define GET_GENERIC(type, l, L)     \
   public:                          \
      inline type L() const __attribute__((always_inline)) { \
         return m_##l;              \
      }

#define SET_GENERIC(type, l, L)     \
   public:                          \
      inline void set##L(type newVal) __attribute__((always_inline)) { \
         m_##l = newVal;            \
      }

#endif // UTILITIES_H_

