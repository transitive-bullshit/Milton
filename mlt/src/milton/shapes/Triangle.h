/**<!-------------------------------------------------------------------->
   @class  Triangle
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Basic representation of a Triangle with optional UV coordinates
   @see also MeshTriangle.h which defines a MeshTriangle class used within meshes
      and differs from Triangle in that it is a standalone Shape 
      and doesn't depend on a parent Mesh to exist
   <!-------------------------------------------------------------------->**/

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <shapes/Transformable.h>
#include <shapes/MeshTriangle.h>

//#define TRIANGLE_INTERSECTION_TYPE    0
//#define TRIANGLE_INTERSECTION_TYPE    1
#define TRIANGLE_INTERSECTION_TYPE    2

namespace milton {

class MILTON_DLL_EXPORT Triangle : public Transformable {
   public:
      
      ///@name Fast-access public data
      //@{-----------------------------------------------------------------
      
      Vertex vertices[3];
      Normal normals[3];
      UV     uvs[3];
      
      
      //@}-----------------------------------------------------------------
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline Triangle()
      { }
      
      inline Triangle(Vertex *vertices_, Normal *normals_ = NULL, 
                      UV *uvs_ = NULL)
      {
         for(int i = 3; i--;) {
            if (vertices_)
               vertices[i] = vertices_[i];
            if (normals_)
               normals[i] = normals_[i];
            if (uvs_)
               uvs[i] = uvs_[i];
         }
      }
      
      inline Triangle(const Vertex &vA, const Vertex &vB, const Vertex &vC)
      {
         vertices[0] = vA;
         vertices[1] = vB;
         vertices[2] = vC;
         
         const Normal &normal = ((vB - vA).cross(vC - vA)).getNormalized();
         normals[0]  = normal;
         normals[1]  = normal;
         normals[2]  = normal;
         uvs[0]      = UV(0, 0);
         uvs[1]      = UV(0, 1);
         uvs[2]      = UV(1, 1);
      }
      
      inline Triangle(const Vertex &vA, const Vertex &vB, const Vertex &vC, 
                      const UV     &tA, const UV     &tB, const UV     &tC)
      {
         vertices[0] = vA;
         vertices[1] = vB;
         vertices[2] = vC;
         
         const Normal &normal = ((vB - vA).cross(vC - vA)).getNormalized();
         normals[0]  = normal;
         normals[1]  = normal;
         normals[2]  = normal;
         uvs[0]      = tA;
         uvs[1]      = tB;
         uvs[2]      = tC;
      }
      
      inline Triangle(const Vertex &vA, const Vertex &vB, const Vertex &vC, 
                      const Normal &nA, const Normal &nB, const Normal &nC, 
                      const UV     &tA, const UV     &tB, const UV     &tC)
      {
         vertices[0] = vA;
         vertices[1] = vB;
         vertices[2] = vC;
         normals[0]  = nA;
         normals[1]  = nB;
         normals[2]  = nC;
         uvs[0]      = tA;
         uvs[1]      = tB;
         uvs[2]      = tC;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /// Initializes this triangle for intersection tests
      virtual void init();
      
      virtual void preview();
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the normal of this triangle in object space
       */
      Vector3 getNormal() const;
      
      /**
       * @returns the centroid (center) of the triangle in object space
       */
      Vector3 getCentroid() const;
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a point chosen uniformly random over the surface of this 
       *    shape in 'pt'
       * @note the probability density of selecting this point is assumed to 
       *    be (1.0 / getSurfaceArea())
       */
      virtual void getRandomPoint(SurfacePoint &pt);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       */
      virtual Point3 getPosition(const UV &uv);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
      
      /**
       * @returns the total surface area of this shape
       */
      virtual real_t _getSurfaceArea();
      
   protected:
#if (0 == TRIANGLE_INTERSECTION_TYPE)
      /// Precomputed data for fast intersection using baryocentric coordinates
      real_t n_u, n_v, n_d;
      real_t b_u, b_v, b_d;
      real_t c_u, c_v, c_d;  // 8 * 9 + 4 = 76 extra bytes per triangle :(
      int n_k;
#endif
};

}

#endif // TRIANGLE_H_

