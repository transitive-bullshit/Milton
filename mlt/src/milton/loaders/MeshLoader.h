/**<!-------------------------------------------------------------------->
   @class  MeshLoader
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      static class which loads mesh data from external files in either OBJ 
   or PLY formats (file extensions '.obj' and '.ply' respectively)
   <!-------------------------------------------------------------------->**/

#ifndef MESH_LOADER_H_
#define MESH_LOADER_H_

#include <common/common.h>
#include <istream>

namespace milton {

class  Mesh;
struct MeshData;

class MILTON_DLL_EXPORT MeshLoader {
   public:
      /**
       * @brief
       *    Attempts to parse and initialize a Mesh from the given file, 
       * inferring its file format from the filename's extension
       * 
       * @returns NULL on error or a valid Mesh otherwise
       */
      static Mesh *load(const std::string &fileName);
      
      /**
       * @brief
       *    Attempts to save the given mesh in OBJ format to the file given
       * 
       * @returns whether or not mesh was successfully written out to the file
       */
      static bool save(Mesh *mesh, const std::string &fileName);
      
      /**
       * @brief
       *    Format-specific mesh loading from a file which defers actual 
       * parsing to _load
       * 
       * @returns true on success with loaded data in outData, false otherwise
       */
      virtual bool load(const std::string &fileName, MeshData &outData);
      
   protected:
      /**
       * @brief
       *    Format-specific mesh loading from an ifstream (input file stream)
       * 
       * @returns true on success with loaded data in outData, false otherwise
       */
      virtual bool _load(std::istream &fileStream, MeshData &outData) = 0;
};

}

#endif // MESH_LOADER_H_

