/**<!-------------------------------------------------------------------->
   @class  MarchingCubes
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2008
   
   @brief
      Marching Cubes is a well-known algorithm for generating a polygonal 
   approximation to an isosurface defined over a scalar field. This 
   class is a fairly standard implementation of Marching Cubes in 3D that 
   generates triangular meshes as output.
   
   References:
      http://www.ia.hiof.no/~borres/cgraph/explain/marching/p-march.html
   <!-------------------------------------------------------------------->**/

#ifndef MARCHING_CUBES_H_
#define MARCHING_CUBES_H_

#include "shapes/ScalarField.h"
#include "shapes/MeshTriangle.h"

namespace milton {

class  Mesh;
struct MCData;

class MILTON_DLL_EXPORT MarchingCubes : public SSEAligned {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MarchingCubes()
         : m_resolution(256, 256, 256)
      { }
      
      inline MarchingCubes(const Vector3 &resolution)
         : m_resolution(resolution)
      { }
      
      virtual ~MarchingCubes()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Generates a triangular mesh approximating a specific isocontour
       * defined over the given ScalarField at the threshold value specified
       * 
       * @note
       *    the generated mesh is not guaranteed to be closed
       * 
       * @returns the generated Mesh or NULL upon error
       */
      virtual Mesh *polygonize(const ScalarField &field, real_t threshold);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the resolution of the grid used for marching squares
       */
      inline const Vector3 &getResolution() const {
         return m_resolution;
      }
      
      /**
       * @brief
       *    sets the resolution of the grid used for marching squares
       */
      inline void setResolution(const Vector3 &resolution) {
         m_resolution = resolution;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Vertex _calculateIntersection(const MCData &data, unsigned edge) const;
      
      inline unsigned _getVertexID(const MCData &data, unsigned edge) const;
      inline unsigned _getVertexID(const MCData &data) const;
      inline unsigned _getVertexID(unsigned i, unsigned j, unsigned k, 
                                   unsigned width, unsigned height) const;
      
      inline Vertex _interpolate(const Vertex &v1, const Vertex &v2, 
                                 real_t coeff1, real_t coeff2, 
                                 real_t threshold) const;
      
      void _addVertex(const Vertex &vertex, unsigned id, MCData &data) const;
      
      void _cleanup(MCData &data) const;
      
   protected:
      Vector3 m_resolution;
};

}

#endif // MARCHING_CUBES_H_

