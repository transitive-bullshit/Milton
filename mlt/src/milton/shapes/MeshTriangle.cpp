/**<!-------------------------------------------------------------------->
   @file   MeshTriangle.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Basic representation of a MeshTriangle with optional UV coordinates
   @see also Triangle.h which defines a Triangle class which derives from 
      Shape and differs from MeshTriangle in that it is a standalone Shape 
      and doesn't depend on a parent Mesh to exist
   <!-------------------------------------------------------------------->**/

#include "MeshTriangle.h"
#include "Mesh.h"

#include <SurfacePoint.h>
#include <Random.h>
#include <Ray.h>

namespace milton {


#if (1 == MESH_TRIANGLE_INTERSECTION_TYPE)
// Taken from chapter 5 of Real Time Collision Detection
// TODO: optimize!!

// 4 dot, 2 cross, 1 sub, 1 negation
real_t MeshTriangle::getIntersection(const Ray &ray, SurfacePoint &pt) {
   const Vertex *vertices = mesh->getVertices();
   const Vertex   &a = vertices[A];
   const Vertex   &b = vertices[B];
   const Vertex   &c = vertices[C];
   const Vector3 &ab = b - a;
   const Vector3 &ac = c - a;
   
   const Point3 &origin     = ray.origin;
   const Vector3 &direction = -ray.direction;
   
   // Compute triangle normal. Can be precalculated or cached if
   // intersecting multiple segments against the same triangle
   const Vector3 &n = ab.cross(ac);
   
   // Compute denominator d. If d <= 0, segment is parallel to or points
   // away from triangle, so exit early
   const real_t d = direction.dot(n);
   if (d <= 0.0)
      return INFINITY;
   
   // Compute intersection t value of pq with plane of triangle. A ray
   // intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
   // dividing by d until intersection has been found to pierce triangle
   const Vector3 ap(origin[0] - a[0], origin[1] - a[1], origin[2] - a[2]);
   real_t t = ap.dot(n);
   if (t < 0.0)
      return INFINITY;
   
   // Compute barycentric coordinate components and test if within bounds
   const Vector3 &e = direction.cross(ap);
   real_t v = ac.dot(e);
   if (v < 0.0 || v > d)
      return INFINITY;
   
   real_t w = -ab.dot(e);
   if (w < 0.0 || v + w > d)
      return INFINITY;
   
   // Segment/ray intersects triangle. Perform delayed division and
   // compute the last barycentric coordinate component
   real_t ood = 1.0 / d;
   t *= ood;
   //v *= ood;
   //w *= ood;
   //real_t u = 1.0 - v - w;
   
   return t;
}

#else // 2 == MESH_TRIANGLE_INTERSECTION_TYPE

#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

// 4 DOT, 3 SUB, 2 CROSS
real_t MeshTriangle::getIntersection(const Ray &ray, SurfacePoint &pt) {
   const real_t *const orig  = ray.origin.data;
   const real_t *const dir   = ray.direction.data;
   
   const Vertex *vertices    = mesh->getVertices();
   const real_t *const vert0 = vertices[A].data;
   const real_t *const vert1 = vertices[B].data;
   const real_t *const vert2 = vertices[C].data;
   
   real_t edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   
   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);
   
   /* begin calculating determinant - also used to calculate U parameter 
*/
   CROSS(pvec, dir, edge2);
   
   /* if determinant is near zero, ray lies in plane of triangle */
   const real_t det = DOT(edge1, pvec);
   
#if 0           /* define if culling is desired */
   // this branch takes forward facing triangles into account
   if (det < EPSILON)
      return INFINITY;
   
   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);
   
   /* calculate U parameter and test bounds */
   const real_t u = DOT(tvec, pvec);
   if (u < 0.0 || u > det)
      return INFINITY;
   
   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);
   
   /* calculate V parameter and test bounds */
   const real_t v = DOT(dir, qvec);
   if (v < 0.0 || u + v > det)
      return INFINITY;
   
   /* calculate t, scale parameters, ray intersects triangle */
   real_t t = DOT(edge2, qvec);
   const real_t inv_det = 1.0 / det;
   t *= inv_det;
   //*u *= inv_det;
   //*v *= inv_det;
   return t;
#else                    /* the non-culling branch */
   // this branch takes forward and backward-facing triangles into account
   if (EQ(det, create_real(0)))
     return INFINITY;
   
   const real_t inv_det = 1.0 / det;
   
   /* prepare to test V parameter */
   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);
   
   /* calculate U parameter and test bounds */
   const real_t u = DOT(tvec, pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
     return INFINITY;
   
   CROSS(qvec, tvec, edge1);
   
   /* calculate V parameter and test bounds */
   const real_t v = DOT(dir, qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
     return INFINITY;
   
   /* calculate t, ray intersects triangle */
   return (DOT(edge2, qvec) * inv_det);
#endif // 0
}

#endif // MESH_TRIANGLE_INTERSECTION_TYPE


/* Returns the geometric surface normal */
Vector3 MeshTriangle::getNormal() const {
   const Vertex *vertices = mesh->getVertices();
   const Vertex &v1 = vertices[A];
   const Vertex &v2 = vertices[B];
   const Vertex &v3 = vertices[C];
   
   const Vertex &v21 = v2 - v1;
   const Vertex &v31 = v3 - v1;
   
   return v21.cross(v31).getNormalized();
}

AABB MeshTriangle::getAABB() const {
   AABB aabb;
   
   const Vertex *vertices = mesh->getVertices();
   const unsigned no = mesh->getNoVertices();
   ASSERT(A < no && B < no && C < no);
   
   aabb.add(vertices[A]);
   aabb.add(vertices[B]);
   aabb.add(vertices[C]);
   
   return aabb;
}


/* Fast triangle intersection with precomputed baryocentric coordinates
 * -------------------------------------------------------------------- */

void MeshTriangleFast::init() {
   ASSERT(mesh);
   
   const Vertex *vertices = mesh->getVertices();
   ASSERT(vertices);
   
   const Vertex &vA = vertices[A];
   const Vertex &vB = vertices[B];
   const Vertex &vC = vertices[C];
   
   // compute geometric normal
   const Vector3 &b = vC - vA;
   const Vector3 &c = vB - vA;
   Vector3 n = c.cross(b);
   
   n_k = n.getMaxDimension();
   ASSERT(n[n_k] != 0);
   
   n /= n[n_k];
   n[n_k] = 1.0;
   
   int u = (n_k + 1) % 3;
   int v = (n_k + 2) % 3;
   
   const real_t bx = b[u];
   const real_t by = b[v];
   const real_t cx = c[u];
   const real_t cy = c[v];
   
   const real_t bot = 1.0 / (bx*cy - by*cx);
   
   n_u = n[u];
   n_v = n[v];
   n_d = vA.dot(n);
   
   b_u = -by * bot;
   b_v =  bx * bot;
   b_d = (by*vA[u] - bx*vA[v]) * bot;
   
   c_u =  cy * bot;
   c_v = -cx * bot;
   c_d = (vA[v]*cx - vA[u]*cy) * bot;
}

real_t MeshTriangleFast::getIntersection(const Ray &ray, SurfacePoint &pt) {
   static unsigned mod5[] = {0, 1, 2, 0, 1};
   const Point3  &origin = ray.origin;
   const Vector3 &dir    = ray.direction;
   
#define ku mod5[n_k + 1]
#define kv mod5[n_k + 2]
   
   const real_t nd = 1.0 / (dir[n_k] + n_u * dir[ku] + n_v * dir[kv]);
   const real_t t  = nd * (n_d - origin[n_k] -  n_u*origin[ku] - n_v*origin[kv]);
   
   if (t < EPSILON)
      return INFINITY;
   
   const real_t hu = origin[ku] + t * dir[ku];
   const real_t hv = origin[kv] + t * dir[kv];
   
   const real_t lambda = hu*b_u + hv*b_v + b_d;
   
   if (lambda < -EPSILON)
      return INFINITY;
   
   const real_t mue = hu * c_u + hv * c_v + c_d;
   
   if ((mue < -EPSILON) || (lambda + mue > EPSILON + 1.0))
      return INFINITY;
   
   //pt.uv.u = lambda;
   //pt.uv.v = mue;
   
   return t;
}

void MeshTriangle::getRandomPoint(SurfacePoint &pt) {
   Vector3 randBary(Random::sample(0, 1), 
                    Random::sample(0, 1), 
                    Random::sample(0, 1));
   
   const real_t sum = randBary.getSum();
   ASSERT(sum > 0);
   randBary /= sum;
   
   const Vertex *vertices = mesh->getVertices();
   const Vertex &v1 = vertices[A];
   const Vertex &v2 = vertices[B];
   const Vertex &v3 = vertices[C];
   
   pt.position = 
      Point3((v1 * randBary[0] + v2 * randBary[1] + v3 * randBary[2]).data);
}

Point3 MeshTriangle::getPosition(const UV &uv) {
#if 0
   const UV *uvs = mesh->getUVs();
   const UV &uv0 = uvs[tA];
   const UV &uv1 = uvs[tB];
   const UV &uv2 = uvs[tC];
   
   const Vector2 a(uv1.u - uv0.u, uv1.v - uv0.v);
   const Vector2 b(uv2.u - uv0.u, uv2.v - uv0.v);
   
   const Vector2 c(uv.u - uv0.u, uv.v - uv0.v);
   const Vector2 d(uv.u - uv0.u, uv.v - uv0.v);
   
   const real_t A  = 1.0 / a.cross(b).getMagnitude();
   const real_t pB = c.cross(b).getMagnitude() * A;
   const real_t pC = d.cross(a).getMagnitude() * A;
   const real_t pA = 1 - pB - pC;
#endif
   
   Vector3 randBary(uv.u, 
                    uv.v, 
                    Random::sample(0, 1));
   
   const real_t sum = randBary.getSum();
   ASSERT(sum > 0);
   randBary /= sum;
   
   const Vertex *vertices = mesh->getVertices();
   const Vertex &v1 = vertices[A];
   const Vertex &v2 = vertices[B];
   const Vertex &v3 = vertices[C];
   
   return 
      Point3((v1 * randBary[0] + v2 * randBary[1] + v3 * randBary[2]).data);
}

real_t MeshTriangle::getSurfaceArea() const {
   const Vertex *vertices = mesh->getVertices();
   const Vertex &v1 = vertices[A];
   const Vertex &v2 = vertices[B];
   const Vertex &v3 = vertices[C];
   
   return 0.5 * (v2 - v1).cross(v3 - v1).getMagnitude();
}

Vector3 MeshTriangle::getBaryocentricCoords(const Point3 &p) const {
   const Vertex *vertices = mesh->getVertices();
   const Vector3 &v1 = vertices[A];
   const Vector3 &v2 = vertices[B];
   const Vector3 &v3 = vertices[C];
   const Vector3 p0(p.data);
   
   // note actual inverse area is 2 / expr, but the 2's cancel out in pA and pB
   const real_t A = 1.0 / (v2 - v1).cross(v3 - v1).getMagnitude();
   
   // baryocentric coordinates of UV using ratios of subtriangle areas
   const real_t pA = (v2 - p0).cross(v3 - p0).getMagnitude() * A;
   const real_t pB = (p0 - v1).cross(v3 - v1).getMagnitude() * A;
   const real_t pC = 1 - pA - pB;
   ASSERT(EQ(pA + pB + pC, create_real(1)));
   
   return Vector3(pA, pB, pC);
}

Vector3 MeshTriangle::getCenter() const {
   const Vertex *vertices = mesh->getVertices();
   const Vector3 &v1 = vertices[A];
   const Vector3 &v2 = vertices[B];
   const Vector3 &v3 = vertices[C];
   
   return (v1 + v2 + v3) / 3;
}

}

