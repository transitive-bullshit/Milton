/**<!-------------------------------------------------------------------->
   @file   MeshLoaderOBJ.h
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Loads OBJ meshes from an external source ('.obj' file extension).
   Wrapper around obj-parser-0.1
   <!-------------------------------------------------------------------->**/

#include "MeshLoaderOBJ.h"
#include <MeshTriangle.h>
#include <Mesh.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

namespace milton {

bool MeshLoaderOBJ::_load(std::istream &istream, MeshData &data) {
   using namespace std::tr1::placeholders;
   
   _data = data;
   
   obj::obj_parser::flags_type obj_parser_flags = 0;
   obj_parser_flags |= obj::obj_parser::parse_blank_lines_as_comment;
   obj_parser_flags |= obj::obj_parser::triangulate_faces;
   obj_parser_flags |= obj::obj_parser::translate_negative_indices;
   
   obj::obj_parser obj_parser(obj_parser_flags);
   
   obj_parser.info_callback(std::tr1::bind(&MeshLoaderOBJ::info_callback, this, data.fileName, _1, _2));
   obj_parser.warning_callback(std::tr1::bind(&MeshLoaderOBJ::warning_callback, this, data.fileName, _1, _2));
   obj_parser.error_callback(std::tr1::bind(&MeshLoaderOBJ::error_callback, this, data.fileName, _1, _2));
   
   obj_parser.geometric_vertex_callback(std::tr1::bind(&MeshLoaderOBJ::geometric_vertex_callback, this, _1, _2, _3));
   obj_parser.texture_vertex_callback(std::tr1::bind(&MeshLoaderOBJ::texture_vertex_callback, this, _1, _2));
   obj_parser.vertex_normal_callback(std::tr1::bind(&MeshLoaderOBJ::vertex_normal_callback, this, _1, _2, _3));
   obj_parser.face_callbacks(
      std::tr1::bind(&MeshLoaderOBJ::triangular_face_geometric_vertices_callback, this, _1, _2, _3),
      std::tr1::bind(&MeshLoaderOBJ::triangular_face_geometric_vertices_texture_vertices_callback, this, _1, _2, _3),
      std::tr1::bind(&MeshLoaderOBJ::triangular_face_geometric_vertices_vertex_normals_callback, this, _1, _2, _3),
      std::tr1::bind(&MeshLoaderOBJ::triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback, this, _1, _2, _3),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::quadrilateral_face_geometric_vertices_callback, this, _1, _2, _3, _4),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::quadrilateral_face_geometric_vertices_texture_vertices_callback, this, _1, _2, _3, _4),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::quadrilateral_face_geometric_vertices_vertex_normals_callback, this, _1, _2, _3, _4),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::quadrilateral_face_geometric_vertices_texture_vertices_vertex_normals_callback, this, _1, _2, _3, _4),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_begin_callback, this, _1, _2, _3),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_vertex_callback, this, _1),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_end_callback, this),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_begin_callback, this, _1, _2, _3),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_vertex_callback, this, _1),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_end_callback, this),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_vertex_normals_begin_callback, this, _1, _2, _3),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_vertex_normals_vertex_callback, this, _1),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_vertex_normals_end_callback, this),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_begin_callback, this, _1, _2, _3),
      NULL, //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_vertex_callback, this, _1),
      NULL //std::tr1::bind(&MeshLoaderOBJ::polygonal_face_geometric_vertices_texture_vertices_vertex_normals_end_callback, this)
         );
   /*obj_parser.group_name_callback(std::tr1::bind(&MeshLoaderOBJ::group_name_callback, this, _1));
   obj_parser.smoothing_group_callback(std::tr1::bind(&MeshLoaderOBJ::smoothing_group_callback, this, _1));
   obj_parser.object_name_callback(std::tr1::bind(&MeshLoaderOBJ::object_name_callback, this, _1));
   obj_parser.material_library_callback(std::tr1::bind(&MeshLoaderOBJ::material_library_callback, this, _1));
   obj_parser.material_name_callback(std::tr1::bind(&MeshLoaderOBJ::material_name_callback, this, _1));
   obj_parser.comment_callback(std::tr1::bind(&MeshLoaderOBJ::comment_callback, this, _1));*/
   
   return obj_parser.parse(istream);
}

void MeshLoaderOBJ::info_callback(const std::string &filename, 
                                  std::size_t line_number, 
                                  const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "info: " << message << std::endl;
}

void MeshLoaderOBJ::warning_callback(const std::string &filename, 
                                     std::size_t line_number, 
                                     const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "warning: " << message << std::endl;
}

void MeshLoaderOBJ::error_callback(const std::string &filename, 
                                   std::size_t line_number, 
                                   const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "error: " << message << std::endl;
}

void MeshLoaderOBJ::geometric_vertex_callback(obj::float_type x, 
                                              obj::float_type y, 
                                              obj::float_type z)
{
   //(*ostream_) << "v " << x << " " << y << " " << z << "\n";
   _data.vertices.push_back(Vertex(x, y, z));
}

void MeshLoaderOBJ::texture_vertex_callback(obj::float_type u, 
                                            obj::float_type v)
{
   //(*ostream_) << "vt " << u << " " << v << "\n";
   _data.uvs.push_back(UV(u, v));
}

void MeshLoaderOBJ::vertex_normal_callback(obj::float_type x, 
                                           obj::float_type y, 
                                           obj::float_type z)
{
   //(*ostream_) << "vn " << x << " " << y << " " << z << "\n";
   _data.normals.push_back(Vector3(x, y, z));
}

void MeshLoaderOBJ::triangular_face_geometric_vertices_callback(obj::index_type v1, 
                                                                obj::index_type v2, 
                                                                obj::index_type v3)
{
   //(*ostream_) << "f " << v1 << " " << v2 << " " << v3 << "\n";
   _data.triangles.push_back(MeshTriangle(v1 - 1, v2 - 1, v3 - 1));
   
   ASSERT(_data.triangles[_data.triangles.size() - 1].A == (unsigned) v1 - 1);
   ASSERT(_data.triangles[_data.triangles.size() - 1].B == (unsigned) v2 - 1);
   ASSERT(_data.triangles[_data.triangles.size() - 1].C == (unsigned) v3 - 1);
}

void MeshLoaderOBJ::triangular_face_geometric_vertices_texture_vertices_callback(const obj::index_2_tuple_type &v1_vt1, 
                                                                                 const obj::index_2_tuple_type &v2_vt2, 
                                                                                 const obj::index_2_tuple_type &v3_vt3)
{
   //texture_vertex_callback(tr1::get<0>(v1_vt1), tr1::get<0>(v2_vt2), tr1::get<0>(v3_vt3));
   //triangular_face_geometric_vertices_callback(tr1::get<0>(v1_vt1), tr1::get<0>(v2_vt2), tr1::get<0>(v3_vt3));
   NYI();
   
   _data.triangles.push_back(MeshTriangle(tr1::get<0>(v1_vt1) - 1, tr1::get<0>(v2_vt2) - 1, tr1::get<0>(v3_vt3) - 1, 
                                          0, 0, 0, 
                                          tr1::get<1>(v1_vt1) - 1, tr1::get<1>(v2_vt2) - 1, tr1::get<1>(v3_vt3) - 1));
   
   ASSERT(_data.triangles[_data.triangles.size() - 1].A == (unsigned) tr1::get<0>(v1_vt1) - 1);
   ASSERT(_data.triangles[_data.triangles.size() - 1].B == (unsigned) tr1::get<0>(v2_vt2) - 1);
   ASSERT(_data.triangles[_data.triangles.size() - 1].C == (unsigned) tr1::get<0>(v2_vt2) - 1);
}

void MeshLoaderOBJ::triangular_face_geometric_vertices_vertex_normals_callback(const obj::index_2_tuple_type &v1_vn1, 
                                                                               const obj::index_2_tuple_type &v2_vn2, 
                                                                               const obj::index_2_tuple_type &v3_vn3)
{
   _data.triangles.push_back(MeshTriangle(tr1::get<0>(v1_vn1) - 1, tr1::get<0>(v2_vn2) - 1, tr1::get<0>(v3_vn3) - 1, 
                                          tr1::get<1>(v1_vn1) - 1, tr1::get<1>(v2_vn2) - 1, tr1::get<1>(v3_vn3) - 1));
}

void MeshLoaderOBJ::triangular_face_geometric_vertices_texture_vertices_vertex_normals_callback(const obj::index_3_tuple_type &v1_vt1_vn1, 
                                                                                                const obj::index_3_tuple_type &v2_vt2_vn2, 
                                                                                                const obj::index_3_tuple_type &v3_vt3_vn3)
{
   _data.triangles.push_back(MeshTriangle(tr1::get<0>(v1_vt1_vn1) - 1, tr1::get<0>(v2_vt2_vn2) - 1, tr1::get<0>(v3_vt3_vn3) - 1, 
                                          tr1::get<2>(v1_vt1_vn1) - 1, tr1::get<2>(v2_vt2_vn2) - 1, tr1::get<2>(v3_vt3_vn3) - 1, 
                                          tr1::get<1>(v1_vt1_vn1) - 1, tr1::get<1>(v2_vt2_vn2) - 1, tr1::get<1>(v3_vt3_vn3) - 1));
}

}

