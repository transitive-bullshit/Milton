/**<!-------------------------------------------------------------------->
   @class  Scene
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008

   @brief
      Contains all primitive and material data representing a given 3D scene
   <!-------------------------------------------------------------------->**/

#ifndef SCENE_H_
#define SCENE_H_

#include <common/common.h>
#include <materials/Material.h>
#include <stats/samplers/EmitterSampler.h>

// disable warning: 'this' : used in base member initializer list
#ifdef MILTON_ARCH_WINDOWS
#  pragma warning(disable : 4355)
#endif

namespace milton {

struct Ray;
struct SurfacePoint;
class  ShapeSet;

class MILTON_DLL_EXPORT Scene {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline explicit Scene(ShapeSet *shapeSet = NULL)
          : m_shapes(shapeSet), m_lights(NULL), m_background(NULL), 
            m_emitterSampler(this), m_initted(false)
      { }
      
      inline explicit Scene(const MaterialList &materials, 
                            ShapeSet *shapeSet = NULL)
          : m_shapes(shapeSet), m_lights(NULL), m_background(NULL), 
            m_materials(materials), m_emitterSampler(this), m_initted(false)
      { }
      
      virtual ~Scene();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization routines
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Tests the given ray with this Shape for intersection. If a valid 
       * intersection exists, its "t" value will be returned and the given 
       * SurfacePoint will be initialized with enough data for this Shape to 
       * later fill in all relevant information lazily (world space normal 
       * at intersection point, uv coordinates, etc.)
       * 
       * @note if you only care about whether or not an intersection exists 
       *    and don't actually need to know the exact intersection point, 
       *    see the intersects method instead.
       * @returns the smallest positive "t" value of any intersections 
       *    found, or INFINITY if no valid intersection exists
       */
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      /**
       * @brief
       *    Used for occlusion/visibility testing where you don't necessarily 
       * care about anything other than whether or not an intersection exists.
       * (generally faster than getIntersection)
       * 
       * @note default implementation defers to getIntersection
       * @returns whether or not the given ray intersects this Shape with a 
       *    positive "t" value greater than EPSILON and less than the given 
       *    @p tMax
       */
      virtual bool intersects(const Ray &ray, real_t tMax = INFINITY);
      
      /**
       * Displays a crude OpenGL preview of this scene
       */
      virtual void preview();
      
      /**
       * @returns the environment or background radiance emitted from beyond 
       *    the extents of the scene in direction -w
       * 
       * @note you can think of background emission as a large spherical 
       *    emitter surrounding the entire scene, emitting inwards
       */
      virtual SpectralSampleSet getBackgroundRadiance(const Vector3 &w);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline ShapeSet *getShapes() {
         return m_shapes;
      }
      
      inline ShapeSet *getLights() {
         return m_lights;
      }
      
      inline EmitterSampler getEmitterSampler() const {
         return m_emitterSampler;
      }
      
      inline Material *getDefaultMaterial() {
         ASSERT(m_materials.size() > 0);
         
         return m_materials.back();
      }
      
      inline MaterialList &getMaterials() {
         return m_materials;
      }
      
      inline Emitter *getBackground() {
         return m_background;
      }
      
      inline void setBackground(Emitter *emitter) {
         m_background = emitter;
      }
      
      inline bool isInitialized() const {
         return m_initted;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      ShapeSet      *m_shapes;
      ShapeSet      *m_lights;
      Emitter       *m_background;
      
      MaterialList   m_materials;
      EmitterSampler m_emitterSampler;
      bool           m_initted;
};

}

#endif // SCENE_H_

