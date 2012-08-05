/**<!-------------------------------------------------------------------->
   @class  MLTMarkovProcess
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Models a random walk throughout the space of all light paths in a scene 
   as a Markov process (where state transitions are "memoryless" in that they 
   only depend on the previous state), with state transitions governed by 
   the Metropolis-Hastings algorithm (states are specific paths of light). 
   
   @see MLTPathMutation
   <!-------------------------------------------------------------------->**/

#ifndef MLT_MARKOV_PROCESS_H_
#define MLT_MARKOV_PROCESS_H_

#include <renderers/utils/Path.h>
#include <QtCore/QThread>

namespace milton {

class  MLTRenderer;
class  MLTPathMutation;
struct PointSample;

class MILTON_DLL_EXPORT MLTMarkovProcess : public QThread {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTMarkovProcess(MLTRenderer *renderer, const Path &path, 
                              const real_t weight, bool primary = false)
         : QThread(), m_renderer(renderer), m_mutation(NULL), m_path(path), 
           m_weight(weight), m_primary(primary)
      { }
      
      virtual ~MLTMarkovProcess();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Begins a random walk throughout path space, starting from an 
       * arbitrary initial state, with transitions determined by the 
       * Metropolis-Hastings algorithm
       */
      virtual void run();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline MLTRenderer *getRenderer() const {
         return m_renderer;
      }
      
      inline MLTPathMutation *getMutation() const {
         return m_mutation;
      }
      
      inline const Path &getPath() const {
         return m_path;
      }
      
      inline void setPath(const Path &path) {
         m_path = path;
      }
      
      inline real_t getWeight() const {
         return m_weight;
      }
      
      inline void setWeight(const real_t weight) {
         m_weight = weight;
      }
      
      inline bool isPrimary() const {
         return m_primary;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _initSample(Path &path, PointSample &sample);
      virtual void _addSample (const PointSample &sample, real_t prob, 
                               bool tentative = false);
      
   protected:
      MLTRenderer     *m_renderer;
      MLTPathMutation *m_mutation;
      Path             m_path;
      real_t           m_weight;
      unsigned         m_maxDepth;
      unsigned         m_maxConsequtiveRejections;
      bool             m_primary;
};

}

#endif // MLT_MARKOV_PROCESS_H_

