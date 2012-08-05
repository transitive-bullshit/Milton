/**<!-------------------------------------------------------------------->
   @class  Material
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract representation of a surface Material, defined without respect 
   to the underlying surface (loose coupling between Shapes and Materials 
   from the point-of-view of a Material, but all Shapes know about their 
   surface Material).  Materials subclass PropertyMap, and it is through this 
   interface that Material properties may be set (ex: diffuse color, 
   texture/bump/color map(s), index of refraction of interior volume, etc.)
   
      Reflectivity, emittance, and sensor response (BSDFs, Emitters, and 
   Sensors respectively) are three properties of a material that are defined 
   at a single point on a surface.  A Material encapsulates BSDF, Emitter, 
   and Sensor properties defined over its surface, where specific instances of 
   BSDF, Emitter, and Sensor are allowed to have their inputs vary with 
   respect to position along the surface.  In this respect, Materials 
   represent a mapping from surface position to associated BSDF, Emitter, and 
   Sensor functions, where the underlying functions themselves remain constant 
   along the surface, and only the inputs vary among the different instances / 
   surface points.  For example, a Material may have a DiffuseBSDF over its 
   entire surface, but a specific DiffuseBSDF instance obtained by getBSDF or 
   implicitly in initSurfacePoint (which fills in the SurfacePoint's BSDF), is 
   allowed to have its 'kd' parameter (diffuse albedo) vary with respect to 
   the given surface point via lookup in an associated 'kd' texture map 
   defined over the UV coordinates of the surface.
   <!-------------------------------------------------------------------->**/

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <core/SurfacePoint.h>
#include <utils/PropertyMap.h>
#include <shapes/Shape.h>

#include <materials/BSDF.h>
#include <materials/Emitter.h>
#include <materials/Sensor.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<Material*>, MaterialList);

class KernelFilter;

class MILTON_DLL_EXPORT Material : public PropertyMap {
   public:
      ///@name Static members
      //@{-----------------------------------------------------------------
      static SurfacePoint s_nullSurfacePoint;
      static Emitter    *s_nullEmitter;
      static Sensor     *s_nullSensor;
      
   public:
      //@}-----------------------------------------------------------------
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline   Material()
         : m_filter(NULL), m_bsdf("diffuse"), m_emitter("null"), m_bumpMap(""),
           m_repeatU(1), m_repeatV(1), m_bumpIntensity(5)
      { }
      
      virtual ~Material();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization that may be necessary before beginning 
       * to sample this material
       * 
       * @note default implementation initializes this material's filter, used 
       *    for filtering texture values
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @returns whether or not this Material is an emitter
       */
      virtual bool isEmitter();
      
      /**
       * @brief
       *    Accessor for instantiating BSDFs used in sampling at various 
       * SurfacePoints along an underlying surface comprised of this Material
       * 
       * @returns a new BSDF defined at the given surface point. The type of 
       *    BSDF returned depends on the 'bsdf' property in the PropertyMap, 
       *    which defaults to DiffuseBSDF
       */
      virtual BSDF *getBSDF(SurfacePoint &pt);
      
      /**
       * @brief
       *    Accessor for instantiating Emitters used for evaluating emittance 
       * along an underlying surface comprised of this Material
       * 
       * @returns a new Emitter defined at the given surface point. The type of 
       *    Emitter returned depends on the 'emitter' property in the 
       *    PropertyMap, which defaults to NullEmitter (no light emitted)
       */
      virtual Emitter *getEmitter(SurfacePoint &pt);
      
      /**
       * @brief
       *    Accessor for instantiating a representative Emitter at any point 
       * on the surface of this Material
       * 
       * @note useful if you just want to know the emittance properties of 
       *    this Material (power, radiant exitance, etc.) and don't actually 
       *    intend to use it for sampling purposes
       * 
       * @returns a new Emitter defined at a dummy surface point. The type of 
       *    Emitter returned depends on the 'emitter' property in the 
       *    PropertyMap, which defaults to NullEmitter (no light emitted)
       */
      virtual Emitter *getEmitter();
      
      /**
       * @brief
       *    Accessor for instantiating Sensors used for evaluating importance 
       * along an underlying surface comprised of this Material
       * 
       * @returns a new Sensor defined at the given surface point
       */
      virtual Sensor *getSensor(SurfacePoint &pt);
      
      /**
       * @brief
       *    Sets up OpenGL material state (color properties, lights, etc.) to 
       * enable a crude/fast preview of geometry to which this Material is 
       * applied
       */
      virtual void preview(Shape *shape);
      
      
      //@}------------------------------------------------------------------
      ///@name Utility methods for extracting BSDF/Emitter/Sensor parameters
      //@{------------------------------------------------------------------
      
      SpectralSampleSet getSpectralSampleSet(const std::string &key, 
                                             const SpectralSampleSet &defaultValue, 
                                             const SurfacePoint &pt);
      
      SpectralSampleSet getSpectralSampleSet(const std::string &key, 
                                             const real_t &defaultValue, 
                                             const SurfacePoint &pt);
      
      SpectralSampleSet getSpectralSampleSet(const std::string &key, 
                                             const std::string &defaultValue, 
                                             const SurfacePoint &pt);
      
      SpectralSampleSet getSpectralSampleSet(const std::string &key, 
                                             const SurfacePoint &pt);
      
      RgbaHDR getSample(const ImagePtr &image, const UV &uv);
      
      inline KernelFilter *getFilter() {
         return m_filter;
      }
      
      void setFilter(KernelFilter *filter);
      
      
      //@}-----------------------------------------------------------------
      ///@name Lazy evaluation of SurfacePoint information
      //@{-----------------------------------------------------------------
      
      virtual void initSurfacePoint(SurfacePoint &pt);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _initShadingNormal(SurfacePoint &pt);
      
   protected:
      KernelFilter     *m_filter;
      
      std::string       m_bsdf;
      std::string       m_emitter;
      std::string       m_bumpMap;
      
      real_t            m_repeatU;
      real_t            m_repeatV;
      real_t            m_bumpIntensity;
};

}

#endif // MATERIAL_H_

