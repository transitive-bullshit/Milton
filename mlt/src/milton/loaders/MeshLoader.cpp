/**<!-------------------------------------------------------------------->
   @file   MeshLoader.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      static class which loads mesh data from external files in either OBJ 
   or PLY formats (file extensions '.obj' and '.ply' respectively)
   <!-------------------------------------------------------------------->**/

#include "MeshLoader.h"
#include "Mesh.h"

#include "MeshLoaderOBJ.h"
#include "MeshLoaderPLY.h"

#include <fstream>

namespace milton {

Mesh *MeshLoader::load(const std::string &fileName) {
   const std::string &suffix = fileName.substr(fileName.rfind('.') + 1);
   MeshData data;
   
   if (suffix == "obj" || suffix == "OBJ") {
      MeshLoaderOBJ loader(data);
      
      if (!loader.load(fileName, data))
         return false;
   } else if (suffix == "ply" || suffix == "PLY") {
      MeshLoaderPLY loader(data);
      
      if (!loader.load(fileName, data))
         return false;
   } else {
      cerr << "unable to infer mesh type '" << suffix << "' from file '" 
           << fileName << "'" << endl;
      
      return NULL;
   }
   
   return new Mesh(data);
}

bool MeshLoader::load(const std::string &fileName, MeshData &outData) {
   std::ifstream fileStream;
   fileStream.open(fileName.c_str());
   
   if (!fileStream.is_open()) {
      cerr << "error opening file '" << fileName << "'" << endl;
      
      return false;
   }
   
   outData.fileName = fileName;
   return _load(fileStream, outData);
}

bool MeshLoader::save(Mesh *mesh, const std::string &fileName) {
   std::ofstream out;
   ASSERT(mesh);
   
   { // attempt to open the given file
      const std::string &suffix = fileName.substr(fileName.rfind('.') + 1);
      if (suffix != "obj" && suffix != "OBJ") {
         cerr << "warning: saving mesh out to file in OBJ format, but requested filename '" << fileName << "' does not end with '.obj'" << endl;
      }
      
      out.open(fileName.c_str());
      
      if (!out.is_open()) {
         cerr << "error opening file '" << fileName << "'" << endl;
         return false;
      }
   }
   
   const unsigned noVertices  = mesh->getNoVertices();
   const unsigned noNormals   = mesh->getNoNormals();
   const unsigned noTriangles = mesh->getNoTriangles();
   
   const Vector3 *vertices = mesh->getVertices();
   const Vector3 *normals  = mesh->getNormals();
   const MeshTriangle *triangles = mesh->getTriangles();
   
   // print out a header, followed by vertices, normals, and faces
   out << "# OBJ mesh exported by the Milton rendering framework" << endl << endl;
   
   out << "# vertices" << endl;
   for(unsigned i = 0; i < noVertices; ++i)
      out << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << endl;
   out << endl;
   
   if (noNormals > 0) {
      out << "# normals" << endl;
      for(unsigned i = 0; i < noNormals; ++i)
         out << "vn " << normals[i][0] << " " << normals[i][1] << " " << normals[i][2] << endl;
      out << endl;
   }
   
   out << "# faces (all polygons have been triangulated for ease-of-use)" << endl;
   for(unsigned i = 0; i < noTriangles; ++i) {
      const MeshTriangle &t = triangles[i];
      // note: OBJ indexing starts at one, not zero
      
      out << "f ";
      for(unsigned j = 0; j < 3; ++j)
         out << (t.data[j] + 1) << "//" << (t.nData[j] + 1) << " ";
      out << endl;
   }
   out << endl;
   
   return true;
}

}

