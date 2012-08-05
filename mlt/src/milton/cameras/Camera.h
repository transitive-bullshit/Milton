/**<!-------------------------------------------------------------------->
   @class  Camera
   @author Matthew Jacobs (jacobs.mh@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract camera that all Milton cameras subclass, representing a 
   mapping from film-space to world-space and vice-versa. Cameras may 
   optionally also specify an OpenGL preview projection matrix that can be 
   used by OpenGLRenderer.
      Film-space is represented by (slightly modified) Normalized Device 
   Coordinates (NDC), which are unit film-plane coordinates, with (x,y) both 
   ranging inbetween 0 and 1. NDC lie in the unit square [0,1]^2, where [0,0] 
   gets mapped to the upper left corner of the corresponding Viewport/Image, 
   and [1,1] is the lower right corner. NDC film-space allows for mapping 
   rendering results onto an arbitrarily-sized Viewport/Image.
   
   @see ThinLensCamera
   <!-------------------------------------------------------------------->**/

#ifndef CAMERA_H_
#define CAMERA_H_

#include <utils/PropertyMap.h>
#include <shapes/Shape.h>
#include <core/Ray.h>

namespace milton {

class  Scene;
struct Viewport;

class MILTON_DLL_EXPORT Camera : public Shape, public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Camera()
         : Shape(), PropertyMap()
      { }
      
      virtual ~Camera()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /// Initializes camera state from its PropertyMap
      virtual void init(Scene *scene) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns a world-space ray for the given point on image plane
       * @param pt is a point on the film plane expressed in NDC ([0,1]^2)
       */
      virtual Ray getWorldRay(const Point2 &pt) const = 0;
      
      /**
       * @returns the projection of the given world-space point onto the film-
       *    plane (result is expressed in NDC  ([0,1]^2))
       */
      virtual Point2 getProjection(const Point3 &p) const = 0;
      
      /**
       * @returns the projection of the given world-space point onto the film-
       *    plane (result is expressed in pixel coordinates on the specified
       *    Viewport @p v)
       */
      virtual Point2 getProjection(const Point3 &p, const Viewport &v) const;
      
      /**
       * @brief
       *    Fills the 16-element, column-major buffer with an OpenGL projection
       * matrix to preview the transformation of points from world-space to 
       * NDC using this camera model
       */
      virtual void fillGLMatrix(real_t *buffer) = 0;
       
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual void setOrientation(const Point3  &eye, 
                                  const Vector3 &look, 
                                  const Vector3 &up) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Sampling functionality
      //@{-----------------------------------------------------------------
      
      /**
       * @returns the point on the surface of this Camera corresponding to the 
       *    given UV coordinates
       */
      virtual Point3 getPosition(const UV &uv);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _getUV(SurfacePoint &pt) const;
      virtual void _getGeometricNormal(SurfacePoint &pt) const;
};

}

#endif // CAMERA_H_

