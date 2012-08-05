/**<!-------------------------------------------------------------------->
   @file   MarchingCubes.cpp
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

#include "MarchingCubes.h"
#include "Mesh.h"

#include "MarchingCubesData.inl"

namespace milton {

DECLARE_STL_TYPEDEF2(std::map<unsigned, unsigned>, VertexIDMap);

struct CubeVertex {
   real_t value;
};

struct MCData {
   MeshData meshData;
   
   unsigned width;
   unsigned height;
   unsigned depth;
   
   real_t   xScale;
   real_t   yScale;
   real_t   zScale;
   
   real_t   threshold;
   
   unsigned i;
   unsigned j;
   unsigned k;
   
   CubeVertex *grid;
   VertexIDMap vertexIDMap;
   
   inline const CubeVertex &cell(unsigned i, unsigned j, unsigned k) const {
      return grid[(k * height + j) * width + i];
   }
   
   inline CubeVertex &cell(unsigned i, unsigned j, unsigned k) {
      return grid[(k * height + j) * width + i];
   }
};

Mesh *MarchingCubes::polygonize(const ScalarField &field, real_t threshold) {
   MCData data;
   
   const AABB &aabb = field.getAABB();
   data.width   = (unsigned) m_resolution[0];
   data.height  = (unsigned) m_resolution[1];
   data.depth   = (unsigned) m_resolution[2];
   
   ASSERT(data.width  > 1);
   ASSERT(data.height > 1);
   ASSERT(data.depth  > 1);
   
   const Vector3 &dimensions = aabb.max - aabb.min;
   data.xScale = dimensions[0] / (data.width  - 1);
   data.yScale = dimensions[1] / (data.height - 1);
   data.zScale = dimensions[2] / (data.depth  - 1);
   Point3 point;
   
   data.threshold = threshold;
   data.grid      = (CubeVertex*)
      malloc(data.width * data.height * data.depth * sizeof(CubeVertex));
   
   // precompute field values at vertices
   for(unsigned i = data.width; i--;) {
      point[0] = aabb.min[0] + i * data.xScale;
      
      for(unsigned j = data.height; j--;) {
         point[1] = aabb.min[1] + j * data.yScale;
         
         for(unsigned k = data.depth; k--;) {
            point[2] = aabb.min[2] + k * data.zScale;
            
            data.cell(i, j, k).value = field.evaluate(point);
            //cout << data.cell(i, j, k).value << endl;
         }
      }
   }
   
   // march over each square, creating triangles on a case-by-case basis
   for(unsigned i = data.width - 1; i--;) {
      data.i = i;
      
      for(unsigned j = data.height - 1; j--;) {
         data.j = j;
         
         for(unsigned k = data.depth - 1; k--;) {
            data.k = k;
            
            const unsigned mcCase = 
               ((1 << 0) * (data.cell(i + 0, j + 0, k + 0).value <= threshold)) | 
               ((1 << 1) * (data.cell(i + 0, j + 1, k + 0).value <= threshold)) | 
               ((1 << 2) * (data.cell(i + 1, j + 1, k + 0).value <= threshold)) | 
               ((1 << 3) * (data.cell(i + 1, j + 0, k + 0).value <= threshold)) | 
               ((1 << 4) * (data.cell(i + 0, j + 0, k + 1).value <= threshold)) | 
               ((1 << 5) * (data.cell(i + 0, j + 1, k + 1).value <= threshold)) | 
               ((1 << 6) * (data.cell(i + 1, j + 1, k + 1).value <= threshold)) | 
               ((1 << 7) * (data.cell(i + 1, j + 0, k + 1).value <= threshold));
            
            const unsigned edge = EdgeTable[mcCase];
            
            if (edge != 0) {
               if (edge & 8) {
                  const Vertex &pt = _calculateIntersection(data, 3);
                  unsigned id = _getVertexID(data, 3);
                  
                  _addVertex(pt, id, data);
               }
               
               if (edge & 1) {
                  const Vertex &pt = _calculateIntersection(data, 0);
                  unsigned id = _getVertexID(data, 0);
                  
                  _addVertex(pt, id, data);
               }
               
               if (edge & 256) {
                  const Vertex &pt = _calculateIntersection(data, 8);
                  unsigned id = _getVertexID(data, 8);
                  
                  _addVertex(pt, id, data);
               }
               
               if (i == data.width - 2) {
                  if (edge & 4) {
                     const Vertex &pt = _calculateIntersection(data, 2);
                     unsigned id = _getVertexID(data, 2);
                     
                     _addVertex(pt, id, data);
                  }
                  
                  if (edge & 2048) {
                     const Vertex &pt = _calculateIntersection(data, 11);
                     unsigned id = _getVertexID(data, 11);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               if (j == data.height - 2) {
                  if (edge & 2) {
                     const Vertex &pt = _calculateIntersection(data, 1);
                     unsigned id = _getVertexID(data, 1);
                     
                     _addVertex(pt, id, data);
                  }
                  
                  if (edge & 512) {
                     const Vertex &pt = _calculateIntersection(data, 9);
                     unsigned id = _getVertexID(data, 9);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               if (k == data.depth - 2) {
                  if (edge & 16) {
                     const Vertex &pt = _calculateIntersection(data, 4);
                     unsigned id = _getVertexID(data, 4);
                     
                     _addVertex(pt, id, data);
                  }
                  
                  if (edge & 128) {
                     const Vertex &pt = _calculateIntersection(data, 7);
                     unsigned id = _getVertexID(data, 7);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               if (i == data.width - 2 && j == data.height - 2) {
                  if (edge & 1024) {
                     const Vertex &pt = _calculateIntersection(data, 10);
                     unsigned id = _getVertexID(data, 10);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               if (i == data.width - 2 && k == data.depth - 2) {
                  if (edge & 64) {
                     const Vertex &pt = _calculateIntersection(data, 6);
                     unsigned id = _getVertexID(data, 6);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               if (j == data.height - 2 && k == data.depth - 2) {
                  if (edge & 32) {
                     const Vertex &pt = _calculateIntersection(data, 5);
                     unsigned id = _getVertexID(data, 5);
                     
                     _addVertex(pt, id, data);
                  }
               }
               
               for (unsigned t = 0; TriangleTable[mcCase][t] != -1; t += 3) {
                  const unsigned vertexA = 
                     _getVertexID(data, TriangleTable[mcCase][t]);
                  const unsigned vertexB = 
                     _getVertexID(data, TriangleTable[mcCase][t + 2]);
                  const unsigned vertexC = 
                     _getVertexID(data, TriangleTable[mcCase][t + 1]);
                  
                  data.meshData.triangles.push_back(
                     MeshTriangle(vertexA, vertexB, vertexC)
                  );
               }
            }
         }
      }
   }
   
   _cleanup(data);
   
   return new Mesh(data.meshData);
}

Vertex MarchingCubes::_calculateIntersection(const MCData &data, 
                                             unsigned edge) const
{
   unsigned v1x = data.i, v1y = data.j, v1z = data.k;
   unsigned v2x = data.i, v2y = data.j, v2z = data.k;
   
   switch (edge) {
      case 0:
         ++v2y;
         break;
      case 1:
         ++v1y;
         ++v2x;
         ++v2y;
         break;
      case 2:
         ++v1x;
         ++v1y;
         ++v2x;
         break;
      case 3:
         ++v1x;
         break;
      case 4:
         ++v1z;
         ++v2y;
         ++v2z;
         break;
      case 5:
         ++v1y;
         ++v1z;
         ++v2x;
         ++v2y;
         ++v2z;
         break;
      case 6:
         ++v1x;
         ++v1y;
         ++v1z;
         ++v2x;
         ++v2z;
         break;
      case 7:
         ++v1x;
         ++v1z;
         ++v2z;
         break;
      case 8:
         ++v2z;
         break;
      case 9:
         ++v1y;
         ++v2y;
         ++v2z;
         break;
      case 10:
         ++v1x;
         ++v1y;
         ++v2x;
         ++v2y;
         ++v2z;
         break;
      case 11:
         ++v1x;
         ++v2x;
         ++v2z;
         break;
   }
   
   const Vertex &v1 = Vertex(v1x * data.xScale, 
                             v1y * data.yScale, 
                             v1z * data.zScale);
   const Vertex &v2 = Vertex(v2x * data.xScale, 
                             v2y * data.yScale, 
                             v2z * data.zScale);
   
   const real_t coeff1 = data.cell(v1x, v1y, v1z).value;
   const real_t coeff2 = data.cell(v2x, v2y, v2z).value;
   
   return _interpolate(v1, v2, coeff1, coeff2, data.threshold);
}

inline unsigned MarchingCubes::_getVertexID(const MCData &data, 
                                            unsigned edge) const
{
   switch (edge) {
      case 0:
         return _getVertexID(data.i, data.j, data.k, data.width, data.height) + 1;
      case 1:
         return _getVertexID(data.i, data.j + 1, data.k, data.width, data.height);
      case 2:
         return _getVertexID(data.i + 1, data.j, data.k, data.width, data.height) + 1;
      case 3:
         return _getVertexID(data.i, data.j, data.k, data.width, data.height);
      case 4:
         return _getVertexID(data.i, data.j, data.k + 1, data.width, data.height) + 1;
      case 5:
         return _getVertexID(data.i, data.j + 1, data.k + 1, data.width, data.height);
      case 6:
         return _getVertexID(data.i + 1, data.j, data.k + 1, data.width, data.height) + 1;
      case 7:
         return _getVertexID(data.i, data.j, data.k + 1, data.width, data.height);
      case 8:
         return _getVertexID(data.i, data.j, data.k, data.width, data.height) + 2;
      case 9:
         return _getVertexID(data.i, data.j + 1, data.k, data.width, data.height) + 2;
      case 10:
         return _getVertexID(data.i + 1, data.j + 1, data.k, data.width, data.height) + 2;
      case 11:
         return _getVertexID(data.i + 1, data.j, data.k, data.width, data.height) + 2;
      default:
         // invalid edge number
         ASSERT(0);
         
         return 0;
   }
}

inline unsigned MarchingCubes::_getVertexID(const MCData &data) const {
   return _getVertexID(data.i, data.j, data.k, data.width, data.height);
}

inline unsigned MarchingCubes::_getVertexID(unsigned i, unsigned j, unsigned k,
                                     unsigned width, unsigned height) const
{
   return 3 * ((k * height + j) * width + i);
}

Vertex MarchingCubes::_interpolate(const Vertex &v1, const Vertex &v2, 
                                   real_t coeff1, real_t coeff2, 
                                   real_t threshold) const
{
   const real_t alpha = (threshold - coeff1) / (coeff2 - coeff1);
   
   // v1 * (1 - alpha) + v2 * alpha
   return Vertex(v1 + alpha * (v2 - v1));
}

void MarchingCubes::_addVertex(const Vertex &vertex, unsigned id, 
                               MCData &data) const
{
   // record generation id -> real vertex id mapping
   data.vertexIDMap[id] = data.meshData.vertices.size();
   
   data.meshData.vertices.push_back(vertex);
}

void MarchingCubes::_cleanup(MCData &data) const {
   std::vector<MeshTriangle> &triangles = data.meshData.triangles;
   
   for(std::vector<MeshTriangle>::iterator iter = 
       triangles.begin(); iter != triangles.end(); ++iter)
   {
      MeshTriangle &triangle = *iter;
      
      for(int i = 3; i--;)
         triangle.data[i] = data.vertexIDMap[triangle.data[i]];
   }
   
   safeFree(data.grid);
}

}

