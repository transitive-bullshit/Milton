/**<!-------------------------------------------------------------------->
   @file   Mesh.cpp
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Representation of a triangular mesh stored in the obj format.
   It consists of a list of vertices's and a list of faces that contain
   indices in the vertex list.
   <!-------------------------------------------------------------------->**/

#include "Mesh.h"
#include <SurfacePoint.h>
#include <kdTreeAccel.h>
#include <NaiveSpatialAccel.h>
#include <ResourceManager.h>
#include <Material.h>
#include <Random.h>
#include <QtCore/QtCore>

//#include <strings.h>
#include <float.h>

// converts a random real inbetween 0 and 1 into a valid triangle index
#define MESH_GET_INDEX(u)  (MIN((unsigned)floor((u) * m_nTriangles), m_nTriangles - 1))

namespace milton {

/* Constructs a mesh out of the specified MeshData */
Mesh::Mesh(const MeshData &data) {
   m_nVertices    = data.vertices.size();
   m_nNormals     = data.normals.size();
   m_nUVs         = data.uvs.size();
   m_nTriangles   = data.triangles.size();
   
   ASSERT(m_nVertices > 0);
   ASSERT(m_nTriangles > 0);
   ASSERT(m_nUVs >= 0);
   
   m_vertices	   = new Vertex[m_nVertices];
   m_normals	   = new Normal[m_nNormals];
   m_uvs          = new UV[m_nUVs];
   m_triangles    = new MeshTriangle[m_nTriangles];
   
   // TODO: heuristic for choosing MeshTriangle or MeshTriangleFast
   //if (m_nTriangles > (1 << 15)) {
   //   m_triangles = new MeshTriangle[m_nTriangles];
   //} else {
   //   m_triangles = new MeshTriangleFast[m_nTriangles];
   //}
   
   /* Copy data over from other mesh */
   if (m_nVertices > 0)
      memcpy(m_vertices,  &data.vertices[0],  sizeof(Vertex)       * m_nVertices);
   if (m_nNormals > 0)
      memcpy(m_normals,   &data.normals[0],   sizeof(Normal)       * m_nNormals);
   if (m_nUVs > 0)
      memcpy(m_uvs,       &data.uvs[0],       sizeof(UV)           * m_nUVs);
   if (m_nTriangles > 0)
      memcpy(m_triangles, &data.triangles[0], sizeof(MeshTriangle) * m_nTriangles);
   
   /*for(unsigned i = m_nVertices; i--;)
      m_vertices[i] = data.vertices[i];
   for(unsigned i = m_nNormals; i--;)
      m_normals[i] = data.normals[i];
   for(unsigned i = m_nUVs; i--;)
      m_uvs[i] = data.uvs[i];*/
   
   for(unsigned i = m_nTriangles; i--;) {
      //m_triangles[i] = data.triangles[i];
      m_triangles[i].mesh = this;
      
      /*if (m_triangles[i].A >= m_nVertices)
         cerr << i << ", A, " << m_triangles[i].A << endl;
      if (m_triangles[i].B >= m_nVertices)
         cerr << i << ", B, " << m_triangles[i].B << endl;
      if (m_triangles[i].C >= m_nVertices)
         cerr << i << ", C, " << m_triangles[i].C << endl;*/
      
      ASSERT(m_triangles[i].A < m_nVertices);
      ASSERT(m_triangles[i].B < m_nVertices);
      ASSERT(m_triangles[i].C < m_nVertices);
      
      if (m_nNormals > 0) {
         ASSERT(m_triangles[i].nA < m_nNormals);
         ASSERT(m_triangles[i].nB < m_nNormals);
         ASSERT(m_triangles[i].nC < m_nNormals);
      }
      if (m_nUVs > 0) {
         ASSERT(m_triangles[i].tA < m_nUVs);
         ASSERT(m_triangles[i].tB < m_nUVs);
         ASSERT(m_triangles[i].tC < m_nUVs);
      }
   }
   
   if (0 == m_nNormals)
      computeNormals();
   
   m_batch        = 0;
   m_spatialAccel = NULL;
}

Mesh::Mesh(unsigned nVertices, unsigned nNormals, unsigned nUVs, 
           unsigned nTriangles)
{
   ASSERT(nVertices > 0);
   ASSERT(nTriangles > 0);
   ASSERT(nUVs >= 0);  
   
   m_nVertices    = nVertices;
   m_nNormals     = nNormals;
   m_nUVs         = nUVs;
   m_nTriangles   = nTriangles;
   
   m_vertices	   = new Vertex[m_nVertices];
   m_normals	   = new Normal[m_nNormals];
   m_uvs          = new UV[m_nUVs];
   m_triangles    = new MeshTriangle[m_nTriangles];
   
   for(unsigned i = m_nTriangles; i--;) {
      m_triangles[i].mesh = this;
      
      ASSERT(m_triangles[i].A < m_nVertices);
      ASSERT(m_triangles[i].B < m_nVertices);
      ASSERT(m_triangles[i].C < m_nVertices);
      
      if (m_nNormals > 0) {
         ASSERT(m_triangles[i].nA < m_nNormals);
         ASSERT(m_triangles[i].nB < m_nNormals);
         ASSERT(m_triangles[i].nC < m_nNormals);
      }
      if (m_nUVs > 0) {
         ASSERT(m_triangles[i].tA < m_nUVs);
         ASSERT(m_triangles[i].tB < m_nUVs);
         ASSERT(m_triangles[i].tC < m_nUVs);
      }
   }
   
   if (0 == m_nNormals)
      computeNormals();
   
   m_batch        = 0;
   m_spatialAccel = NULL;
}

/* Copy constructor; does not copy kd-Trees or texture */
Mesh::Mesh(const Mesh &mesh) {
   m_nVertices    = mesh.m_nVertices;
   m_nNormals     = mesh.m_nNormals;
   m_nUVs         = mesh.m_nUVs;
   m_nTriangles   = mesh.m_nTriangles;      
   
   m_vertices	   = new Vertex[m_nVertices];
   m_normals	   = new Normal[m_nNormals];
   m_uvs          = new UV[m_nUVs];
   m_triangles    = new MeshTriangle[m_nTriangles];
   
   /* Copy data over from other mesh */
   memcpy(m_vertices,  mesh.m_vertices,  sizeof(Vertex)       * m_nVertices);
   memcpy(m_normals,   mesh.m_normals,   sizeof(Normal)       * m_nNormals);
   memcpy(m_uvs,       mesh.m_uvs,       sizeof(UV)           * m_nUVs);
   memcpy(m_triangles, mesh.m_triangles, sizeof(MeshTriangle) * m_nTriangles);
   
   for(unsigned i = m_nTriangles; i--;) {
      m_triangles[i].mesh = this;
      
      ASSERT(m_triangles[i].A < m_nVertices);
      ASSERT(m_triangles[i].B < m_nVertices);
      ASSERT(m_triangles[i].C < m_nVertices);
      
      if (m_nNormals > 0) {
         ASSERT(m_triangles[i].nA < m_nNormals);
         ASSERT(m_triangles[i].nB < m_nNormals);
         ASSERT(m_triangles[i].nC < m_nNormals);
      }
      if (m_nUVs > 0) {
         ASSERT(m_triangles[i].tA < m_nUVs);
         ASSERT(m_triangles[i].tB < m_nUVs);
         ASSERT(m_triangles[i].tC < m_nUVs);
      }
   }
   
   m_batch        = 0;
   m_spatialAccel = NULL;
   
   inherit(mesh);
}

Mesh::~Mesh() {
   safeDeleteArray(m_vertices);
   safeDeleteArray(m_normals);
   safeDeleteArray(m_uvs);
   safeDeleteArray(m_triangles);
   
   safeDelete(m_spatialAccel);
   
   if (m_batch != 0)
      glDeleteLists(m_batch, 1);
}

void Mesh::init(bool forceUpdate) {
   if (forceUpdate) {
      setPreviewDirty();
      safeDelete(m_spatialAccel);
   }
   
   init();
}

void Mesh::init() {
   if (!m_material) {
      m_material = new Material();
      (*m_material)["kd"] = SpectralSampleSet(1, 0, 0);
   }
   ASSERT(m_material);
   
   if (NULL == m_spatialAccel) {
      for(unsigned i = 0; i < m_nTriangles; ++i) {
         m_triangles[i].init();
         
         ASSERT(m_triangles[i].getAABB().isValid());
      }
      
      const std::string &accelType = getValue<std::string>("spatialAccel", 
                                                           "kdTree");
      
      IntersectableList *primitives = new IntersectableList(m_nTriangles);
      for(unsigned i = m_nTriangles; i--;)
         (*primitives)[i] = static_cast<Intersectable*>(&m_triangles[i]);
      
      if (accelType == "kdTree") {
         m_spatialAccel = new kdTreeAccel();
      } else {
         ASSERT(accelType == "naive");
         
         m_spatialAccel = new NaiveSpatialAccel();
      }
      
      m_spatialAccel->setGeometry(primitives);
      m_spatialAccel->inherit(*this);
      m_spatialAccel->init();
      
      m_objSpaceAABB = m_spatialAccel->getAABB();
      
      // transform AABB into parent coordinate system
      Transformable::init();
      
      //ResourceManager::log.info << "Mesh: " << m_aabb << endl;
   }
}

void Mesh::preview() {
   /*GLreal_t data[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, data);
	Matrix4x4 m = Matrix4x4(data).getTranspose();
   cerr << m << endl;*/
   
   bool enableAccelPreview = 
      ResourceManager::getValue<bool>("enableAccelPreview", true);
   bool flush = ((enableAccelPreview != m_enableAccelPreview) || 
                 ResourceManager::getValue<bool>("flushPreview", false));
   
   if (!m_material) {
      m_material = new Material();
      (*m_material)["kd"] = SpectralSampleSet(1, 0, 0);
   }
   
   Transformable::preview();
   
   /*glDisable(GL_CULL_FACE);
   glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);*/
   
   if (m_batch == 0 || flush) {
      if (m_batch != 0)
         glDeleteLists(m_batch, 1);
      
      m_batch = glGenLists(1);
      m_enableAccelPreview = enableAccelPreview;
      
      glNewList(m_batch, GL_COMPILE_AND_EXECUTE);
      
      if (m_enableAccelPreview && m_spatialAccel) {
         m_spatialAccel->preview();
      } else {
#ifdef DEBUG
         m_objSpaceAABB.preview();
#endif
      }
      
      glBegin(GL_TRIANGLES);
      
      for(unsigned i = m_nTriangles; i--;) {
         const MeshTriangle &tri = m_triangles[i];
         
         glNormal3real_tv(m_normals[tri.nA].data);
         glTexCoord2real_tv(m_uvs[tri.tA].data);
         glVertex3real_tv(m_vertices[tri.A].data);
         
         glNormal3real_tv(m_normals[tri.nB].data);
         glTexCoord2real_tv(m_uvs[tri.tB].data);
         glVertex3real_tv(m_vertices[tri.B].data);
         
         glNormal3real_tv(m_normals[tri.nC].data);
         glTexCoord2real_tv(m_uvs[tri.tC].data);
         glVertex3real_tv(m_vertices[tri.C].data);
      }
      
      /*glEnable(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_DOUBLE, 0, m_vertices);
      
      if (m_nNormals > 0) {
         glEnable(GL_NORMAL_ARRAY);
         glNormalPointer(GL_DOUBLE, 0, m_normals);
      }
      
      if (m_nUVs > 0) {
         glEnable(GL_TEXTURE_COORD_ARRAY);
         glTexCoordPointer(2, GL_DOUBLE, 0, m_uvs);
      }
      
      // won't work because triangle indices aren't consecutive in memory
      // (MeshTrianles)
      glDrawElements(GL_TRIANGLES, m_nTriangles, GL_UNSIGNED_INT, m_triangles);*/
      
      glEnd();
      glEndList();
   } else {
      glCallList(m_batch);
   }
}

/* Computes the normals.  The vertex positions should be set.  The normals
 * will be averaged (Gouraud Shading) */
void Mesh::computeNormals() {
   /*bool *normalFlip = new bool[m_nTriangles * 3];
   
   for (unsigned i = 0; i < m_nTriangles; i++) {
      const Vector3 &normal = m_triangles[i].getNormal(pt);
      const Vector3 &nA = m_normals[m_triangles[i].nA];
      const Vector3 &nB = m_normals[m_triangles[i].nB];
      const Vector3 &nC = m_normals[m_triangles[i].nC];
      normalFlip[i*3 + 0] = normal.dot(nA) < 0;
      normalFlip[i*3 + 1] = normal.dot(nB) < 0;
      normalFlip[i*3 + 2] = normal.dot(nC) < 0;
   }*/
   
   Normal *normals = new Normal[m_nVertices];
   m_nNormals = m_nVertices;
   
   for(unsigned i = 0; i < m_nTriangles; i++) {
      unsigned v1 = m_triangles[i].A;
      unsigned v2 = m_triangles[i].B;
      unsigned v3 = m_triangles[i].C;
      
      m_triangles[i].nA = v1;
      m_triangles[i].nB = v2;
      m_triangles[i].nC = v3;
      
      const Vector3 &normal = m_triangles[i].getNormal();
      
      normals[v1] += normal;
      normals[v2] += normal;
      normals[v3] += normal;
   }
   
   for(unsigned i = 0; i < m_nNormals; i++)
      normals[i].normalize();
   
   Normal *oldNormals = m_normals;
   m_normals = normals;
   
   setPreviewDirty();
   safeDeleteArray(oldNormals);
   
   //safeDeleteArray(normalFlip);
}

real_t Mesh::getIntersection(const Ray &ray, SurfacePoint &pt) {
   ASSERT(m_spatialAccel);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   real_t retVal = m_spatialAccel->getIntersection(Ray(P, D), pt);
   
   if (Ray::isValid(retVal)) {
      ASSERT(pt.index < m_nTriangles);
      pt.shape = this;
   }
   
   return retVal;
}

bool Mesh::intersects(const Ray &ray, real_t tMax) {
   ASSERT(m_spatialAccel);
   
   // TODO: creating a new Ray here is rather inefficient...
   Point3  P;
   Vector3 D;
   _transformRayWorldToObj(ray, P, D);
   
   return m_spatialAccel->intersects(Ray(P, D), tMax);
}

void Mesh::_getUV(SurfacePoint &pt) const {
   ASSERT(pt.index < m_nTriangles);
   
   if (m_nUVs > 0) {
      const UV &uv = m_uvs[m_triangles[pt.index].tA];
      // TODO: not fully implemented!
      
      pt.uv = uv;
   }
}

void Mesh::_getGeometricNormal(SurfacePoint &pt) const {
   ASSERT(pt.index < m_nTriangles);
   
   Point3 P;
   _transformPoint3WorldToObj(pt.position, P);
   
   const MeshTriangle &tri = m_triangles[pt.index];
   const Vector3 &coords   = tri.getBaryocentricCoords(P);
   const Normal &n = (coords[0] * m_normals[tri.nA] + 
                      coords[1] * m_normals[tri.nB] + 
                      coords[2] * m_normals[tri.nC]);
   
   _transformVector3ObjToWorld(n, pt.normalG);
   //_transformVector3ObjToWorld(m_triangles[pt.index].getNormal(), pt.normalG);
}

void Mesh::getPoint(SurfacePoint &pt, const UV &uv) {
   pt.index = MESH_GET_INDEX(uv.u);
   ASSERT(pt.index < m_nTriangles);
   
   Transformable::getPoint(pt, uv);
}

Point3 Mesh::getPosition(const UV &uv) {
   // uv.u determines which triangle
   const unsigned index = MESH_GET_INDEX(uv.u);
   
   return m_transToWorld * 
      m_triangles[index].getPosition(UV(Random::sample(0, 1), uv.v));
}

real_t Mesh::_getSurfaceArea() {
   real_t area = 0;
   
   // TODO: calculate this in world-space
   for(unsigned i = m_nTriangles; i--;) {
      const MeshTriangle &t = m_triangles[i];
      
      const Vertex &v1 = m_transToWorld * m_vertices[t.A];
      const Vertex &v2 = m_transToWorld * m_vertices[t.B];
      const Vertex &v3 = m_transToWorld * m_vertices[t.C];
      
      area += (v2 - v1).cross(v3 - v1).getMagnitude();
   }
   
   return 0.5 * area;
}

void Mesh::setPreviewDirty() {
   GLuint batch = m_batch;
   m_batch = 0;
   
   if (batch != 0)
      glDeleteLists(batch, 1);
}

}

