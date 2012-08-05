#include <DifferentialGeometry.h>
#include <MeshUtils.h>

void DifferentialSurface::init() {
   const Vector3 *vertices = mesh->getVertices();
   const unsigned n = mesh->getNoVertices();
   
   deltas.clear();
   
   safeDelete(L);
   L = new SparseMatrix(n, n);
   
   safeDeleteArray(vertexNeighborhood);
   MeshUtils::getNeighbors(mesh, &vertexNeighborhood, NULL);
   
   // second order Laplacian
   for(unsigned i = 0; i < n; ++i) {
      Vector3 delta = vertices[i];
      
      L->setValue(i, i, 1.0);
      
      const Neighborhood *neighbors = vertexNeighborhood + i;
      const real_t alpha = create_real(1.0) / neighbors->noVertices;
      
      for(unsigned j = neighbors->noVertices; j--;) {
         const unsigned index = neighbors->vertices[j];
         
         delta -= alpha * vertices[index];
         
         L->setValue(i, index, -alpha);
      }
      
      deltas.push_back(delta);
   }
   
   //L->save("L.png");
}

void DifferentialSurface::update() {
   const Vector3 *vertices = mesh->getVertices();
   const unsigned n = mesh->getNoVertices();
   
   ASSERT(vertexNeighborhood);
   ASSERT(deltas.size() == n);
   
   // second order Laplacian
   for(unsigned i = 0; i < n; ++i) {
      const Neighborhood *n = vertexNeighborhood + i;
      const real_t alpha = create_real(1.0) / n->noVertices;
      
      deltas[i] = vertices[i];
      
      for(unsigned j = n->noVertices; j--;) {
         const unsigned index = n->vertices[j];
         
         deltas[i] -= alpha * vertices[index];
      }
   }
}

void DifferentialCurve::init(unsigned order) {
   const unsigned n = vertices.size();
   
   deltas.clear();
   aabb = AABB();
   
   safeDelete(L);
   L = new SparseMatrix(n, n);
   
   if (1 == order) {
      // first order Laplacian
      for(unsigned i = 0; i < n; ++i) {
         const unsigned j = (i == 0 ? n - 1 : i - 1);
         const Vector3 &d = vertices[i] - vertices[j];
         
         L->setValue(i, i,  1);
         L->setValue(i, j, -1);
         
         aabb.add(vertices[i]);
         deltas.push_back(d);
      }
   } else {
      ASSERT(2 == order);
      
      // second order Laplacian
      for(unsigned i = 0; i < n; ++i) {
         const unsigned j = (i == 0 ? n - 1 : i - 1);
         const unsigned k = (i == n - 1 ? 0 : i + 1);
         const Vector3 &d = vertices[i] - 0.5 * (vertices[j] + vertices[k]);
         
         L->setValue(i, i,  1);
         L->setValue(i, j, -0.5);
         L->setValue(i, k, -0.5);
         
         aabb.add(vertices[i]);
         deltas.push_back(d);
      }
   }
   
   //L->save("l.png");
}

void DifferentialCurve::update(unsigned order) {
   const unsigned n = vertices.size();
   
   deltas.clear();
   aabb = AABB();
   
   if (1 == order) {
      // first order Laplacian
      for(unsigned i = 0; i < n; ++i) {
         const unsigned j = (i == 0 ? n - 1 : i - 1);
         const Vector3 &d = vertices[i] - vertices[j];
         
         aabb.add(vertices[i]);
         deltas.push_back(d);
      }
   } else {
      ASSERT(2 == order);
      
      // second order Laplacian
      for(unsigned i = 0; i < n; ++i) {
         const unsigned j = (i == 0 ? n - 1 : i - 1);
         const unsigned k = (i == n - 1 ? 0 : i + 1);
         const Vector3 &d = vertices[i] - 0.5 * (vertices[j] + vertices[k]);
         
         aabb.add(vertices[i]);
         deltas.push_back(d);
      }
   }
}

