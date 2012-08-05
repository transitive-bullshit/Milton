#ifndef OBJ_OBJ_HPP_INCLUDED
#define OBJ_OBJ_HPP_INCLUDED

#include <cstddef>
#include <boost/cstdint.hpp>

#include <boost/tr1/functional.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/tr1/utility.hpp>
#include <boost/tr1/type_traits.hpp>

namespace milton {

namespace obj {

   typedef std::size_t size_type;
   typedef std::ptrdiff_t index_type;
   typedef double float_type;
   typedef std::tr1::tuple<index_type, index_type> index_2_tuple_type;
   typedef std::tr1::tuple<index_type, index_type, index_type> index_3_tuple_type;

} // namespace obj

}

#endif // OBJ_OBJ_HPP_INCLUDED

