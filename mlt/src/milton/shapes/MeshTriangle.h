/**<!-------------------------------------------------------------------->
   @class  MeshTriangle
   @author Travis Fischer (fisch0920@gmail.com)
   @author Nong Li (nongli@gmail.com)
   @date   Spring 2008
   
   @brief
      Basic representation of a MeshTriangle with optional UV coordinates
   @see also Triangle.h which defines a Triangle class which derives from 
      Shape and differs from MeshTriangle in that it is a standalone Shape 
      and doesn't depend on a parent Mesh to exist
   <!-------------------------------------------------------------------->**/

#ifndef MESH_TRIANGLE_H_
#define MESH_TRIANGLE_H_

#include <shapes/Intersectable.h>
#include <core/UV.h>

namespace milton {

//#define MESH_TRIANGLE_INTERSECTION_TYPE    0
//#define MESH_TRIANGLE_INTERSECTION_TYPE    1
#define MESH_TRIANGLE_INTERSECTION_TYPE    2

typedef Vector3 Vertex;
typedef Vector3 Normal;

class  Mesh;
struct Ray;

class MILTON_DLL_EXPORT MeshTriangle : public Intersectable {
   public:
      
      ///@name Fast-access public data
      //@{-----------------------------------------------------------------
      
      union {
         struct {
            unsigned A, B, C;     // 0-indexed to vertex list
         };
         
         unsigned data[3];
      };
      
      union {
         struct {
            unsigned nA, nB, nC;  // 0-indexed to normal list
         };
         
         unsigned nData[3];
      };
      
      union {
         struct {
            unsigned tA, tB, tC;  // 0-indexed to texture list
         };
         
         unsigned tData[3];
      };
      
      Mesh *mesh;
      
      
      //@}-----------------------------------------------------------------
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MeshTriangle() 
         : mesh(NULL)
      {
         A  =  B =  C = 0;
         nA = nB = nC = 0;
         tA = tB = tC = 0;
      }
      
      inline MeshTriangle(unsigned A_, unsigned B_, unsigned C_,
                          unsigned nA_ = 0, unsigned nB_ = 0, unsigned nC_ = 0,
                          unsigned tA_ = 0, unsigned tB_ = 0, unsigned tC_ = 0)
         : A(A_),   B(B_),   C(C_), 
           nA(nA_), nB(nB_), nC(nC_), 
           tA(tA_), tB(tB_), tC(tC_), 
           mesh(NULL)
      { }
      
      inline MeshTriangle(const MeshTriangle &copy)
         : A(copy.A),   B(copy.B),   C(copy.C), 
           nA(copy.nA), nB(copy.nB), nC(copy.nC), 
           tA(copy.tA), tB(copy.tB), tC(copy.tC), 
           mesh(copy.mesh)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      /// Initializes this triangle for intersection tests
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual AABB getAABB() const;
      
      Vector3 getNormal() const;
      
      Vector3 getBaryocentricCoords(const Point3 &p) const;
      
      Vector3 getCenter() const;
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a point chosen uniformly random over the surface of this 
       *    shape in 'pt'
       * @note the probability density of selecting this point is assumed to 
       *    be (1.0 / getSurfaceArea())
       */
      void getRandomPoint(SurfacePoint &pt);
      
      /**
       * @returns the point on the surface of this shape corresponding to the 
       *    given UV coordinates
       */
      Point3 getPosition(const UV &uv);
      
      /**
       * @returns the total surface area of this shape
       */
      real_t getSurfaceArea() const;
      
      
      //@}-----------------------------------------------------------------
};

class MILTON_DLL_EXPORT MeshTriangleFast : public MeshTriangle {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MeshTriangleFast()
         : MeshTriangle()
      { }
      
      inline MeshTriangleFast(unsigned A_, unsigned B_, unsigned C_,
                              unsigned nA_ = 0, unsigned nB_ = 0, unsigned nC_ = 0,
                              unsigned tA_ = 0, unsigned tB_ = 0, unsigned tC_ = 0)
         : MeshTriangle(A_,  B_,  C_, 
                        nA_, nB_, nC_, 
                        tA_, tB_, tC_)
      { }
      
      inline MeshTriangleFast(const MeshTriangle &copy)
         : MeshTriangle(copy)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Core functionality
      //@{-----------------------------------------------------------------
      
      virtual void   init();
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      
      //@}-----------------------------------------------------------------
      
   private:
      /// Precomputed data for fast intersection using baryocentric coordinates
      real_t n_u, n_v, n_d;
      real_t b_u, b_v, b_d;
      real_t c_u, c_v, c_d;  // 8 * 9 + 4 = 76 extra bytes per triangle :(
      unsigned n_k;
};

}

#endif // MESH_TRIANGLE_H_

