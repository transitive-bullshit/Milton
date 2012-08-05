/**<!-------------------------------------------------------------------->
   @class  Renderer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Abstract rendering engine which attempts to evaluate a rendering 
   equation over a given 2D domain (the film plane) and pipes its output 
   to an equally abstract RenderOutput (generally an image wrapper, but 
   allowing for distributed rendering)
   <!-------------------------------------------------------------------->**/

#ifndef RENDERER_H_
#define RENDERER_H_

#include <utils/PropertyMap.h>
#include <utils/SpectralSampleSet.h>
#include <utils/Timer.h>

namespace milton {

class DirectIllumination;
class Camera;
class Scene;

class MILTON_DLL_EXPORT Renderer : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline Renderer(Camera *camera = NULL, Scene *scene = NULL)
         : PropertyMap(), m_camera(camera), m_scene(scene), m_initted(false), 
           m_directIllumination(NULL)
      { }
      
      virtual ~Renderer();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization routines
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before 
       * beginning to render the underlying scene
       * 
       * @note
       *    Default implementation ensures we have a valid scene and camera
       * and calls init on both of them respectively
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * Renders the underlying scene
       */
      virtual void render() = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline Camera *getCamera() {
         return m_camera;
      }
      
      inline void setCamera(Camera *camera) {
         m_camera = camera;
      }
      
      inline Scene *getScene() {
         return m_scene;
      }
      
      inline void setScene(Scene *scene) {
         m_scene = scene;
      }
      
      virtual bool supportsOpenGL() {
         return false;
      }
      
      virtual bool isMLT() const {
         return false;
      }
      
      inline DirectIllumination *getDirectIllumination() {
         return m_directIllumination;
      }
      
      inline const Timer &getTimer() const {
         return m_timer;
      }
      
      virtual std::string getElapsedTime() const;
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      Camera *m_camera;
      Scene  *m_scene;
      bool    m_initted;
      
      DirectIllumination *m_directIllumination;
      Timer               m_timer;
      //boost::timer        m_timer;
};

}

#endif // RENDERER_H_

