/**<!-------------------------------------------------------------------->
   @class  MLTPerturbationPathMutation
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      MLTPerturbationPathMutation implements lens and caustic perturbations.
   The core idea shared by these mutations is that by slightly moving vertices 
   along a path, you can explore nearby paths that will make similar 
   contributions to the image, thereby increasing the acceptance probability.  
   Perturbations are designed to complement the strengths and weaknesses of 
   bidirectional mutations in that they make relatively small, low-cost 
   changes to paths that have a larger acceptance probability as opposed to 
   bidirectional mutations which are in charge of making large scale changes 
   (making successive paths less correlated) at the expense of generally lower 
   acceptance probabilities.
      This class implements two types of perturbation mutations:
   Lens perturbations concentrate on small changes to the film plane location 
   by modifying the lens edge of the form (L|D)DS*E.  Caustic perturbations 
   also change the lens edge but are designed to concentrate solely on primary 
   caustic paths of the form LS+DE.  Multi-chain perturbations focus on 
   paths with a suffix of the form (D|L)DS+DS+E and are currently not 
   implemented.
   
   @see p. 350-354 of Veach's thesis (section 11.4.3)
   <!-------------------------------------------------------------------->**/

#ifndef MLT_PERTURBATION_PATH_MUTATION_H_
#define MLT_PERTURBATION_PATH_MUTATION_H_

#include <renderers/mlt/MLTPathMutation.h>

namespace milton {

class MILTON_DLL_EXPORT MLTPerturbationPathMutation : public MLTPathMutation {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTPerturbationPathMutation(MLTRenderer *renderer = NULL)
         : MLTPathMutation(renderer)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t mutate(const Path &X, Path &Y);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual void _perturbVector(Vector3 &N) const;
      
   protected:
      real_t m_r1, m_r2;
      real_t m_t1, m_t2;
};

}

#endif // MLT_PERTURBATION_PATH_MUTATION_H_

