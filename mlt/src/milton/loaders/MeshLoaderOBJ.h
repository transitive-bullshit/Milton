/**<!-------------------------------------------------------------------->
   @class  MeshLoaderOBJ
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Loads OBJ meshes from an external source ('.obj' file extension).
   Wrapper around obj-parser-0.1
   <!-------------------------------------------------------------------->**/

#ifndef MESH_LOADER_OBJ_H_
#define MESH_LOADER_OBJ_H_

#include <loaders/MeshLoader.h>
#include <istream>

#include <third-party/objparser/obj.h>
#include <boost/tr1/functional.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/tr1/utility.hpp>
#include <boost/tr1/type_traits.hpp>
#include <boost/function.hpp>

#ifdef HAVE_CONFIG_H
#  include <third-party/objparser/obj/config.hpp>
#endif

namespace milton {

class MILTON_DLL_EXPORT MeshLoaderOBJ : public MeshLoader {
   public:
      inline MeshLoaderOBJ(MeshData &outData)
         : _data(outData)
      { }
      
   protected:
      virtual bool _load(std::istream &istream, MeshData &outData);
      
   private:
      void info_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      void warning_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      void error_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      void geometric_vertex_callback(obj::float_type x, obj::float_type y, obj::float_type z);
      void texture_vertex_callback(obj::float_type u, obj::float_type v);
      void vertex_normal_callback(obj::float_type x, obj::float_type y, obj::float_type z);
      void triangular_face_geometric_vertices_callback(obj::index_type v1, obj::index_type v2, obj::index_type v3);
      void triangular_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type& v1_vt1, const obj::index_2_tuple_type& v2_vt2, const obj::index_2_tuple_type& v3_vt3);
      void triangular_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type& v1_vn1, const obj::index_2_tuple_type& v2_vn2, const obj::index_2_tuple_type& v3_vn3);
      void triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type& v1_vt1_vn1, const obj::index_3_tuple_type& v2_vt2_vn2, const obj::index_3_tuple_type& v3_vt3_vn3);
      
   protected:
      MeshData &_data;
};

}

#endif // MESH_LOADER_OBJ_H_
