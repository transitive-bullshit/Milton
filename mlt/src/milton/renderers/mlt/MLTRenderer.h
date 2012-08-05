/**<!-------------------------------------------------------------------->
   @class  MLTRenderer
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Spring 2009

   @brief
      Metropolis Light Transport (MLT) renderer based on Veach & Guibas, 1997.
   MLT is currently the most efficient light transport algorithm known for
   robustly rendering arbitrarily complex scenes that many other rendering 
   algorithms would consider difficult for various reasons (eg. strong 
   indirect illumination, small cracks / holes, etc). At its core, MLT is an 
   application of the general-purpose Metropolis-Hastings sampler, applied in 
   the context of trying to sample all possible paths of light starting at an 
   emitter and ending at a virtual camera sensor in a carefully tuned way such 
   that the probability density of simulating any one path is proportional to 
   that path's relative contribution to what that virtual sensor would end up 
   "seeing" in the virtual scene.
   
   @see MLTMarkovProcess
   @see MLTPathMutation
   <!-------------------------------------------------------------------->**/

#ifndef MLT_RENDERER_H_
#define MLT_RENDERER_H_

#include <renderers/PointSampleRenderer.h>
#include <renderers/utils/Path.h>

namespace milton {

DECLARE_STL_TYPEDEF(std::vector<Path>,    PathList);
DECLARE_STL_TYPEDEF(std::vector<real_t >, RealList);

class BidirectionalPathTracer;

class MILTON_DLL_EXPORT MLTRenderer : public PointSampleRenderer {
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      inline MLTRenderer(RenderOutput *output = NULL, 
                         Camera *camera = NULL, 
                         Scene *scene = NULL)
         : PointSampleRenderer(output, camera, scene), m_pathGenerator(NULL)
      { }
      
      virtual ~MLTRenderer();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization
      //@{-----------------------------------------------------------------
      
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      /**
       * @brief
       *    Renders the underlying scene using Metropolis Light Transport (MLT)
       */
      virtual void render();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors / Mutators
      //@{-----------------------------------------------------------------
      
      virtual bool isMLT() const {
         return true;
      }
      
      inline unsigned getMaxDepth() const {
         return m_maxDepth;
      }
      
      inline unsigned getMaxConsequtiveRejections() const {
         return m_maxConsequtiveRejections;
      }
      
      
      //@}-----------------------------------------------------------------
      
   protected:
      virtual real_t _initSeedPaths(PathList &outPaths, RealList &outW, 
                                    const unsigned noInitialPaths);
      
   protected:
      BidirectionalPathTracer *m_pathGenerator;
      
      // max path length
      unsigned m_maxDepth;
      
      // maximum number of consequtive rejections
      unsigned m_maxConsequtiveRejections;
};

}

#endif // MLT_RENDERER_H_

