/**<!-------------------------------------------------------------------->
   @class  MeshLoaderPLY
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Loads PLY meshes from an external source ('.ply' file extension).
   Wrapper around ply-parser-0.1
   <!-------------------------------------------------------------------->**/

#ifndef MESH_LOADER_PLY_H_
#define MESH_LOADER_PLY_H_

#include <loaders/MeshLoader.h>
#include <istream>

#include <third-party/plyparser/ply.h>
#include <boost/tr1/functional.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/tr1/utility.hpp>
#include <boost/tr1/type_traits.hpp>
#include <boost/function.hpp>

#ifdef HAVE_CONFIG_H
#  include <third-party/plyparser/ply/config.hpp>
#endif

using namespace std::tr1::placeholders;

namespace milton {

class MILTON_DLL_EXPORT MeshLoaderPLY : public MeshLoader {
   public:
      inline MeshLoaderPLY(MeshData &outData)
         : _data(outData)
      { }
   
   protected:
      virtual bool _load(std::istream &istream, MeshData &outData);
      
   private:
      void info_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      void warning_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      void error_callback(const std::string& filename, std::size_t line_number, const std::string& message);
      
      std::tr1::tuple<std::tr1::function<void()>, std::tr1::function<void()> > element_definition_callback(const std::string& element_name, std::size_t count);
      
      std::tr1::function<void (ply::float32)> scalar_property_definition_callback(const std::string& element_name, const std::string& property_name);
      
      std::tr1::tuple<std::tr1::function<void (ply::uint8)>, std::tr1::function<void (ply::int32)>, std::tr1::function<void ()> > list_property_definition_callback(const std::string& element_name, const std::string& property_name);
      
      void vertex_begin();
      void vertex_x(ply::float32 x);
      void vertex_y(ply::float32 y);
      void vertex_z(ply::float32 z);
      void vertex_end();
      void face_begin();
      void face_vertex_indices_begin(ply::uint8 size);
      void face_vertex_indices_element(ply::int32 vertex_index);
      void face_vertex_indices_end();
      void face_end();
      
   protected:
      MeshData &_data;
      
      real_t m_x, m_y, m_z;
      unsigned m_faceIndex, m_prevFaceIndex, m_firstFaceIndex;
};

}

#endif // MESH_LOADER_PLY_H_

