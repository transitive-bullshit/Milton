#ifndef DIFFERENTIAL_GEOMETRY_H_
#define DIFFERENTIAL_GEOMETRY_H_

#include <shapes/Mesh.h>
#include <accel/AABB.h>
#include <SparseMatrix.h>

struct Neighborhood;

DECLARE_STL_TYPEDEF(std::vector<Vector3 >,  Vector3List);
DECLARE_STL_TYPEDEF(std::vector<Vector2 >,  Vector2List);
DECLARE_STL_TYPEDEF(std::vector<unsigned >, VertexBindingList);

struct DifferentialCurve;
struct DifferentialSurface;

DECLARE_STL_TYPEDEF(std::vector<DifferentialCurve*>,   CurveList);
DECLARE_STL_TYPEDEF(std::vector<DifferentialSurface*>, SurfaceList);

struct DifferentialCurve {
   Vector3List   vertices;
   AABB          aabb;
   
   Vector3List   deltas;
   SparseMatrix *L;
   
   VertexBindingList bindings;
   unsigned          surfaceIndex;
   bool              closed;
   
   inline DifferentialCurve(bool closed_ = true)
      : L(NULL), surfaceIndex(0), closed(closed_)
   { }
   
   inline DifferentialCurve(const Vector3List &vertices_)
      : vertices(vertices_), L(NULL), surfaceIndex(0)
   { }
   
   /*inline DifferentialCurve(const DifferentialCurve &copy)
      : vertices(copy.vertices), aabb(copy.aabb), deltas(copy.deltas), 
        L(copy.L ? new SparseMatrix(*copy.L) : NULL), bindings(copy.bindings), 
        surfaceIndex(copy.surfaceIndex)
   { }*/
   
   /// compute L and deltas (of the given @p order) from the current vertices
   /// order may be 1 or 2 for first or second order Laplacian respectively
   void init(unsigned order = 2);
   
   /// compute deltas (of the given @p order) from the current vertices
   /// order may be 1 or 2 for first or second order Laplacian respectively
   /// @note L is not recomputed
   void update(unsigned order = 2);
   
   inline double *getVertexArray() {
      ASSERT(vertices.size() > 0);
      
      return (double *)&vertices[0];
   }
   
   ~DifferentialCurve() {
      safeDelete(L);
   }
};

struct DifferentialSurface {
   Mesh         *mesh;
   SparseMatrix *L;
   Neighborhood *vertexNeighborhood;
   
   Vector3List   deltas;
   CurveList     curves;
   
   /// @note takes ownership of @p mesh_
   inline DifferentialSurface(Mesh *mesh_ = NULL)
      : mesh(mesh_), L(NULL), vertexNeighborhood(NULL)
   { }
   
   /// compute L and deltas from the current mesh
   void init();
   
   /// compute deltas from the current mesh
   /// @note L is not recomputed
   void update();
   
   inline double *getVertexArray() {
      return (double *)mesh->getVertices();
   }
   
   ~DifferentialSurface() {
      //safeDelete(mesh);
      //safeDelete(L);
   }
};

#endif // DIFFERENTIAL_GEOMETRY_H_

