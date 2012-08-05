/**<!-------------------------------------------------------------------->
   @class  kdTreeAccel
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2008
   
   @brief
      An N-dimensional kd-Tree is an axis-aligned binary spatial partitioning 
   data structure.  Each internal node is split along one of the N principle 
   axes, where the split plane is typically chosen at each node based on some 
   sort of heuristic.
      This kd-Tree implementation supports several common 
   split plane generation methods, namely:  spatial midpoint, median of 
   geometry, and surface area heuristic (SAH).  Note that a kd-Tree 
   constructed by choosing the spatial midpoint along the current split axis 
   as the next split plane is equivalent to an Octree.  Also note that though 
   the SAH produces the best quality tree overall with respect to visibility 
   tests, it can take a long time to build and is thus mainly suitable for 
   static scenes (whereas other acceleration data structures may be more 
   appropriate for dynamic geometry & animations).
      The SAH kd-Tree may be tweaked with several parameters.  See 
   kdTreeAccelParams for more info.  The O(nlog n) SAH construction has 
   been implemented as opposed to the O(n) or O(n^2) alternatives (using 
   std::sort for internal sorting).  This choice was made because the O(n) 
   solutions which exist both have large constants (which diminish their 
   utility in practice), and are nowhere near as readable or easy to 
   understand as the relatively straightforward O(nlog n) alternative.
   <!-------------------------------------------------------------------->**/

#ifndef KD_TREE_ACCEL_H_
#define KD_TREE_ACCEL_H_

#include "accel/SpatialAccel.h"

namespace milton {

#define KD_MAX_DEPTH       (24)

struct kdNode;
struct kdWorkBuffer;
struct kdSplitPlane;
struct kdSAHCost;

DECLARE_STL_TYPEDEF(std::vector<unsigned>, IndexedIntersectableList);

class MILTON_DLL_EXPORT kdTreeAccel : public SpatialAccel {
   
   public:
      /**
       * @brief Supported split plane generation techniques
       */
      enum SplitPlaneType {
         SPLIT_PLANE_MIDDLE = 0, 
         SPLIT_PLANE_MEDIAN, 
         SPLIT_PLANE_SAH     // default
      };
      
      /**
       * @brief Supported split axis selection techniques
       */
      enum SplitAxisType {
         SPLIT_AXIS_ROUND_ROBIN = 0, // default
         SPLIT_AXIS_LONGEST_EXTENT
      };
      
      /**
       * @brief Parameters controlling construction of a kdTreeAccel
       */
      struct BuildParams {
         /// split plane generation method (defaults to SPLIT_PLANE_SAH)
         SplitPlaneType kdSplitPlaneType;
         
         /// split axis selection method (defaults to SPLIT_AXIS_ROUND_ROBIN)
         SplitAxisType kdSplitAxisType;
         
         /// minimum number of primitives s.t. a leaf cell will automatically
         /// be created (below this number, further subdivision either won't 
         /// help or requires too much memory)
         unsigned kdMinPrimitives;
         
         /// maximum depth of tree
         unsigned kdMaxDepth;
         
         /// number of threads to use during construction
         unsigned kdNoThreads;
         
         /// if true, after construction, will attempt to reorganize memory 
         /// layout efficiently in memory
         bool kdPostCompress;
         
         /// SAH-specific parameters
         real_t kdCostTraversal;
         real_t kdEmptyBias;
         
         /**
          * Constructs default kd-Tree parameters
          */
         BuildParams(SplitPlaneType splitType = SPLIT_PLANE_SAH, 
                     SplitAxisType  splitAxis = SPLIT_AXIS_ROUND_ROBIN, 
                     unsigned minPrimitives   = 3, 
                     unsigned maxDepth        = KD_MAX_DEPTH, 
                     unsigned noThreads       = 1, 
                     bool postCompress        = true, 
                     real_t costTraversal     = 1, 
                     real_t emptyBias         = 0.9)
            : kdSplitPlaneType(splitType), 
              kdSplitAxisType(splitAxis), 
              kdMinPrimitives(minPrimitives), 
              kdMaxDepth(maxDepth), 
              kdNoThreads(noThreads), 
              kdPostCompress(postCompress), 
              kdCostTraversal(costTraversal), 
              kdEmptyBias(emptyBias)
         {
            ASSERT(kdMaxDepth <= KD_MAX_DEPTH);
         }
      };
      
   public:
      ///@name Constructors
      //@{-----------------------------------------------------------------
      
      explicit kdTreeAccel();
      virtual ~kdTreeAccel();
      
      
      //@}-----------------------------------------------------------------
      ///@name Initialization Routines
      //@{-----------------------------------------------------------------
      
      inline void setBuildParams(const BuildParams &buildParams) {
         m_buildParams = buildParams;
      }
      
      /**
       * Constructs a kd-Tree with the current BuildParams
       * @overridden
       */
      virtual void init();
      
      
      //@}-----------------------------------------------------------------
      ///@name Main usage interface
      //@{-----------------------------------------------------------------
      
      virtual real_t getIntersection(const Ray &ray, SurfacePoint &pt);
      
      virtual bool intersects(const Ray &ray, 
                              real_t tMax = INFINITY);
      
      virtual void preview();
      
      
      //@}-----------------------------------------------------------------
      ///@name Accessors
      //@{-----------------------------------------------------------------
      
      inline const BuildParams &getBuildParams() const {
         return m_buildParams;
      }
      
#if DEBUG
      inline const std::vector<kdNode*> &getIntersectedDebugList() const {
         return m_intersected;
      }
#endif
      
      //@}-----------------------------------------------------------------
      
   protected:
      void        _buildTree(kdWorkBuffer *);
      void        _buildTreeHelper(kdWorkBuffer *, kdNode *node, 
                                   IndexedIntersectableList *primitives, 
                                   const unsigned splitAxis, 
                                   const real_t   splitPos, 
                                   const unsigned depth);
      unsigned    _computeSplitAxis(kdWorkBuffer *) const;
      
      void        _postCompressTree();
      
      typedef void (kdTreeAccel::*SplitFunction) (kdWorkBuffer *) const;
      
      SplitFunction m_splitAxisFunction;
      SplitFunction m_splitPlaneFunction;
      
      // split axis functions
      void _computeSplitAxisRoundRobin(kdWorkBuffer *) const;
      void _computeSplitAxisLongestExtent(kdWorkBuffer *) const;
      
      // split position functions
      void _computeSplitPlaneMiddle(kdWorkBuffer *) const;
      void _computeSplitPlaneMedian(kdWorkBuffer *) const;
      void _computeSplitPlaneSAH   (kdWorkBuffer *) const;
      
      /// @returns number of candidate planes (planes stored in 'outPlanes')
      inline unsigned _computeSplitPlanesSAH(
         const IndexedIntersectableList &primitives, 
         const unsigned int noPrimitives, 
         const unsigned int splitAxis, 
         kdSplitPlane *outPlanes) const;
      
      /// @returns Surface Area Heuristic (SAH) cost for given subdivision
      ///    in out variable 'outCost'
      inline void     _computeSplitCostSAH(const kdWorkBuffer *, 
                                           real_t splitPos, 
                                           const unsigned splitAxis, 
                                           unsigned noLeft, 
                                           unsigned noPlane, 
                                           unsigned noRight, 
                                           kdSAHCost &outCost) const;
      
   protected:
      BuildParams    m_buildParams;
      
      // TODO: internal data structures
      kdNode        *m_root; 
      
   private:
      void _initProperties(kdWorkBuffer &);
      
      void _reset();
      
#if DEBUG
   private:
      // for visualizing / debugging the order in which nodes are traversed
      std::vector<kdNode*> m_intersected;
#endif
};

}

#endif // KD_TREE_ACCEL_H_
