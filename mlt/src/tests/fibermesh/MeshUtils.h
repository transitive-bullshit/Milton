#ifndef MESH_UTILS_H_
#define MESH_UTILS_H_

#include "DifferentialGeometry.h"
#include <set>

/**
 * Stores neighborhoods that give relationships between connected vertices 
 * and polygons
 */
struct Neighborhood {
   // Indices of all adjacent polygons
   unsigned  noPolygons;
   unsigned *polygons;
   
   // Indices of all adjacent vertices
   unsigned  noVertices;
   unsigned *vertices;

   inline Neighborhood() 
      : noPolygons(0), polygons(NULL), 
        noVertices(0), vertices(NULL)
   { }
   
   ~Neighborhood() {
      safeDeleteArray(polygons);
      safeDeleteArray(vertices);
   }
};

class MeshUtils {
   public:
      static bool isPointInPolygon(const Vector2 &p, 
                                   const Vector2List &curve);
      
      /// @note polygonN is optional; set to NULL to only find vertex neighborhoods
      static bool getNeighbors(Mesh *mesh, 
                               Neighborhood **vertexN, 
                               Neighborhood **polygonN);
      
      static void computeVertexShortestPaths(DifferentialSurface *surface, 
                                             unsigned fromVertex, 
                                             vector<unsigned> &P);
      
      static void computeVertexShortestPaths(DifferentialSurface *surface, 
                                             unsigned fromVertex, 
                                             vector<unsigned> &P, 
                                             vector<double> &D);
      
   private:
      static void _createArrayFromSet(const std::set<unsigned> &toAdd, 
                                      unsigned **array, unsigned *size);
};

#endif // MESH_UTILS_H_

