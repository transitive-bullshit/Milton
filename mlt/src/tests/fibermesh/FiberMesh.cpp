#include "FiberMesh.h"
#include "LeastSquaresSolver.h"
#include "MeshUtils.h"
#include "Frontend.h"

#include <milton.h>
#include <QtCore>
#include <deque>
#include <map>

struct Constraint {
   Vector3  pos;
   double   weight;
   unsigned index;
   
   inline Constraint(unsigned index_, const Vector3 &pos_, double weight_ = 1)
      : pos(pos_), weight(weight_), index(index_)
   { }
   
   inline Constraint()
   { }
};

DECLARE_STL_TYPEDEF(std::vector<Constraint>, ConstraintList);

DragData::~DragData() {
   safeDelete(solver);
}

FiberMesh::FiberMesh(Frontend *parent) 
   : m_parent(parent), m_dragData(), m_mutex(new QMutex())
{ }

FiberMesh::~FiberMesh() {
   m_dragData.isDragging = false;
   
   FOREACH(CurveListIter, m_curves, iter) {
      DifferentialCurve *curve = *iter;
      
      safeDelete(curve);
   }
   
   m_curves.clear();
   safeDelete(m_mutex);
}

void FiberMesh::init() {
   m_curves.clear();
   m_dragData.isDragging = false;
}

Vector3 FiberMesh::getWorldVertex(const Vector2 &filmPt) {
   Camera *camera = m_parent->getCamera();
   
   const Ray &ray = camera->getWorldRay(Point2(filmPt[0], filmPt[1]));
   return Vector3((ray.origin + ray.direction).data);
}

bool FiberMesh::addCurve(const Vector2List &vertices) {
   const unsigned inputN = vertices.size();
   
   cerr << endl;
   if (inputN < 20) {
      cerr << "invalid input curve" << endl;
      return false;
   }
   
   const unsigned width  = m_parent->getWidth();
   const unsigned height = m_parent->getHeight(); 
   const real_t invWidth  = 1.0 / width;
   const real_t invHeight = 1.0 / height; 
   
   const bool closed = 
      (m_surfaces.size() == 0 || 
       (((vertices.front() - vertices.back()).getMagnitude() < 
         (width + height) / 15.0) && 
        ((vertices[0] - vertices[1]).getNormalized().dot((vertices[inputN - 2] - vertices[inputN - 1]).getNormalized()) > 0)));
   Vector2List smooth;
   
   m_curves.push_back(new DifferentialCurve(closed));
   DifferentialCurve *curve = m_curves.back();
   
   cerr << "adding " << (closed ? "closed" : "non-closed") << " curve " 
        << m_curves.size() << " with " << inputN << " vertices" << endl;
   
   { // nicely resample input curve vertices
      const unsigned noVertices = MIN(80u, inputN);
      cerr << "resampling curve regularly from " << inputN << " to " 
           << noVertices << " vertices" << endl;
      
      Vector2 prev = vertices[(closed ? inputN - 1 : 0)];
      real_t dist  = 0;
      
      for(unsigned i = !closed; i < inputN; ++i) {
         const Vector2 &v = vertices[i];
         
         dist += (v - prev).getMagnitude();
         prev  = v;
      }
      
      unsigned index = 1;
      Vector2 cur = vertices[1];
      prev = vertices[0];
      
      real_t delta   = dist / noVertices;
      real_t curDist = 0, newDelta = 0;
      real_t newDist = (cur - prev).getMagnitude();
      
      do {
         if (newDist >= newDelta) {
            const real_t d = (newDist - curDist);
            const real_t t = (d > 0 ? (newDist - newDelta) / d : 1);
            
            smooth.push_back((1 - t) * cur + t * prev);
            newDelta += delta;
         } else {
            curDist = newDist;
            prev    = cur;
            
            if (++index >= inputN) {
               if (!closed)
                  break;
               
               index = 0;
            }
            
            cur = vertices[index];
            newDist = curDist + (cur - prev).getMagnitude();
         }
      } while(newDelta < dist);
   }
   
   unsigned n = smooth.size();
   for(unsigned i = 0; i < n; ++i) {
      const Vector2 &v = smooth[i];
      const Vector2 filmPt(v[0] * invWidth, v[1] * invHeight);
      
      // project film-space point on curve out into world space
      const Vector3 &pt = getWorldVertex(filmPt);
      
      // add current world-space vertex to the resulting curve
      curve->vertices.push_back(pt);
   }
   
   // TODO: interpret smoothed stroke
   //    1) does it intersect any previously drawn surfaces?
   //       * is it a control curve drawn completely on the surface?
   //       * does it start off the surface, intersect the surface, then end on the surface?
   //    2) else attempt to create a new surface
   unsigned interpretation = 0;
   
   cerr << "interpreting curve.." << endl;
   Camera *camera = m_parent->getCamera();
   SurfacePoint sp;
   
   for(unsigned i = m_surfaces.size(); i--;) {
      DifferentialSurface *surface = m_surfaces[i];
      Mesh *mesh = surface->mesh;
      unsigned noIntersected = 0;
      
      for(unsigned j = n; j--;) {
         const Vector2 &v = smooth[j];
         const Point2 filmPt(v[0] * invWidth, v[1] * invHeight);
         
         Ray ray(camera->getWorldRay(filmPt));
         
         const real_t t = mesh->getIntersection(ray, sp);
         noIntersected += (Ray::isValid(t));
      }
      
      if (noIntersected == n) {
         // entire curve drawn onto an existing surface
         cerr << "entire curve drawn onto an existing surface; " 
              << "binding curve to mesh" << endl;
         
         interpretation = 1;
         const MeshTriangle *triangles = mesh->getTriangles();
         const Vector3 *vertices       = mesh->getVertices();
         vector<unsigned> bindings, triIndices;
         
         for(unsigned j = 0; j < n; ++j) {
            const Vector2 &v = smooth[j];
            const Point2 filmPt(v[0] * invWidth, v[1] * invHeight);
            
            Ray ray(camera->getWorldRay(filmPt));
            const real_t t = mesh->getIntersection(ray, sp);
            
            ASSERT(Ray::isValid(t));
            ASSERT(sp.index < mesh->getNoTriangles());
            
            triIndices.push_back(sp.index);
            const Vector3 &position = Vector3((ray.origin + t * ray.direction).data);
            const MeshTriangle &tri = triangles[sp.index];
            
            unsigned index = 0;
            real_t minDist = INFINITY;
            
            for(unsigned k = 3; k--;) {
               unsigned curIndex    = tri.data[k];
               const Vector3 &v     = vertices[curIndex];
               const real_t curDist = (v - position).getMagnitude2();
               
               if (curDist < minDist) {
                  index   = curIndex;
                  minDist = curDist;
               }
            }
            
            ASSERT(index == tri.A || index == tri.B || index == tri.C);
            bindings.push_back(index);
         }
         
         unsigned prev = (unsigned) -1;
         std::map<unsigned, unsigned> verts;
         vector<unsigned> P, bindings2;
         
         curve->bindings.clear();
         curve->vertices.clear();
         n = bindings.size();
         
         for(unsigned j = 0; j < n - (!closed); ++j) {
            unsigned binding0 = bindings[j];
            unsigned binding1 = bindings[j + 1 - n * (j == n - 1)];
            
            if (binding0 == binding1) {
               if (binding0 != prev) {
                  bindings2.push_back(binding0);
                  prev = binding0;
               }
            } else {
               MeshUtils::computeVertexShortestPaths(surface, binding0, P);
               
               vector<unsigned> path;
               path.push_back(binding1);
               
               while(path.back() != binding0)
                  path.push_back(P[path.back()]);
               
               std::reverse(path.begin(), path.end());
               for(unsigned k = 0; k < path.size(); ++k) {
                  const unsigned l = path[k];
                  
                  if (l != prev) {
                     bindings2.push_back(l);
                     prev = l;
                  }
               }
            }
         }
         
         curve->bindings = bindings2;
         
         if (bindings2.size() == 1) {
            cerr << "invalid input curve" << endl;
            m_curves.pop_back();
            return false;
         }
         
         // remove cycles that may have formed from different shortest paths
         if (!closed) {
            if (curve->bindings.front() == curve->bindings.back())
               curve->bindings.pop_back();
            
            /*std::map<unsigned, unsigned>::const_iterator iter;
            unsigned offset = 0;
            
            for(unsigned j = 0; j < bindings2.size(); ++j) {
               const unsigned binding = bindings2[j];
               
               if ((iter = verts.find(binding)) == verts.end()) {
                  verts[binding] = j;
                  continue;
               }
               
               std::vector<unsigned>::iterator s = curve->bindings.begin();
               unsigned start = verts[binding];
               for(unsigned k = start; k <= j; ++k) {
                  if (k >= offset) {
                     std::vector<unsigned>::iterator cur = s + k - offset;
                     curve->bindings.erase(cur);
                     ++offset;
                  }
               }
            }*/
         }
         
         /*cerr << endl;
         cerr << "new bindings: " << endl;
         for(unsigned j = 0; j < curve->bindings.size(); ++j)
            cerr << curve->bindings[j] << ", ";
         cerr << endl;*/
         
         for(unsigned j = 0; j < curve->bindings.size(); ++j)
            curve->vertices.push_back(vertices[curve->bindings[j]]);
         
         ASSERT(curve->bindings.size() == curve->vertices.size());
         
         curve->surfaceIndex = i;
         surface->curves.push_back(curve);
         break;
      }
   }
   
   if (interpretation == 0) {
      if (!closed || !_addSurface(smooth, curve)) {
         m_curves.pop_back();
         return false;
      }
   }
   
   curve->init(2);
   return true;
}

void FiberMesh::dragBegin(unsigned curveIndex, unsigned handleIndex) {
   DifferentialCurve *curve = m_curves[curveIndex];
   m_dragData.handle = curve->vertices[handleIndex];
   
   cerr << "dragging curve " << curveIndex << " with handle index " 
        << handleIndex << endl;
   
   m_dragData.curveIndex  = curveIndex;
   m_dragData.handleIndex = handleIndex;
   
   const unsigned n = curve->vertices.size();
   const unsigned k = 3; // dimension (x, y, z)
   const unsigned m = n; // number of constraints (extra constraints are zeroed out)
   
   LeastSquaresSolver *solver = new LeastSquaresSolver(n + m, n, k);
   SparseMatrix *A = solver->getA();
   
   { // copy L into top of A
      const SparseArray &l = curve->L->getData();
      
      SparseColArrayConstIterator j;
      SparseArrayConstIterator i;
      
      for(i = l.begin(); i != l.end(); ++i)
         for(j = i->second.begin(); j != i->second.end(); ++j)
            A->addValue(i->first, j->first, j->second);
   }
   
   { // fill in solution vector b with previously computed delta coordinates
      double *b = solver->getB();
      memset(b, 0, sizeof(double) * (n + m) * k);
      
      // fill in deltas
      for(unsigned i = n; i--;)
         for(unsigned j = k; j--;)
            b[i * k + j] = curve->deltas[i][j];
   }
   
   safeDelete(m_dragData.solver);
   m_dragData.solver = solver;
   
   // add initial ROI constraints
   _updateConstraints(.05);
   m_dragData.isDragging = true;
}

void FiberMesh::_updateConstraints(const real_t roiSize) {
   DifferentialCurve *curve = m_curves[m_dragData.curveIndex];
   const unsigned n = curve->vertices.size();
   const unsigned k = 3; // dimension (x, y, z)
   ConstraintList constraints;
   
   m_dragData.roiSize = roiSize;
   
   { // calculate constraints on boundary of ROI
      constraints.push_back(Constraint(m_dragData.handleIndex, m_dragData.handle, 1));
      
      m_dragData.roiSupport = MIN(((n - 1) / 2) - 1, MAX((unsigned)(n * roiSize), 4u));
      unsigned roiA = m_dragData.handleIndex - m_dragData.roiSupport;
      unsigned roiB = m_dragData.handleIndex + m_dragData.roiSupport;
      
      if (m_dragData.handleIndex < m_dragData.roiSupport)
         roiA = n - (m_dragData.roiSupport - m_dragData.handleIndex);
      if (roiB >= n)
         roiB = roiB - n;
      
      if (roiA < roiB) {
         for(unsigned i = 0; i <= roiA; ++i)
            constraints.push_back(Constraint(i, curve->vertices[i], 1));
         for(unsigned i = roiB; i < n; ++i)
            constraints.push_back(Constraint(i, curve->vertices[i], 1));
      } else {
         for(unsigned i = roiB; i <= roiA; ++i)
            constraints.push_back(Constraint(i, curve->vertices[i], 1));
      }
   }
   
   { // add constraints to remaining (lower) columns of A
      SparseMatrix *A = m_dragData.solver->getA();
      double *b = m_dragData.solver->getB();
      unsigned index = n;
      
      for(unsigned i = n; i < (unsigned) A->getM(); ++i)
         A->clearRow(i);
      
      for(unsigned c = 0; c < constraints.size(); ++c, ++index) {
         const Constraint &constraint = constraints[c];
         A->addValue(index, constraint.index, constraint.weight);
         
         for(unsigned i = k; i--;)
            b[index * k + i] = constraint.pos[i];
      }
      
      A->save("a.png");
   }
}

void FiberMesh::drag(const Vector3 &pt) {
   ASSERT(m_dragData.isDragging);
   ASSERT(m_dragData.solver);
   
   DifferentialCurve *curve = m_curves[m_dragData.curveIndex];
   const unsigned n = curve->vertices.size();
   
   // compute variable ROI size proportional to handle offset relative to 
   // curve's original AABB dimensions
   real_t diff = (m_dragData.handle - pt).getMagnitude();
   diff /= 4 * curve->aabb.getDiagonal().getMagnitude();
   diff  = ((real_t)((unsigned)(100 * MIN(1, diff + .1)))) / 100.0;
   
   if (diff - m_dragData.roiSize > 0.01)
      _updateConstraints(diff);
   
   double *b = m_dragData.solver->getB() + 3 * n;
   for(unsigned i = 3; i--;)
      b[i] = pt[i];
   
   // reconstruct new curve
   unsigned noSolved = m_dragData.solver->solve((double *)&(curve->vertices[0]));
   if (noSolved < 3)
      cerr << "error solving drag system" << endl;
   
   optimizeSurface(m_curves[m_dragData.curveIndex]->surfaceIndex, 1);
}

void FiberMesh::dragEnd() {
   ASSERT(m_dragData.isDragging);
   
   DifferentialCurve *curve = m_curves[m_dragData.curveIndex];
   curve->update(2);
   optimizeSurface(m_curves[m_dragData.curveIndex]->surfaceIndex, 8);
   
   m_dragData.isDragging = false;
}

Vector3 FiberMesh::_getNormal(DifferentialSurface *surface, unsigned i, unsigned m) {
   // find vertex normal (special case if first iteration)
   Mesh *mesh = surface->mesh;
   
   if (i < m) {
      const MeshTriangle *triangles = mesh->getTriangles();
      const Neighborhood *neighbors = surface->vertexNeighborhood + i;
      Vector3 *vertices = mesh->getVertices();
      unsigned k = m - 1;
      
      ASSERT(neighbors->noPolygons > 0);
      for(unsigned p = 0; p < neighbors->noPolygons; ++p) {
         const MeshTriangle &t = triangles[neighbors->polygons[p]];
         
         for(unsigned l = 3; l--;) {
            if (t.data[l] >= m) {
               k = t.data[l];
               break;
            }
         }
         
         if (k >= m) {
            const Vector3 &v0 = (vertices[i + 1 - m * (i == m - 1)] - 
                                 vertices[i]);
            const Vector3 &v1 = (vertices[k] - vertices[i]);
            const Vector3 &v2 = v0.cross(v1);
            
            return v0.cross(v2).getNormalized();
         }
      }
   }
   
   return mesh->getNormals()[i];
}

bool FiberMesh::optimizeSurface(unsigned i, unsigned noIterations) {
   QMutexLocker lock(m_mutex);
   DifferentialSurface *surface = m_surfaces[i];
   DifferentialCurve curve;
   
   cerr << "optimizing surface " << i << " with " << noIterations 
        << (noIterations == 1 ? " iteration" : " iterations") 
        << endl
        << "\t(" << surface->curves.size() << " constraint curve(s))" 
        << endl;
   
   const double wV = 100, wE = 0.001;
   
   std::set<unsigned> constrained;
   for(unsigned i = 0; i < surface->curves.size(); ++i) {
      const DifferentialCurve *cur = surface->curves[i];
      
      constrained.insert(cur->bindings.begin(), cur->bindings.end());
      curve.vertices.insert(curve.vertices.end(), cur->vertices.begin(), 
                            cur->vertices.end());
      curve.bindings.insert(curve.bindings.end(), cur->bindings.begin(), 
                            cur->bindings.end());
      
      ASSERT(curve.vertices.size() == curve.bindings.size());
   }
   
   Mesh *mesh = surface->mesh;
   const MeshTriangle *triangles = mesh->getTriangles();
   Vector3 *vertices = mesh->getVertices();
   
   const unsigned n = mesh->getNoVertices();
   const unsigned m = curve.vertices.size();
   const unsigned k = 3;
   bool success = false;
   
   unsigned p = 0;
   for(unsigned i = m; i--;) {
      const unsigned vertexIndex = curve.bindings[i];
      
      const Neighborhood &neighbors = surface->vertexNeighborhood[vertexIndex];
      const unsigned noNeighbors = neighbors.noVertices;
      p += noNeighbors;
   }
   
   LeastSquaresSolver *solver1 = new LeastSquaresSolver(n + n,  n, 1);
   LeastSquaresSolver *solver2 = new LeastSquaresSolver(n + m + p, n, k);
   
   { // Setup A1 and A2 which are invariant throughout the iteration process
     // note: A1 changes only once after the first iteration
      SparseMatrix *A1 = solver1->getA();
      SparseMatrix *A2 = solver2->getA();
      const SparseArray &l = surface->L->getData();
      
      SparseColArrayConstIterator j;
      SparseArrayConstIterator i;
      
      // copy mesh Laplacian operator into top of A1 and A2
      // (dependent on connectivy which doesn't change as vertices are updated)
      for(i = l.begin(); i != l.end(); ++i) {
         for(j = i->second.begin(); j != i->second.end(); ++j) {
            A1->addValue(i->first, j->first, j->second);
            A2->addValue(i->first, j->first, j->second);
         }
      }
      
      // setup constraints in A1 and A2
      for(unsigned i = m, index = 0; i--;) {
         const unsigned vertexIndex    = curve.bindings[i];
         const Neighborhood &neighbors = surface->vertexNeighborhood[vertexIndex];
         const unsigned noNeighbors    = neighbors.noVertices;
         
         //for(unsigned j = noNeighbors; j--;)
            //A2->addValue(n + m + i, neighbors.vertices[j], -wE / noNeighbors);
         //A2->addValue(n + m + i, vertexIndex, wE); // part of target edge vector
         
         for(unsigned j = noNeighbors; j--; ++index) {
            A2->addValue(n + m + index, neighbors.vertices[j], -wE);
            A2->addValue(n + m + index, vertexIndex, wE); // part of target edge vector
         }
         
         A1->addValue(n + vertexIndex, vertexIndex, 1);
         A2->addValue(n + 0 + i, vertexIndex, wV); // vertex position
      }
      
      A1->save("A1.png");
      A2->save("A2.png");
   }
   
   double *lms         = new double[n];
   double *edgeLengths = new double[n];
   double *curvatures  = new double[n];
   
   double maxDist = 0;
   for(unsigned i = n; i--;) {
      for(unsigned j = i; j--;) {
         double dist = (vertices[j] - vertices[i]).getMagnitude2();
         if (dist > maxDist)
            maxDist = dist;
      }
   }
   
   maxDist = sqrt(maxDist);
   
   // TODO: should be maxDist / 2, but that produces strange results..
   double k2 = 1.0 / maxDist;
   
   //cerr << k2 << endl;
   
   // repeatedly solve a series of linear systems until the system overall 
   // converges to a nonlinear optimum
   for(unsigned iteration = 0; iteration < noIterations; ++iteration) {
      const Vector3 *normals = mesh->getNormals();
      
      for(unsigned i = n; i--;) {
         curvatures[i] = k2;
      }
      
      if (iteration == 1) {
         SparseMatrix *A1 = solver1->getA();
         
         for(unsigned i = n; i--;)
            A1->addValue(n + i, i, 1);
      }
      
      { // compute target laplacian magnitudes
         double *b = solver1->getB();
         memset(b, 0, sizeof(double) * (n + n) * 1);
         
         if (iteration == 0) {
            // constrain all of the boundary vertices on the original control curve
            for(unsigned i = m; i--;) {
               const unsigned vertexIndex = curve.bindings[i];
               
               b[n + vertexIndex] = curvatures[vertexIndex];//curve.deltas[i].getMagnitude();
               //cerr << curve.deltas[i].getMagnitude() << " vs " << b[n + vertexIndex] << endl;
            }
         } else {
            // constrain all vertices
            for(unsigned i = n; i--;) {
               /*const Neighborhood &neighbors = surface->vertexNeighborhood[i];
               Vector3 v = 2 * vertices[i];
               double norm = 2.0 / neighbors.noVertices;
               
               for(unsigned j = neighbors.noVertices; j--;)
                  v -= norm * vertices[neighbors.vertices[j]];
               
               curvatures[i] = v.dot(normals[i]);*/
               
               /*const Neighborhood *neighbors = surface->vertexNeighborhood + i;
               const unsigned noNeighbors = neighbors->noPolygons;
               
               real_t Ai = 0;
               for(unsigned j = noNeighbors; j--;)
                  Ai += triangles[neighbors->polygons[j]].getSurfaceArea();
               Ai /= 3;*/
               
               curvatures[i] = -normals[i].dot(surface->deltas[i]);
               
               b[n + i] = curvatures[i]; //surface->deltas[i].getMagnitude();
            }
         }
         
         // solve for target laplacian magnitudes
         if (!solver1->solve(lms)) {
            cerr << "error solving surface optimization (1: target laplacian magnitudes)" << endl;
            goto cleanup;
         }
      }
      
      { // compute target edge lengths
         double *b = solver1->getB();
         memset(b, 0, sizeof(double) * (n + n) * 1);
         
         // constrain all of the boundary vertices on the original control curve
         if (iteration == 0) {
            for(unsigned i = m; i--;) {
               const unsigned vertexIndex = curve.bindings[i];
               const Vector3 &v = vertices[vertexIndex];
               
               const Neighborhood &neighbors = surface->vertexNeighborhood[vertexIndex];
               real_t cumEdgeLength = 0;
               
               for(unsigned j = neighbors.noVertices; j--;)
                  cumEdgeLength += (vertices[neighbors.vertices[j]] - v).getMagnitude();
               
               b[n + vertexIndex] = cumEdgeLength / neighbors.noVertices;
            }
         } else {
            // constrain all vertices
            for(unsigned i = n; i--;) {
               const Neighborhood &neighbors = surface->vertexNeighborhood[i];
               const Vector3 &v = vertices[i];
               real_t cumEdgeLength = 0;
               
               for(unsigned j = neighbors.noVertices; j--;)
                  cumEdgeLength += (vertices[neighbors.vertices[j]] - v).getMagnitude();
               
               b[n + i] = cumEdgeLength / neighbors.noVertices;
            }
         }
         
         // solve for target edge lengths
         if (!solver1->solve(edgeLengths)) {
            cerr << "error solving surface optimization (2: edge lengths)" << endl;
            goto cleanup;
         }
      }
      
      { // initialize solution vector for third minimization problem
         double *b2 = solver2->getB();
         memset(b2, 0, sizeof(double) * (n + m + p) * k);
         
         // insert integrated target laplacians into solution vector
         for(unsigned i = n; i--;) {
            const Neighborhood *neighbors = surface->vertexNeighborhood + i;
            const unsigned noNeighbors = neighbors->noPolygons;
            
            // compute cumulative area estimate for all triangles incident on vertex i
            real_t Ai = 0;
            for(unsigned j = noNeighbors; j--;)
               Ai += triangles[neighbors->polygons[j]].getSurfaceArea();
            Ai /= 3;
            
            // find vertex normal (special case if first iteration)
            Vector3 normal = normals[i];
            if (iteration == 0 && i < m) {
               unsigned k = m - 1;
               
               for(unsigned p = 0; p < neighbors->noPolygons; ++p) {
                  const MeshTriangle &t = triangles[neighbors->polygons[p]];
                  
                  for(unsigned l = 3; l--;) {
                     if (t.data[l] >= m) {
                        k = t.data[l];
                        break;
                     }
                  }
                  
                  if (k >= m)
                     break;
               }
               
               if (k >= m) {
                  const Vector3 &v0 = (vertices[i + 1 - m * (i == m - 1)] - 
                                       vertices[i]);
                  const Vector3 &v1 = (vertices[k] - vertices[i]);
                  const Vector3 &v2 = v0.cross(v1);
                  normal = v0.cross(v2).getNormalized();
               }
            }
            
            const Vector3 &delta = (Ai * lms[i]) * normals[i];
            
            //cerr << delta << ": " << Ai << ", " << lms[i] << ", " << normals[i] << endl;
            for(unsigned j = k; j--;)
               b2[i * k + j] = delta[j];
         }
         
         // insert target edge vectors and control curve positions into solution vector
         for(unsigned i = m, index = 0; i--;) {
            const unsigned vertexIndex = curve.bindings[i];
            const Vector3 &v = vertices[vertexIndex];
            
            const Neighborhood *neighbors = surface->vertexNeighborhood + vertexIndex;
            const unsigned noNeighbors = neighbors->noVertices;
            
            for(unsigned j = noNeighbors; j--; ++index) {
               const unsigned neighborIndex = neighbors->vertices[j];
               
               const Vector3 &edgeVector = 
                  (v - vertices[neighborIndex]).getNormalized() * 
                  ((edgeLengths[vertexIndex] + edgeLengths[neighborIndex]) / 2);
               
               for(unsigned l = k; l--;)
                  b2[(n + m + index) * k + l] = edgeVector[l] * wE;
            }
            
            /*Vector3 edgeVector;
            
            // compute target edge vector
            for(unsigned j = noNeighbors; j--;) {
               const unsigned neighborIndex = neighbors->vertices[j];
               
               edgeVector += 
                  (v - vertices[neighborIndex]).getNormalized() * 
                  ((edgeLengths[vertexIndex] + edgeLengths[neighborIndex]) / 2);
            }
            
            edgeVector /= noNeighbors;
            
            for(unsigned l = k; l--;)
               b2[(n + m + i) * k + l] = edgeVector[l];
            */
            
            for(unsigned l = k; l--;)
               b2[(n + 0 + i) * k + l] = wV * curve.vertices[i][l];
         }
         
         /*cerr << "deltas:\n";
         for(unsigned i = 0; i < n; ++i)
            cerr << "{ " << b2[i*3+0] << ", " << b2[i*3+1] << ", " << b2[i*3+2] << "} ";
         cerr << endl << "positional constraints:\n";
         for(unsigned i = n; i < n + m; ++i)
            cerr << "{ " << b2[i*3+0] << ", " << b2[i*3+1] << ", " << b2[i*3+2] << "} ";
         cerr << endl << "edge constraints:\n";
         for(unsigned i = n + m; i < n + m + p; ++i)
            cerr << "{ " << b2[i*3+0] << ", " << b2[i*3+1] << ", " << b2[i*3+2] << "} ";
         cerr << endl;*/
      }
      
      { // solve for target vertices
         double *v = surface->getVertexArray();
         
         if (solver2->solve(v) < k) {
            cerr << "error solving surface optimization (3: vertex positions)" << endl;
            goto cleanup;
         }
         
         // reinitialize mesh after geometry update
         surface->mesh->init(true);
         surface->mesh->computeNormals();
         surface->update();
      }
      
      /*for(unsigned i = n; i--;) {
         bool isCurve = 0;
         
         for(unsigned j = m; j--;) {
            if ((isCurve = (curve.bindings[j] == i)))
               break;
         }
         
         cerr << (isCurve ? "curve" : "not curve") << " : " << curvatures[i] << endl;
      }*/
   }
   
   cerr << "done" << endl;
   success = true;
   
cleanup:
   safeDeleteArray(lms);
   safeDeleteArray(edgeLengths);
   safeDeleteArray(curvatures);
   safeDelete(solver1);
   safeDelete(solver2);
   
   return success;
}

bool FiberMesh::_addSurface(const Vector2List &vertices, DifferentialCurve *curve) {
   const unsigned width  = m_parent->getWidth();
   const unsigned height = m_parent->getHeight();
   const unsigned m = vertices.size();
   const unsigned resolution = 15;
   
   cerr << "triangulating mesh" << endl;
   
   Vector2 min = Vector2::infinity();
   Vector2 max = Vector2::negativeInfinity();
   
   // find 2D AABB
   for(unsigned i = m; i--;) {
      const Vector2 &v = vertices[i];
      
      min[0] = MIN(min[0], v[0]);
      min[1] = MIN(min[1], v[1]);
      max[0] = MAX(max[0], v[0]);
      max[1] = MAX(max[1], v[1]);
   }
   
   Vector2List v;
   for(unsigned i = resolution; i--;) {
      for(unsigned j = resolution; j--;) {
         const Vector2 &p = min + Vector2(j * (max[0] - min[0]) / resolution, 
                                          i * (max[1] - min[1]) / resolution);
         
         if (MeshUtils::isPointInPolygon(p, vertices))
            v.push_back(p);
      }
   }
   
   const unsigned n = v.size();
   const std::string &prefix  = "test";
   const std::string &inFile  = prefix + ".m";
   const std::string &outFile = prefix + ".mat";
   MeshData data;
   
   { // setup matlab file to execute delaunay triangulation
      std::ofstream out;
      out.open(inFile.c_str());
      
      if (!out.is_open()) {
         cerr << "error opening file '" << inFile << "'" << endl;
         return false;
      }
      
      out << "y = [ ";
      for(unsigned i = 0; i < m; ++i)
         out << vertices[i][1] << " ";
      for(unsigned i = 0; i < n; ++i)
         out << v[i][1] << " ";
      out << " ]; x = [ ";
      for(unsigned i = 0; i < m; ++i)
         out << vertices[i][0] << " ";
      for(unsigned i = 0; i < n; ++i)
         out << v[i][0] << " ";
      out << " ]; t = delaunay(x,y); save('" << outFile << "', 't', '-ASCII'); quit;" << endl;
   }
   
   { // invoke matlab :-)
      const std::string &command = "matlab -nodisplay -r " + prefix;
      FILE *f = popen(command.c_str(), "r");
      if (NULL == f)
         return false;
      
      if (pclose(f))
         return false;
   }
   
   { // insert all mesh vertices
      for(unsigned i = 0; i < m; ++i)
         data.vertices.push_back(getWorldVertex(vertices[i][0] / width, vertices[i][1] / height));
      for(unsigned j = 2; j--;)
         for(unsigned i = 0; i < n; ++i)
            data.vertices.push_back(getWorldVertex(v[i][0] / width, v[i][1] / height));
   }
   
   { // parse triangles from matlab output
      std::ifstream in;
      in.open(outFile.c_str());
      
      if (!in.is_open()) {
         cerr << "error opening file '" << in << "'" << endl;
         return false;
      }
      
      do {
         double aIn, bIn, cIn;
         in >> aIn >> bIn >> cIn;
         
         if (in.eof() || in.bad() || in.fail())
            break;
         
         // a, b, c are vertex indices for current triangle
         unsigned a = aIn - 1;
         unsigned b = bIn - 1;
         unsigned c = cIn - 1;
         
         if (a < m && b < m && c < m)
            continue;
         
         // insert frontside and backside triangles
         // aP, bP, cP are vertex indices for backside triangle
         unsigned aP = a + n * (a >= m);
         unsigned bP = b + n * (b >= m);
         unsigned cP = c + n * (c >= m);
         
         // ensure indices are within valid bounds
         ASSERT(a  < m + n     && b  < m + n     && c  < m + n);
         ASSERT(aP < m + n + n && bP < m + n + n && cP < m + n + n);
         
         data.triangles.push_back(MeshTriangle(b,  a,  c));
         data.triangles.push_back(MeshTriangle(aP, bP, cP));
      } while(1);
   }
   
   { // remote temporary files
      remove(inFile.c_str());
      remove(outFile.c_str());
   }
   
   { // bind mesh boundary vertices to curve vertices
      curve->surfaceIndex = m_surfaces.size();
      curve->bindings.resize(m);
      
      for(unsigned i = m; i--;)
         curve->bindings[i] = i;
   }
   
   cerr << "initializing mesh" << endl;
   
   { // initialize new mesh
      Mesh *mesh = new Mesh(data);
      ASSERT(mesh);
      
      Material *material = new Material();
      (*mesh)["spatialAccel"] = std::string("naive");
      (*material)["kd"] = SpectralSampleSet(1, 0, 0);
      mesh->setMaterial(material);
      mesh->init();
      
      DifferentialSurface *surface = new DifferentialSurface(mesh);
      surface->init();
      surface->curves.push_back(curve);
      
      m_surfaces.push_back(surface);
   }
   
   return true;
}

#if 0
   const Vector2 norm((resolution - 1) / (max[0] - min[0]), (resolution - 1) / (max[1] - min[1]));
   
   for(unsigned i = 0; i < n; ++i) {
      const Vector2 &vA = (vertices[i] - min) * norm;
      const Vector2 &vB = (vertices[(i == 0 ? n - 1 : i - 1)] - min) * norm;
      
      p.drawLine(QPointF(vA[0], vA[1]), QPointF(vB[0], vB[1]));
   }
   
   const unsigned res2 = (resolution + 1);
   const unsigned size = res2 * res2;
   
   Cell *grid = new Cell[size];
   unsigned sentinel = (unsigned) -1;
   unsigned count = 0;
   
   for(unsigned i = res2 * res2; i--;)
      grid[i].index = sentinel;
   
   const Vector3 &A = getWorldVertex(Vector2(0 * (max[0]-min[0])/width + min[0] / width, 0*(max[1]-min[1])/height + min[1] / height));
   const Vector3 &B = getWorldVertex(Vector2(1 * (max[0]-min[0])/width + min[0] / width, 0*(max[1]-min[1])/height + min[1] / height));
   const Vector3 &C = getWorldVertex(Vector2(0 * (max[0]-min[0])/width + min[0] / width, 1*(max[1]-min[1])/height + min[1] / height));
   
   const Vector3 &N = (A - B).cross(C - B).getNormalized() / 10;
   
   for(unsigned i = 0; i < resolution; ++i) {
      for(unsigned j = 0; j < resolution; ++j) {
         const QRgb &rgb = data[i * resolution + j];
         
         if (!rgb)
            continue;
         
         unsigned vA = grid[res2 * (i + 0) + j + 0].index;
         unsigned vB = grid[res2 * (i + 0) + j + 1].index;
         unsigned vC = grid[res2 * (i + 1) + j + 0].index;
         unsigned vD = grid[res2 * (i + 1) + j + 1].index;
         
         unsigned vAP = grid[res2 * (i + 0) + j + 0].prime;
         unsigned vBP = grid[res2 * (i + 0) + j + 1].prime;
         unsigned vCP = grid[res2 * (i + 1) + j + 0].prime;
         unsigned vDP = grid[res2 * (i + 1) + j + 1].prime;
         
         const unsigned boundary =
            ((j == 0 || (!data[i * resolution + j - 1])) << 0) | // left
            ((j == resolution - 1 || (!data[i * resolution + j + 1])) << 1) | // right
            ((i == 0 || (!data[(i - 1) * resolution + j])) << 2) | // top
            ((i == resolution - 1 || (!data[(i + 1) * resolution + j])) << 3); // bottom
         
         if (vA == sentinel) {
            vA = grid[res2 * (i + 0) + j + 0].index = count++;
            const Vector2 &v = Vector2(j + 0, i + 0) / ((real_t) resolution);
            const Vector3 &w = getWorldVertex(Vector2(v[0] * (max[0]-min[0])/width + min[0] / width, v[1]*(max[1]-min[1])/height + min[1] / height));
            
            if (boundary & 5 || (i > 0 && j > 0 && !data[(i - 1) * resolution + j - 1])) {
               m.vertices.push_back(w);
               vAP = vA;
            } else {
               m.vertices.push_back(w + N);
               m.vertices.push_back(w - N);
               
               vAP = count++;
            }
            
            grid[res2 * (i + 0) + j + 0].prime = vAP;
         }
         
         if (vB == sentinel) {
            vB = grid[res2 * (i + 0) + j + 1].index = count++;
            const Vector2 &v = Vector2(j + 1, i + 0) / ((real_t) resolution);
            const Vector3 &w = getWorldVertex(Vector2(v[0] * (max[0]-min[0])/width + min[0] / width, v[1]*(max[1]-min[1])/height + min[1] / height));
            
            if (boundary & 6 || (j < resolution - 1 && i > 0 && !data[(i - 1) * resolution + j + 1])) {
               m.vertices.push_back(w);
               vBP = vB;
            } else {
               m.vertices.push_back(w + N);
               m.vertices.push_back(w - N);
               
               vBP = count++;
            }
            
            grid[res2 * (i + 0) + j + 1].prime = vBP;
         }
         
         if (vC == sentinel) {
            vC = grid[res2 * (i + 1) + j + 0].index = count++;
            const Vector2 &v = Vector2(j + 0, i + 1) / ((real_t) resolution);
            const Vector3 &w = getWorldVertex(Vector2(v[0] * (max[0]-min[0])/width + min[0] / width, v[1]*(max[1]-min[1])/height + min[1] / height));
            
            if (boundary & 9 || (j > 0 && i < resolution - 1 && !data[(i + 1) * resolution + j - 1])) {
               m.vertices.push_back(w);
               vCP = vC;
            } else {
               m.vertices.push_back(w + N);
               m.vertices.push_back(w - N);
               
               vCP = count++;
            }
            
            grid[res2 * (i + 1) + j + 0].prime = vCP;
         }
         
         if (vD == sentinel) {
            vD = grid[res2 * (i + 1) + j + 1].index = count++;
            const Vector2 &v = Vector2(j + 1, i + 1) / ((real_t) resolution);
            const Vector3 &w = getWorldVertex(Vector2(v[0] * (max[0]-min[0])/width + min[0] / width, v[1]*(max[1]-min[1])/height + min[1] / height));
            
            if (boundary & 10 || (j < resolution - 1 && i < resolution - 1 && !data[(i + 1) * resolution + j + 1])) {
               m.vertices.push_back(w);
               vDP = vD;
            } else {
               m.vertices.push_back(w + N);
               m.vertices.push_back(w - N);
               
               vDP = count++;
            }
            
            grid[res2 * (i + 1) + j + 1].prime = vDP;
         }
         
         // add 2 triangles on front side
         m.triangles.push_back(MeshTriangle(vB, vA, vC));
         m.triangles.push_back(MeshTriangle(vD, vB, vC));
         
         // add 2 triangles on back side
         m.triangles.push_back(MeshTriangle(vAP, vBP, vCP));
         m.triangles.push_back(MeshTriangle(vBP, vDP, vCP));
      }
   }
   
   ASSERT(count == m.vertices.size());
   safeDeleteArray(grid);
   
   { // bind mesh boundary vertices to curve vertices
      curve->surfaceIndex = m_surfaces.size();
      curve->bindings.resize(n);
      
      const unsigned noVertices = m.vertices.size();
      
      for(unsigned i = 0; i < n; ++i) {
         const Vector3 &curveV = curve->vertices[i];
         real_t minDist = INFINITY;
         unsigned min   = 0;
         
         for(unsigned j = noVertices; j--;) {
            const Vector3 &v  = m.vertices[j];
            const real_t diff = (v - curveV).getMagnitude2();
            
            if (diff < minDist) {
               minDist = diff;
               min     = j;
            }
         }
         
         // binding the closest mesh vertex to this curve vertex
         ASSERT(minDist < INFINITY);
         m.vertices[min] = curveV;
         curve->bindings[i] = min;
      }
   }
#endif 

/*const Neighborhood *neighbors = surface->vertexNeighborhood + i;
const unsigned noNeighbors = neighbors->noPolygons;
const Vector3 &vi = vertices[i];
std::set<unsigned> nei;

for(unsigned j = noNeighbors; j--;) {
   const MeshTriangle &tri = triangles[neighbors->polygons[j]];
   
   for(unsigned k = 3; k--;) {
      if (tri.data[k] == i || nei.find(tri.data[k]) != nei.end())
         continue;
      
      nei.insert(tri.data[k]);
      const Vector3 &vk = vertices[tri.data[k]];
      bool found = false;
      
      for(unsigned l = j; l--;) {
         const MeshTriangle &tri2 = triangles[neighbors->polygons[l]];
         
         for(unsigned m = 3; m--;)
            found |= (tri2.data[m] == tri.data[k]);
         
         if (!found)
            continue;
         
         unsigned o = tri.data[(k + 1) % 3];
         if (o == i) o = tri.data[(k + 2) % 3];
         ASSERT(o != i);
         
         unsigned r = 0;
         for(unsigned m = 3; m--;) {
            if (tri2.data[m] != tri.data[k] && tri2.data[m] != i) {
               r = tri2.data[m];
               break;
            }
         }
         
         const real_t alpha = acos((vi - vertices[o]).getNormalized().dot((vk - vertices[o]).getNormalized()));
         const real_t beta  = acos((vi - vertices[r]).getNormalized().dot((vk - vertices[r]).getNormalized()));
         
         curvatures[i] += (.5 * (cos(alpha)/sin(alpha) + cos(beta)/sin(beta)) * (vi - vk)).getMagnitude();
      }
      
      ASSERT(found);
   }
   
   curvatures[i] /= tri.getSurfaceArea() / 3;
}*/

