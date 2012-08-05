/**<!-------------------------------------------------------------------->
   @class  MLTPathMutation
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Abstract superclass for all path mutations used by MLTRenderer, whose 
   main task is to sample mutated versions of a path and return the 
   probability density of accepting the mutation.
   
   @see MLTAggregatePathMutation
   @see MLTBidirPathMutation
   @see MLTLensSubpathMutation
   @see MLTPerturbationPathMutation
   <!-------------------------------------------------------------------->**/

#ifndef MLT_PATH_MUTATION_H_
#define MLT_PATH_MUTATION_H_

#include <renderers/mlt/MLTRenderer.h>
#include <renderers/utils/Path.h>

namespace milton {

class MILTON_DLL_EXPORT MLTPathMutation {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTPathMutation(MLTRenderer *renderer = NULL)
         : m_renderer(renderer)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes all optional parameters from the renderer's PropertyMap
       * 
       * @note default implementation is empty
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Calculates a mutated path @Y, conditioned on the given path @X, 
       * returning the acceptance probability density a(Y|X) of accepting 
       * the given mutation according to the Metropolis-Hastings algorithm.
       * 
       * @note
       *    a(Y|X) = MIN(1, (f(y) * T(X|Y)) / (T(Y|X) * f(x)))
       * 
       * @returns a(Y|X)
       */
      virtual real_t mutate(const Path &X, Path &Y) = 0;
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      inline MLTRenderer *getRenderer() const {
         return m_renderer;
      }
      
      inline void setRenderer(MLTRenderer *renderer) {
         m_renderer = renderer;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      MLTRenderer *m_renderer;
};

}

#endif // MLT_PATH_MUTATION_H_

