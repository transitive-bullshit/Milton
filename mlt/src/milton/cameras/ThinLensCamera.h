/**<!-------------------------------------------------------------------->
   @class  ThinLensCamera
   @author Travis Fischer (fisch0920@gmail.com)
   @date   January 2008
   
   @brief
      Thin lens camera approximation supporting common camera inputs and 
   allowing for depth of field
   
   @param
      aperture - diameter of the aperature of the lens in mm (eg. 35mm, 50mm)
   @param
      fstop    - (aka F number), is defined as the unitless ratio of the focal 
                 length to the diameter of the aperture of the lens
   @param
      focalDistance - the distance in meters between the lens and the focal 
                      plane (objects lying on the focal plane will be in 
                      perfect focus)
   @param
      focalPoint    - (optional) user may specify a focalPoint in NDC on the 
                      film-plane instead of specifying a focalDistance to 
                      "auto-focus" the object intersected by a ray traced 
                      through the corresponding pixel, whose distance away 
                      from the lens will be used as the focalDistance.
                      @note focalPoint is expressed as a vector2 in the unit 
                      square [0,1]^2, where [0,0] is the upper left corner 
                      of the image, and [1,1] is the lower right corner
   
   (focal length is derived from fstop and aperture)
   
   @see http://www.eecs.berkeley.edu/~barsky/VisRendPapers/survey1.pdf
   <!-------------------------------------------------------------------->**/

#ifndef THIN_LENS_CAMERA_H_
#define THIN_LENS_CAMERA_H_

#include "cameras/Camera.h"

namespace milton {

class MILTON_DLL_EXPORT ThinLensCamera : public Camera {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      ThinLensCamera();
      virtual ~ThinLensCamera();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init(Scene *scene);
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual Ray    getWorldRay(const Point2 &pt) const;
      
      virtual Point2 getProjection(const Point3 &p) const;
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual void   fillGLMatrix(real_t *buffer);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual void setOrientation(const Point3  &eye,
                                  const Vector3 &look, 
                                  const Vector3 &up);
      
      inline Point3 getEye() const {
         return m_eye;
      }
      
      inline Vector3 getUp() const {
         return m_v;
      }
      
      inline Vector3 getLook() const {
         return -m_n;
      }
      
      inline Vector3 getU() const {
         return m_u;
      }
      
      inline Vector3 getV() const {
         return m_v;
      }
      
      inline Vector3 getN() const {
         return m_n;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      void _computeMatrix();
      
   protected:
      Matrix4x4     m_filmToWorld;
      Matrix4x4     m_worldToFilm;
      
      /// Position
      Point3        m_eye;
      
      /// Camera's internal right-handed coordinate system
      Vector3       m_u, m_v, m_n;
      
      /// Clipping planes
      real_t        m_near, m_far;
      
      /// View volume parameters
      real_t        m_aspect;
      real_t        m_hAngle;
      
      /// Aperture diameter (input in mm; stored internally in meters)
      real_t        m_aperture;
      
      /// F-stop or F-number; unitless
      real_t        m_fstop;
      
      /// Focal length in meters
      real_t        m_focalLength;
      
      /// Focal distance in meters
      real_t        m_focalDistance;
};

}

#endif // THIN_LENS_CAMERA_H_

