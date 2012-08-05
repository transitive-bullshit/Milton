/**<!-------------------------------------------------------------------->
   @file   MeshLoaderPLY.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Loads PLY meshes from an external source ('.ply' file extension).
   Wrapper around ply-parser-0.1
   <!-------------------------------------------------------------------->**/

#include "MeshLoaderPLY.h"
#include "MeshTriangle.h"
#include "Mesh.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

namespace milton {

bool MeshLoaderPLY::_load(std::istream &istream, MeshData &data) {
   _data = data;
   
   ply::ply_parser::flags_type ply_parser_flags = 1; // triangulate??
   ply::ply_parser ply_parser(ply_parser_flags);
   
   ply_parser.info_callback(std::tr1::bind(&MeshLoaderPLY::info_callback, this, data.fileName, _1, _2));
   ply_parser.warning_callback(std::tr1::bind(&MeshLoaderPLY::warning_callback, this, data.fileName, _1, _2));
   ply_parser.error_callback(std::tr1::bind(&MeshLoaderPLY::error_callback, this, data.fileName, _1, _2));
   
   ply_parser.element_definition_callback(std::tr1::bind(&MeshLoaderPLY::element_definition_callback, this, _1, _2));
   
   ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
   ply::at<ply::float32>(scalar_property_definition_callbacks) = std::tr1::bind(&MeshLoaderPLY::scalar_property_definition_callback, this, _1, _2);
   ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);
   
   ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
   ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = std::tr1::bind(&MeshLoaderPLY::list_property_definition_callback, this, _1, _2);
   ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);
   
   return ply_parser.parse(istream);
}

void MeshLoaderPLY::info_callback(const std::string &filename, 
                                  std::size_t line_number, 
                                  const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "info: " << message << std::endl;
}

void MeshLoaderPLY::warning_callback(const std::string &filename, 
                                     std::size_t line_number, 
                                     const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "warning: " << message << std::endl;
}

void MeshLoaderPLY::error_callback(const std::string &filename, 
                                   std::size_t line_number, 
                                   const std::string &message)
{
   std::cerr << filename << ":" << line_number << ": " << "error: " << message << std::endl;
}

std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> > 
MeshLoaderPLY::element_definition_callback(const std::string& element_name, 
                                           std::size_t count)
{
   if (element_name == "vertex") {
      return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(
         std::tr1::bind(&MeshLoaderPLY::vertex_begin, this),
         std::tr1::bind(&MeshLoaderPLY::vertex_end, this)
         );
   }
   else if (element_name == "face") {
      return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(
         std::tr1::bind(&MeshLoaderPLY::face_begin, this),
         std::tr1::bind(&MeshLoaderPLY::face_end, this)
         );
   }
   else {
      return std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> >(0, 0);
   }
}

std::tr1::function<void (ply::float32)> 
MeshLoaderPLY::scalar_property_definition_callback(const std::string& element_name, 
                                                   const std::string& property_name)
{
   if (element_name == "vertex") {
      if (property_name == "x") {
         return std::tr1::bind(&MeshLoaderPLY::vertex_x, this, _1);
      }
      else if (property_name == "y") {
         return std::tr1::bind(&MeshLoaderPLY::vertex_y, this, _1);
      }
      else if (property_name == "z") {
         return std::tr1::bind(&MeshLoaderPLY::vertex_z, this, _1);
      }
      else {
         return 0;
      }
   }
   else {
      return 0;
   }
}

std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> > 
MeshLoaderPLY::list_property_definition_callback(const std::string& element_name, 
                                                 const std::string& property_name)
{
   if ((element_name == "face") && (property_name == "vertex_indices")) {
      return std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> >(
         std::tr1::bind(&MeshLoaderPLY::face_vertex_indices_begin, this, _1),
         std::tr1::bind(&MeshLoaderPLY::face_vertex_indices_element, this, _1),
         std::tr1::bind(&MeshLoaderPLY::face_vertex_indices_end, this)
         );
   }
   else {
      return std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> >(0, 0, 0);
   }
}

void MeshLoaderPLY::vertex_begin()
{ }

void MeshLoaderPLY::vertex_x(ply::float32 x)
{
   m_x = x;
}

void MeshLoaderPLY::vertex_y(ply::float32 y)
{
   m_y = y;
}

void MeshLoaderPLY::vertex_z(ply::float32 z)
{
   m_z = z;
}

void MeshLoaderPLY::vertex_end() {
   //(*ostream_) << "v " << vertex_x_ << " " << vertex_y_ << " " << vertex_z_ << "\n";
   
   _data.vertices.push_back(Vertex(m_x, m_y, m_z));
}

void MeshLoaderPLY::face_begin()
{ }

void MeshLoaderPLY::face_vertex_indices_begin(ply::uint8 size) {
   m_faceIndex = 0;
}

void MeshLoaderPLY::face_vertex_indices_element(ply::int32 vertex_index) {
   if (m_faceIndex == 0) {
      m_firstFaceIndex = vertex_index;
   } else {
      if (m_faceIndex > 1) {
         // completed another triangle face
         _data.triangles.push_back(
            MeshTriangle(m_firstFaceIndex, m_prevFaceIndex, vertex_index));
      }
      
      m_prevFaceIndex = vertex_index;
   }
   
   ++m_faceIndex;
}

void MeshLoaderPLY::face_vertex_indices_end() 
{ }

void MeshLoaderPLY::face_end() 
{ }

}

