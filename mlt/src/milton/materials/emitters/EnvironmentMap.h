/**<!-------------------------------------------------------------------->
   @class  EnvironmentMap
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Environment map using latitude-longitude format.  An environment map 
   roughly models distant illumination surrounding a scene by conceptually 
   surrounding the scene with a spherical emitter that emits light inwards 
   according to a distribution defined by a 2D 'environment' texture map that 
   is projected onto the surface of the sphere.  Note the term HDR environment 
   map is thrown around a lot; the only difference is that the underlying 
   environment texture is stored in some type of high dynamic range format, 
   such as OpenEXR or HDR.
   <!-------------------------------------------------------------------->**/

#ifndef ENVIRONMENT_MAP_H_
#define ENVIRONMENT_MAP_H_

#include <common/image/Image.h>
#include <materials/Material.h>
#include <materials/Emitter.h>
#include <core/UV.h>

namespace milton {

class KernelFilter;

class MILTON_DLL_EXPORT EnvironmentMap : public Emitter {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   EnvironmentMap(Material *parent = NULL, 
                              ImagePtr image = ImagePtr())
         : Emitter(Material::s_nullSurfacePoint, NULL, parent), 
           m_filter(NULL), m_image(image)
      { }
      
      virtual ~EnvironmentMap()
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to use this environment map
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual SpectralSampleSet getLe(const Vector3 &w);
      
      /// Overridden to be empty
      virtual void preview(Shape *shape);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline ImagePtr &getImage() {
         return m_image;
      }
      
      inline void setImage(ImagePtr image) {
         m_image = image;
      }
      
      
      //@}-----------------------------------------------------------------
      ///@name Spherical <-> Planar mapping
      //@{-----------------------------------------------------------------
      
      virtual Vector2 getSphericalCoords(const Vector3 &w);
      virtual Vector2 getSphericalCoords(const UV &uv);
      virtual UV getUV(const Vector3 &w);
      virtual UV getUV(const Vector2 &coords);
      virtual Vector3 getDirection(const UV &uv);
      
      
      //@}------------------------------------------------------------------
      
   protected:
      KernelFilter *m_filter;
      ImagePtr      m_image;
};

}

#endif // ENVIRONMENT_MAP_H_

