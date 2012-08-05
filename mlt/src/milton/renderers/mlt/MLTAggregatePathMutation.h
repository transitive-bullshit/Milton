/**<!-------------------------------------------------------------------->
   @class  MLTAggregatePathMutation
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Aggregation of one or more sub-MLTPathMutations under one coherent 
   interface. Used to choose between bidirectional, lens subpath, and 
   perturbation mutations during Metropolis transitions in MLTMarkovProcess.
   <!-------------------------------------------------------------------->**/

#ifndef MLT_AGGREGATE_PATH_MUTATION_H_
#define MLT_AGGREGATE_PATH_MUTATION_H_

#include <renderers/mlt/MLTPathMutation.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<MLTPathMutation*>, MLTPathMutationList);

class MILTON_DLL_EXPORT MLTAggregatePathMutation : public MLTPathMutation {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTAggregatePathMutation(MLTRenderer *renderer = NULL)
         : MLTPathMutation(renderer)
      { }
      
      virtual ~MLTAggregatePathMutation();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Initializes all sub mutations and optional parameters from the 
       * renderer's PropertyMap
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t mutate(const Path &X, Path &Y);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      MLTPathMutationList m_mutations;
      std::vector<real_t> m_weights;
};

}

#endif // MLT_AGGREGATE_PATH_MUTATION_H_

