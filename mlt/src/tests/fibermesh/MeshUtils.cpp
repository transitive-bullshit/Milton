#include "MeshUtils.h"
#include <queue>
#include <map>
using namespace std;

typedef pair<double, unsigned> pairDI_t;

bool MeshUtils::getNeighbors(Mesh *mesh, 
                             Neighborhood **vertexN, 
                             Neighborhood **polygonN)
{
   unsigned noVertices  = mesh->getNoVertices();
   unsigned noPolygons  = mesh->getNoTriangles();
   MeshTriangle *polygons = mesh->getTriangles();
   ASSERT(vertexN);
   
   Neighborhood *vertexNeighborhood  = new Neighborhood[noVertices];
   *vertexN  = vertexNeighborhood;
   
   // map from vertex index to a set containing indices of polygons which 
   // contain that vertex
   map<unsigned, set<unsigned> > vertexDegrees;
   
   for(unsigned t = noPolygons; t--;) {
      const MeshTriangle &poly = polygons[t];

      for(unsigned j = 3; j--;)
         vertexDegrees[poly.data[j]].insert(t);
   }
   
   // Handle vertex neighborhoods
   for(unsigned v = noVertices; v--;) {
      set<unsigned>::const_iterator iter;
      set<unsigned> vertNeighbors;

      // polygon neighbors of current vertex
      _createArrayFromSet(vertexDegrees[v],
                          &vertexNeighborhood[v].polygons, 
                          &vertexNeighborhood[v].noPolygons);
      
      // Loop through all neighboring polygons of current vertex
      for(unsigned i = vertexNeighborhood[v].noPolygons; i--;) {
         const MeshTriangle &poly = polygons[vertexNeighborhood[v].polygons[i]];
         
         // For all vertices of the current neighboring polygon
         for(unsigned j = 3; j--;) {
            if (poly.data[j] != v)
               vertNeighbors.insert(poly.data[j]);
         }
      }
      
      _createArrayFromSet(vertNeighbors,
                          &vertexNeighborhood[v].vertices, 
                          &vertexNeighborhood[v].noVertices);
   }
   
   if (polygonN) {
      Neighborhood *polygonNeighborhood = new Neighborhood[noPolygons];
      *polygonN = polygonNeighborhood;
      
      // Handle polygon neighborhoods
      for(unsigned t = noPolygons; t--;) {
         set<unsigned> polyNeighbors, vertNeighbors;
         const MeshTriangle &poly = polygons[t];
         
         // Loop through each vertex of current polygon
         for(unsigned j = 3; j--;) {
            const Neighborhood &vertNeighbors = vertexNeighborhood[poly.data[j]];
            const unsigned *polyNeighborsOfJ = vertNeighbors.polygons;
            
            // Loop through all neighboring polygons of current vertex
            for(unsigned k = vertNeighbors.noPolygons; k--;) {
               unsigned neighborIndex = polyNeighborsOfJ[k];
               
               if (neighborIndex != t)
                  polyNeighbors.insert(neighborIndex);
            }
         }

         _createArrayFromSet(polyNeighbors, 
                             &polygonNeighborhood[t].polygons, 
                             &polygonNeighborhood[t].noPolygons);
         
         const Neighborhood &polyNeighborsOfT = polygonNeighborhood[t];
         for(unsigned i = polyNeighborsOfT.noPolygons; i--;) {
            unsigned neighborIndex = polyNeighborsOfT.polygons[i];
            
            bool add = true;
            for(unsigned j = 3; j--;)
               add = (add && (neighborIndex != poly.data[j]));
            
            if (add) {
               // Add vertices of current polygon to vertex neighbors
               for(unsigned k = 3; k--;)
                  vertNeighbors.insert(polygons[neighborIndex].data[k]);
            }
         }
         
         _createArrayFromSet(vertNeighbors, 
                             &polygonNeighborhood[t].vertices, 
                             &polygonNeighborhood[t].noVertices);
      }
   }
   
   return true;
}

void MeshUtils::_createArrayFromSet(const set<unsigned> &toAdd, 
                                    unsigned **array, unsigned *size)
{
   set<unsigned>::const_iterator iter;
   
   *size  = toAdd.size();
   *array = new unsigned[*size];
   unsigned *arr = *array;
   
   for(iter = toAdd.begin(); iter != toAdd.end(); iter++)
      *arr++ = *iter;
}

bool MeshUtils::isPointInPolygon(const Vector2 &p, 
                                 const Vector2List &curve)
{
   const unsigned n = curve.size();
   bool odd = false;
   
   for(unsigned i = 0, j = n - 1; i < n; j = i++) {
      if (((curve[i][1] < p[1] && curve[j][1] >= p[1]) || 
           (curve[j][1] < p[1] && curve[i][1] >= p[1])) && 
          (curve[i][0] + (p[1] - curve[i][1]) * (curve[j][0] - curve[i][0]) / 
           (curve[j][1] - curve[i][1]) < p[0]))
      {
         odd = !odd;
      }
   }
   
   return odd;
}

void MeshUtils::computeVertexShortestPaths(DifferentialSurface *surface, 
                                           unsigned fromVertex, 
                                           vector<unsigned> &P)
{
   vector<double> D;
   
   MeshUtils::computeVertexShortestPaths(surface, fromVertex, P, D);
}

void MeshUtils::computeVertexShortestPaths(DifferentialSurface *surface, 
                                           unsigned fromVertex, 
                                           vector<unsigned> &P, 
                                           vector<double> &D)
{
   const Mesh *mesh = surface->mesh;
   const unsigned noVertices = mesh->getNoVertices();
   const Vector3 *vertices   = mesh->getVertices();
   
   P.clear();
   P.resize(noVertices, -1);
   
   D.clear();
   D.resize(noVertices, INFINITY);
   
   // reverse comparator so that Q.top() returns the smallest element
   priority_queue<pairDI_t, vector<pairDI_t>, greater<pairDI_t> > Q;
   Q.push(pairDI_t(0.0, fromVertex));
   D[fromVertex] = 0.0;
   
   // Dijkstra's algorithm
   while(!Q.empty()) {
      const unsigned curVert = Q.top().second;
      const double dist = Q.top().first;
      Q.pop();
      
      const Neighborhood &neighbors = surface->vertexNeighborhood[curVert];
      
      // this check handles duplicates in the queue
      if (dist <= D[curVert]) {
         for(unsigned i = neighbors.noVertices; i--;) {
            const unsigned curNeighbor = neighbors.vertices[i];
            const double cost = (vertices[curVert] - vertices[curNeighbor]).getMagnitude();
            
            if (D[curVert] + cost < D[curNeighbor]) {
               D[curNeighbor] = D[curVert] + cost;
               Q.push(pairDI_t(D[curNeighbor], curNeighbor));
               P[curNeighbor] = curVert;
            }
         }
      }
   }
}

/*void MeshUtils::triangleDistanceOnMesh(DifferentialSurface *surface, 
                                       unsigned fromTri, 
                                       vector<double> &D, 
                                       vector<unsigned> &P)
{
   const Mesh *mesh = surface->mesh;
   const unsigned noTriangles    = mesh->getNoTriangles();
   const MeshTriangle *triangles = mesh->getTriangles();
   
   P.clear();
   P.resize(noTriangles, -1);
   
   D.clear();
   D.resize(noTriangles, DBL_MAX);
   
   // reverse comparator so that Q.top() returns the smallest element
   priority_queue<pairDI_t, vector<pairDI_t>, greater<pairDI_t> > Q;
   Q.push(pairDI_t(0.0, fromTri));
   D[fromTri] = 0.0;
   
   while(!Q.empty()) {
      const unsigned curVert = Q.top().second;
      const double dist = Q.top().first;
      Q.pop();
      
      const Neighborhood &neighbors = m_features->m_triangleNeighborhood[curVert];
      
      //this check handles duplicates in the queue
      if (dist <= D[curVert]) {
         for(unsigned i = neighbors.noPolygons; i--;) {
            const unsigned curNeighbor = neighbors.triangles[i];
            const double cost = euclideanTriangleDistance(curVert, curNeighbor);
            
            if (D[curVert] + cost < D[curNeighbor]) {
               D[curNeighbor] = D[curVert] + cost;					
               Q.push(pairDI_t(D[curNeighbor], curNeighbor));					
               P[curNeighbor] = curVert;
            }
         }
      }      
   }
   
#ifdef DEBUG
   for(unsigned i = D.size(); i--;)
      ASSERT(D[i] != DBL_MAX);
#endif
}*/

