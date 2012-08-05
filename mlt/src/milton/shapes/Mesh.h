/**<!-------------------------------------------------------------------->
   @class  Mesh
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Representation of a triangular mesh stored in the obj format.
   It consists of a list of vertices's and a list of faces that contain
   indices in the vertex list.
   <!-------------------------------------------------------------------->**/

#ifndef MESH_H_
#define MESH_H_

#include <shapes/Transformable.h>
#include <shapes/MeshTriangle.h>
#include <utils/PropertyMap.h>

#include <GL/gl.h>

namespace milton {

/// Temporary struct for loading mesh data
struct MILTON_DLL_EXPORT MeshData {
   std::vector<Vertex>       vertices;
   std::vector<Normal>       normals;
   std::vector<UV>           uvs;
   std::vector<MeshTriangle> triangles;
   
   std::string               fileName;
};

class SpatialAccel;

class MILTON_DLL_EXPORT Mesh : public Transformable, public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      /// Constructs a mesh out of the specified MeshData
      Mesh(const MeshData &data);
      
      /// Constructs a mesh with the specified vertex/triangle count
      Mesh(unsigned nVertices, unsigned nNormals, unsigned nUVs, 
           unsigned nTriangles);
      
      /// Copy constructor
      Mesh(const Mesh &mesh);
      
      /// Cleans up vertex and triangle data
      virtual ~Mesh();
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes this Mesh and its underlying SpatialAccel
       * 
       * @param forceUpdate if true, reinitializes this mesh, including 
       *    recreating the underlying SpatialAccel and dirtying this mesh's 
       *    OpenGL preview display list
       */
      virtual void init(bool forceUpdate);
      
      /**
       * @brief
       *    Initializes this Mesh and its underlying SpatialAccel
       */
      virtual void init();
      
      /**
       * @brief
       *    Displays an OpenGL preview of this mesh; optimized for repeated 
       * calls via a compiled OpenGL display list
       */
      virtual void preview();
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool   intersects(const Ray &ray, real_t tMax = INFINITY);
      
      /**
       * Computes the normals.  The vertex positions should be set.  The normals
       * will be averaged (Gourand Shading)
       */
      void computeNormals();
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates in 'pt'
       */
      virtual void getPoint(SurfacePoint &pt, const UV &uv);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       */
      virtual Point3 getPosition(const UV &uv);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      /// @returns the number of vertices
      inline unsigned getNoVertices() const {
         return m_nVertices;
      }
      
      /// @returns the number of normals
      inline unsigned getNoNormals() const {
         return m_nNormals;
      }
      
      /// @returns the number of uvs
      inline unsigned getNoUVs() const {
         return m_nUVs;
      }
      
      /// @returns the number of triangles
      inline unsigned getNoTriangles() const {
         return m_nTriangles;
      }
      
      /// @returns a pointer to the vertex data and the number of vertices
      inline Vertex *getVertices(unsigned &nVertices) const {
         nVertices = m_nVertices;
         
         return m_vertices;
      }
      
      /// @returns a pointer to the normal data and the number of normals
      inline Normal *getNormals(unsigned &nNormals) const {
         nNormals = m_nNormals;
         
         return m_normals;
      }
      
      /// @returns a pointer to the uv data and the number of uvs
      inline UV *getUVs(unsigned &nUVs) const {
         nUVs = m_nUVs;
         
         return m_uvs;
      }
      
      /// @returns a pointer to the triangle data and the number of triangles
      inline MeshTriangle *getTriangles(unsigned &nTriangles) const {
         nTriangles = m_nTriangles;
         
         return m_triangles;
      }
      
      /// @returns a pointer to the vertex data
      inline Vertex *getVertices() const {
         return m_vertices;
      }
      
      /// @returns a pointer to the normal data
      inline Normal *getNormals() const {
         return m_normals;
      }
      
      /// @returns a pointer to the uv data
      inline UV *getUVs() const {
         return m_uvs;
      }
      
      /// @returns a pointer to the triangle data
      inline MeshTriangle *getTriangles() const {
         return m_triangles;
      }
      
      void setPreviewDirty();
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the aggregate surface area of all triangles in this mesh
       */
      virtual real_t _getSurfaceArea();
      
   protected:
      unsigned	     m_nVertices;
      unsigned      m_nNormals;
      unsigned      m_nUVs;
      unsigned      m_nTriangles;
      
      Vertex       *m_vertices;
      Normal       *m_normals;
      UV           *m_uvs;
      MeshTriangle *m_triangles;
      
      // display list
      GLuint        m_batch;
      
      SpatialAccel *m_spatialAccel;
      
      bool          m_enableAccelPreview;
};

}

#endif // MESH_H_

