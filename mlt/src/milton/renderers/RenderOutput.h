/**<!-------------------------------------------------------------------->
   @class  RenderOutput
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      Records point samples from a renderer which may be used to construct 
   an output image on a local or distributed machine
   <!-------------------------------------------------------------------->**/

#ifndef RENDER_OUTPUT_H_
#define RENDER_OUTPUT_H_

#include <common/image/RgbaImage.h>
#include <utils/PropertyMap.h>
#include <filters/filters.h>
#include <core/Viewport.h>

class  QMutex;

namespace milton {

struct PointSample;
class  ToneMap;
class  Renderer;

class MILTON_DLL_EXPORT RenderOutput : public PropertyMap {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline RenderOutput(const Viewport &d)
         : PropertyMap(), m_viewport(d), m_isMLT(false), 
           m_output(NULL), m_progressiveValues(NULL), m_tonemap(NULL), 
           m_proposed(NULL), m_noSamples(NULL), m_locks(NULL), 
           m_parent(NULL), m_seconds(0)
      { }
      
      inline RenderOutput(Image *output = NULL)
         : PropertyMap(), m_viewport(480, 480), m_isMLT(false), 
           m_output(output), m_progressiveValues(NULL), m_tonemap(NULL), 
           m_proposed(NULL), m_noSamples(0), m_locks(NULL), 
           m_parent(NULL), m_seconds(0)
      {
         if (output) {
            const unsigned width  = output->getWidth();
            const unsigned height = output->getHeight();
            
            m_viewport.setSize(width, height);
         }
      }
      
      virtual ~RenderOutput();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Performs any initialization which may be necessary before adding 
       * samples with 'addSample'
       * 
       * @note
       *    Default implementation initializes the viewport with the properties 
       * this RenderOutput has been assigned
       */
      virtual void init();
      
      /**
       * @brief
       *    Performs any post-processing on output after all samples have 
       * been collected (called after rendering is complete)
       * 
       * @note
       *    Default implementation is blank
       */
      virtual void finalize();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Adds the given point sample, which is assumed to have a valid 
       * value, to this output
       * 
       * @note default implementation adds the sample to an internal image 
       *    using a box filter
       */
      virtual void addSample(PointSample &sample);
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline const Viewport &getViewport() const {
         return m_viewport;
      }
      
      inline void setViewport(const Viewport &d) {
         m_viewport = d;
      }
      
      inline ToneMap *getToneMap() {
         return m_tonemap;
      }
      
      inline Image *getImage() {
         return m_output;
      }
      
      virtual void setImage(Image *image);
      
      inline void setParent(Renderer *renderer) {
         m_parent = renderer;
      }
      
      virtual RgbaImage *getFinalizedOutput();
      
      void addPropposed(const PointSample &);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _addSample(const unsigned row, const unsigned col, 
                              PointSample &value, const real_t weight, 
                              bool lock = false);
      
      void _lockPixel  (unsigned row, unsigned col);
      void _unlockPixel(unsigned row, unsigned col);
      
   protected:
      /// Provides mutual exclusion to sample storage data structure(s)
      Viewport m_viewport;
      bool     m_isMLT, m_filterProposed;
      
      Image   *m_output;
      ProgressiveFilterValue<SpectralSampleSet> *m_progressiveValues;
      ToneMap *m_tonemap;
      
      unsigned long *m_proposed;
      unsigned long *m_noSamples;
      QMutex        *m_locks;
      
      Renderer      *m_parent;
      unsigned long  m_seconds;
};

}

#endif // RENDER_OUTPUT_H_

