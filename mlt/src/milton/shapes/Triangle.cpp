/**<!-------------------------------------------------------------------->
   @file   Triangle.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Basic representation of a Triangle with optional UV coordinates
   @see also Triangle.h which defines a Triangle class used within meshes
      and differs from Triangle in that it is a standalone Shape 
      and doesn't depend on a parent Mesh to exist
   <!-------------------------------------------------------------------->**/

#include "Triangle.h"

#include <SurfacePoint.h>
#include <Random.h>
#include <AABB.h>
#include <GL/gl.h>

namespace milton {

void Triangle::_getUV(SurfacePoint &pt) const {
   Point3  p;
   _transformPoint3WorldToObj(pt.position, p);
   const Vector3 &v1 = vertices[0];
   const Vector3 &v2 = vertices[1];
   const Vector3 &v3 = vertices[2];
   const Vector3 p0(p.data);
   
   // note actual inverse area is 2 / expr, but the 2's cancel out in pA and pB
   const real_t A = 1.0 / (v2 - v1).cross(v3 - v1).getMagnitude();
   
   // baryocentric coordinates of UV using ratios of subtriangle areas
   const real_t pA = (v2 - p0).cross(v3 - p0).getMagnitude() * A;
   const real_t pB = (p0 - v1).cross(v3 - v1).getMagnitude() * A;
   const real_t pC = 1 - pA - pB;
   
   pt.uv.u = pA * uvs[0].u + pB * uvs[1].u + pC * uvs[2].u;
   pt.uv.v = pA * uvs[0].v + pB * uvs[1].v + pC * uvs[2].v;
}

void Triangle::_getGeometricNormal(SurfacePoint &pt) const {
   const Vector3 &nObj = getNormal();
   
   _transformVector3ObjToWorld(nObj, pt.normalG);
}

void Triangle::preview() {
   Transformable::preview();
   
   glBegin(GL_TRIANGLES);
   
   for(unsigned i = 0; i < 3; ++i) {
      glNormal3real_tv(normals[i].data);
      glTexCoord2real_t(uvs[i].u, uvs[i].v);
      glVertex3real_tv(vertices[i].data);
   }
   
   glEnd();
}

/* Returns the centroid of the triangle */
Vector3 Triangle::getCentroid() const {
   const Vertex &v1 = vertices[0];
   const Vertex &v2 = vertices[1];
   const Vertex &v3 = vertices[2];
   
   return (v1 + v2 + v3) / 3;
}

void Triangle::init() {
   m_objSpaceAABB = AABB();
   m_objSpaceAABB.add(vertices[0]);
   m_objSpaceAABB.add(vertices[1]);
   m_objSpaceAABB.add(vertices[2]);
   
#if (0 == TRIANGLE_INTERSECTION_TYPE)
   /* Compute geometric normal */
   const Vector3 &b = vertices[2] - vertices[0];
   const Vector3 &c = vertices[1] - vertices[0];
   Vector3 n = c.cross(b);
   
   n_k = n.getMaxDimension();
   ASSERT(n[n_k] != 0);
   
   n /= n[n_k];
   n[n_k] = 1.0;
   
   const int u = (n_k + 1) % 3;
   const int v = (n_k + 2) % 3;
   
   const real_t bx = vertices[2][u] - vertices[0][u];
   const real_t by = vertices[2][v] - vertices[0][v];
   const real_t cx = vertices[1][u] - vertices[0][u];
   const real_t cy = vertices[1][v] - vertices[0][v];
   
   const real_t bot = 1.0 / (bx*cy - by*cx);
   
   n_u = n[u];
   n_v = n[v];
   n_d = vertices[0].dot(n);
   
   b_u = -by * bot;
   b_v =  bx * bot;
   b_d = (by*vertices[0][u] - bx*vertices[0][v]) * bot;
   
   c_u =  cy * bot;
   c_v = -cx * bot;
   c_d = (vertices[0][v]*cx - vertices[0][u]*cy) * bot;
#endif
   
   Transformable::init();
}

#if (0 == TRIANGLE_INTERSECTION_TYPE)

real_t Triangle::getIntersection(const Ray &ray, SurfacePoint &pt) {
   static int mod5[] = {0, 1, 2, 0, 1};
   
   Point3  origin;
   Vector3 dir;
   _transformRayWorldToObj(ray, P, D);
   
#define ku mod5[n_k + 1]
#define kv mod5[n_k + 2]
   
   const real_t nd = 1.0 / (dir[n_k] + n_u * dir[ku] + n_v * dir[kv]);
   
   const real_t t = (n_d - origin[n_k] -  n_u*origin[ku] - n_v*origin[kv]) * nd;
   
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
   
   return t;
}

#elif (1 == TRIANGLE_INTERSECTION_TYPE)
// Taken from chapter 5 of Real Time Collision Detection
// TODO: optimize!!

// 4 dot, 2 cross, 1 sub, 1 negation
real_t Triangle::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  origin;
   Vector3 direction;
   _transformRayWorldToObj(ray, origin, direction);
   direction = -direction;
   
   const Vertex   &a = vertices[0];
   const Vertex   &b = vertices[1];
   const Vertex   &c = vertices[2];
   const Vector3 &ab = b - a;
   const Vector3 &ac = c - a;
   
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

#else // 2 == TRIANGLE_INTERSECTION_TYPE

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
real_t Triangle::getIntersection(const Ray &ray, SurfacePoint &pt) {
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   const real_t *const orig  = P.data;
   const real_t *const dir   = D.data;
   
   const real_t *const vert0 = vertices[0].data;
   const real_t *const vert1 = vertices[1].data;
   const real_t *const vert2 = vertices[2].data;
   
   real_t edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   
   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);
   
   /* begin calculating determinant - also used to calculate U parameter 
*/
   CROSS(pvec, dir, edge2);
   
   /* if determinant is near zero, ray lies in plane of triangle */
   const real_t det = DOT(edge1, pvec);
   
#if 1           /* define if culling is desired */
   // this branch takes forward and backward-facing triangles into account
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
   // this branch takes forward facing triangles into account
   if (EQ(det, 0))
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
     return 0;
   
   /* calculate t, ray intersects triangle */
   return (DOT(edge2, qvec) * inv_det);
#endif // 0
}

#endif // TRIANGLE_INTERSECTION_TYPE



/* returns the geometric surface normal in object space */
Vector3 Triangle::getNormal() const {
   const Vertex &v21 = vertices[1] - vertices[0];
   const Vertex &v31 = vertices[2] - vertices[0];
   
   return v21.cross(v31).getNormalized();
}

void Triangle::getRandomPoint(SurfacePoint &pt) {
   Vector3 randBary(Random::sample(0, 1), 
                    Random::sample(0, 1), 
                    Random::sample(0, 1));
   
   const real_t sum = randBary.getSum();
   ASSERT(sum > 0);
   randBary /= sum;
   
   const Vertex &v1 = vertices[0];
   const Vertex &v2 = vertices[1];
   const Vertex &v3 = vertices[2];
   
   pt.position = m_transToWorld * 
      Point3((v1 * randBary[0] + v2 * randBary[1] + v3 * randBary[2]).data);
   
   _getUV(pt);
}

Point3 Triangle::getPosition(const UV &uv) {
   /*real_t x = CLAMP(.5 + tan((1.0 - uv.u) / (2 * M_PI)), 0, 1);
   real_t y = CLAMP(.5 + sin((0.5 - uv.v) / M_PI) * 0.5, 0, 1);
   real_t z = sqrt(1 - x * x - y * y);
   return m_transToWorld * Point3((v1 * x + v2 * y + v3 * z).data);*/
   
   const Vector2 a(uvs[1].u - uvs[0].u, uvs[1].v - uvs[0].v);
   const Vector2 b(uvs[2].u - uvs[0].u, uvs[2].v - uvs[0].v);
   
   const Vector2 c(uv.u - uvs[0].u, uv.v - uvs[0].v);
   const Vector2 d(uv.u - uvs[0].u, uv.v - uvs[0].v);
   
   const real_t A  = 1.0 / a.cross(b).getMagnitude();
   const real_t pB = c.cross(b).getMagnitude() * A;
   const real_t pC = d.cross(a).getMagnitude() * A;
   const real_t pA = 1 - pB - pC;
   
   const Vertex &v1 = vertices[0];
   const Vertex &v2 = vertices[1];
   const Vertex &v3 = vertices[2];
   
   return m_transToWorld * Point3((v1 * pA + v2 * pB + v3 * pC).data);
}

real_t Triangle::_getSurfaceArea() {
   const Vertex &v1 = vertices[0];
   const Vertex &v2 = vertices[1];
   const Vertex &v3 = vertices[2];
   
   return 0.5 * (v2 - v1).cross(v3 - v1).getMagnitude();
}

}

