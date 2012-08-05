/**<!-------------------------------------------------------------------->
   @class  MLTBidirPathMutation
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009
   
   @brief
      Bidirectional mutations are the foundation of the MLT algorithm and are 
   responsible for handling large mutations, such as modifying a path's length.
   This is achieved by deleting a (possibly empty) subpath of the current path 
   and replacing it by a (possibly empty) different subpath.
   
   @see p. 345-350 of Veach's thesis (section 11.4.2)
   <!-------------------------------------------------------------------->**/

#ifndef MLT_BIDIR_PATH_MUTATION_H_
#define MLT_BIDIR_PATH_MUTATION_H_

#include <renderers/mlt/MLTPathMutation.h>

namespace milton {

class MILTON_DLL_EXPORT MLTBidirPathMutation : public MLTPathMutation {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTBidirPathMutation(MLTRenderer *renderer = NULL)
         : MLTPathMutation(renderer)
      { }
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t mutate(const Path &X, Path &Y);
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual real_t _getMutationR(const Path &pathX, 
                                   const Path &pathY, 
                                   const unsigned kd, 
                                   const unsigned ka, 
                                   const real_t pd, 
                                   const real_t pa1, 
                                   const int l, 
                                   const int m);
      
      virtual real_t _getPd1(const Path &X, 
                             const unsigned kd) const;
      virtual real_t _getPd2(const Path &X, 
                             const unsigned kd, const signed l) const;
      
      virtual real_t _getPa1(const Path &X, 
                             const unsigned kd, const unsigned ka) const;
      virtual real_t _getPa2(const Path &X, 
                             const unsigned kd, const unsigned ka, 
                             const unsigned lP) const;
};

}

#endif // MLT_BIDIR_PATH_MUTATION_H_

