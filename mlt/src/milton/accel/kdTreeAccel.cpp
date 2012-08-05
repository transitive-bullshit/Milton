/**<!-------------------------------------------------------------------->
   @file   kdTreeAccel.cpp
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

#include "kdTreeAccel.h"

#include <SurfacePoint.h>
#include <ResourceManager.h>
#include <Random.h>
#include <QtCore/QtCore>
#include <Log.h>

#include <GL/gl.h>
#include <boost/static_assert.hpp>
#include <algorithm>
#include <climits>
using namespace std;

namespace milton {

// notes on debugging kd-Trees:
//    If issues arise, the first thing you want to do is determine whether the 
// problem is in tree construction or tree traversal. To narrow this down, I'd
// recommend replacing the traversal with a really simple / naive DFS that will 
// traverse the entire data structure without any pruning at all, recording the 
// minimum t-value along the way. If you're still experiencing problems, then 
// the problem is likely with the construction of your tree. Needless to say, 
// you should also test with no acceleration data structure at all (linear 
// traversal through all primitives in the scene ala NaiveSpatialAccel), to 
// determine whether the problem lies in a primitive intersection routine of 
// in the acceleration data structure itself.
//    Note the presence of kdNode::preview which recursively draws a version 
// of the kd-Tree (starting with the root kdNode) in OpenGL. This method has 
// been invaluable in visualizing and debugging edge cases with planar 
// primitives and subtle, yet complex scenes.


// -------------------------------------------------------------------------
// Internal data structures
// -------------------------------------------------------------------------

#define KD_FLAG_BITS                (3)
#define KD_LEAF_FLAG                (3)

#define KD_FLAG(node)               ((node)->flag  &  KD_FLAG_BITS)

#define KD_LEAF_NODE(node)          (KD_FLAG(node) == KD_LEAF_FLAG)
#define KD_INTERNAL_NODE(node)      (KD_FLAG(node) != KD_LEAF_FLAG)

#define KD_SPLIT_AXIS(node)         (KD_FLAG(node))
#define KD_SPLIT_POS(node)          ((node)->splitPos)
#define KD_NO_PRIMITIVES(node)      ((node)->noPrimitives >> 2)

// left/right children stored next to each other on 16 byte boundary
// right child is stored at left child + 8 bytes
#define KD_LEFT_CHILD(node)         \
   ((kdNode*)((node)->flag & ~KD_FLAG_BITS))
#define KD_RIGHT_CHILD(node)        \
   (KD_LEFT_CHILD(node) + 1)
#define KD_CHILD(node, n)           \
   (((kdNode*)((node)->flag & ~KD_FLAG_BITS)) + n)

#define KD_INIT_INTERNAL(node, leftChild, splitAxis, pos)         \
   do {                                                           \
      (node)->child    = (leftChild);                             \
      (node)->flag    |= (splitAxis);                             \
      (node)->splitPos = (pos);                                   \
      ASSERT(KD_INTERNAL_NODE((node)));                           \
      workBuf->log.noInternal++;                                  \
   } while(0)

#define KD_INIT_LEAF(node, prims, noPrims)                        \
   do {                                                           \
      (node)->noPrimitives = ((noPrims) << 2);                    \
      (node)->flag        |= (KD_LEAF_FLAG);                      \
      (node)->primitives   = (prims);                             \
      ASSERT(KD_LEAF_NODE((node)));                               \
      workBuf->log.noLeaves++;                                    \
      workBuf->log.minPrimitives  =                               \
         MIN(workBuf->log.minPrimitives, (unsigned) (noPrims));   \
      workBuf->log.maxPrimitives  =                               \
         MAX(workBuf->log.maxPrimitives, (unsigned) (noPrims));   \
      workBuf->log.avgPrimitives += (unsigned) (noPrims);         \
      workBuf->log.minDepth  =                                    \
         MIN(workBuf->log.minDepth, workBuf->depth);              \
      workBuf->log.maxDepth  =                                    \
         MAX(workBuf->log.maxDepth, workBuf->depth);              \
      workBuf->log.avgDepth += workBuf->depth;                    \
   } while(0)

/// 8 byte packed struct optimized for cache performance, 
/// (represents a single node in the kd-Tree)
struct kdNode {
   union {  // 4 bytes
      kdNode *child;
      unsigned int noPrimitives;
      unsigned int flag;
   };
   union {  // 4 bytes
      IndexedIntersectableList *primitives;
      float splitPos;
   };
   
   inline void cleanup() {
      if (KD_INTERNAL_NODE(this)) {
         kdNode *leftChild  = KD_LEFT_CHILD(this);
         kdNode *rightChild = KD_RIGHT_CHILD(this);
         
         ASSERT(leftChild && rightChild);
         rightChild->cleanup();
         leftChild->cleanup();
         
         // no need to free right child as both children share the same malloc
         // i.e. they're twins -- born at the same time & die at the same time
         // ... creepy :)
         free(leftChild);
      } else { // leaf node
         if (primitives)
            delete primitives;
      }
   }
   
   void preview(const AABB &aabb, kdTreeAccel *accel, unsigned depth = 0) const {
      bool displayNormal = true;
      
#if DEBUG
      // handy view which allows the visualizer to step through and display 
      // each node encountered during a given traversal of the kd-Tree
      if ((displayNormal = ResourceManager::contains("kdTreeIndex"))) {
         const std::vector<kdNode*> &intersected = 
            accel->getIntersectedDebugList();
         
         const unsigned index = 
            ResourceManager::getValue<unsigned>("kdTreeIndex");
         
         const bool draw = 
            (index < intersected.size() && this == intersected[index]);
         
         if (draw) {
            if (KD_INTERNAL_NODE(this))
               glColor4f(1, 0, 0, 1);
            else 
               glColor4f(0, 1, 0, 1);
            
            aabb.preview();
         }
      }
#endif
      
      if (KD_INTERNAL_NODE(this)) {
         kdNode *leftChild  = KD_LEFT_CHILD(this);
         kdNode *rightChild = KD_RIGHT_CHILD(this);
         
         ASSERT(leftChild && rightChild);
         const unsigned splitAxis = KD_SPLIT_AXIS(this);
         const float splitPlane   = KD_SPLIT_POS(this);
         
         ASSERT(aabb.min[splitAxis] < splitPlane);
         ASSERT(aabb.max[splitAxis] > splitPlane);
         
         if (displayNormal) {
#if 0
            glColor4f(1, 1, 1, 1);
            aabb.preview();
#else
            AABB split(aabb);
            split.min[splitAxis] = splitPlane;
            split.max[splitAxis] = splitPlane;
            
            // draw opaque bounding box of current split plane, colored by 
            // RGB = XYZ according to the current split axis
            const unsigned axis = splitAxis;//depth % 3;
            glColor4f((axis == 0), (axis == 1), (axis == 2), 1);
            split.preview();
            
            // draw transparent quad representing split plane
            glColor4f(1, 1, 1, 0.5f);
            
            glBegin(GL_QUADS);
            
            glNormal3f((splitAxis == 0), (splitAxis == 1), (splitAxis == 2));
            
            if (splitAxis == 0) {
               glVertex3real_t(split.min[0], split.min[1], split.min[2]);
               glVertex3real_t(split.min[0], split.min[1], split.max[2]);
               
               glVertex3real_t(split.min[0], split.max[1], split.max[2]);
               glVertex3real_t(split.min[0], split.max[1], split.min[2]);
            } else if (splitAxis == 1) {
               glVertex3real_t(split.min[0], split.min[1], split.min[2]);
               glVertex3real_t(split.min[0], split.min[1], split.max[2]);
               
               glVertex3real_t(split.max[0], split.min[1], split.max[2]);
               glVertex3real_t(split.max[0], split.min[1], split.min[2]);
            } else {
               ASSERT(splitAxis == 2);
               
               glVertex3real_t(split.min[0], split.min[1], split.min[2]);
               glVertex3real_t(split.min[0], split.max[1], split.min[2]);
               
               glVertex3real_t(split.max[0], split.max[1], split.min[2]);
               glVertex3real_t(split.max[0], split.min[1], split.min[2]);
            }
            
            glEnd();
#endif
         }
         
         { // recur on left child
            AABB left(aabb);
            left.max[splitAxis] = splitPlane;
            leftChild->preview(left, accel, depth + 1);
         }
         
         { // recur on right child
            AABB right(aabb);
            right.min[splitAxis] = splitPlane;
            rightChild->preview(right, accel, depth + 1);
         }
      } else {
/*#ifdef DEBUG
         IntersectableList *prims = accel->getIntersectables();
         
         // ensure kd-Tree integrity
         for(unsigned i = prims->size(); i--;) {
            Intersectable *isect = (*prims)[i];
            ASSERT(isect);
            
            if (isect->getAABB().intersects(aabb)) {
               bool found = false;
               
               for(unsigned i = KD_NO_PRIMITIVES(this); i--;) {
                  const Intersectable *const curIntersectable = (*prims)[(*primitives)[i]];
                  
                  if ((found = (isect == curIntersectable)))
                     break;
               }
               
               ASSERT(found);
            }
         }
#endif*/
      }
   }
};

//BOOST_STATIC_ASSERT(sizeof(kdNode) == 8);

struct kdSplitPlane {
   enum SPLIT_TYPE { SPLIT_MAX = 0, SPLIT_PLANE = 1, SPLIT_MIN = 2 };
   
   real_t     splitPos;
   SPLIT_TYPE splitType;
   
   inline bool operator< (const kdSplitPlane &p) const {
      if (splitPos < p.splitPos) return true;
      if (splitPos > p.splitPos) return false;
      return (splitType < p.splitType);
      /*return 
         ((splitPos < p.splitPos) || 
          (splitPos == p.splitPos && splitType < p.splitType));*/
   }
   
   inline bool operator==(const kdSplitPlane &p) const {
      return (splitType == p.splitType && splitPos == p.splitPos);
   }
};

struct kdSAHCost {
   real_t   totalExpCost;
   real_t   splitPos;
   unsigned splitAxis;
};

/// fixed-size stack used during traversal
struct kdStack {
   struct kdStackNode {
      kdNode *node;
      real_t  tMax;
   };
   
   kdStackNode  nodes[KD_MAX_DEPTH];
   kdStackNode *top;
   
   inline kdStack()
      : top(nodes)
   { }
   
   inline bool isEmpty() const {
      return (top == nodes);
   }
   
   inline void push(kdNode *node, real_t tMax) {
      top->node = node;
      top->tMax = tMax;
      ++top;
   }
   
   inline void pop(kdNode *&node, real_t &tMax) {
      ASSERT(!isEmpty());
      
      --top;
      node = top->node;
      tMax = top->tMax;
   }
};

struct kdTreeAccelLog {
   // internal node statistics
   unsigned noInternal;
   unsigned minDepth;
   unsigned maxDepth;
   unsigned avgDepth;
   
   // leaf node statistics
   unsigned noLeaves;
   unsigned minPrimitives;
   unsigned maxPrimitives;
   real_t   avgPrimitives;
   
   inline kdTreeAccelLog()
      : noInternal(0), minDepth(std::numeric_limits<unsigned>::max()), 
        maxDepth(0), avgDepth(0), 
        noLeaves(0), minPrimitives(std::numeric_limits<unsigned>::max()), 
        maxPrimitives(0), avgPrimitives(0)
   { }
};

/// buffer used internally / extensively during tree construction
struct kdWorkBuffer : public Log {
   kdNode                   *node;
   
   kdTreeAccelLog            log;
   
   // indices of active primitives at current level
   IndexedIntersectableList *primitives;
   unsigned                  noPrimitives;
   
   unsigned                  splitAxis;
   real_t                    splitPos;
   
   kdSplitPlane             *splitPlanes;
   
   unsigned                  depth;
   AABB                      aabb;
   
   // SAH-specific
   bool                      forceLeaf; // if SAH implies leaf
   
   inline kdWorkBuffer() 
      : node(NULL), log(), primitives(NULL), noPrimitives(0), 
        splitAxis(3), splitPos(0), splitPlanes(NULL), 
        depth(0), forceLeaf(false)
   {
      init();
   }
   
   virtual ~kdWorkBuffer() {
      safeDeleteArray(splitPlanes);
   }
};


// -------------------------------------------------------------------------
// Main public interface
// -------------------------------------------------------------------------


kdTreeAccel::kdTreeAccel() 
   : SpatialAccel(), m_buildParams(), m_root(NULL)
{
   _reset();
}

kdTreeAccel::~kdTreeAccel() {
   _reset();
}

void kdTreeAccel::init() {
   kdWorkBuffer workBuf;
   
   if (m_primitives->size() > 1)
      workBuf << "initializing kdTree: " << m_primitives->size() << " primitives" << endl;
   
   // free any prior data structures
   _reset();
   
   // initialize global AABB
   SpatialAccel::init();
   
   // initialize build parameters from PropertyMap
   _initProperties(workBuf);
   
   // initialize split position member function
   SplitFunction splitPositionFunctions[] = {
      &kdTreeAccel::_computeSplitPlaneMiddle, 
      &kdTreeAccel::_computeSplitPlaneMedian, 
      &kdTreeAccel::_computeSplitPlaneSAH, 
   };
   
   ASSERT(m_buildParams.kdSplitPlaneType < 3);
   m_splitPlaneFunction = splitPositionFunctions[m_buildParams.kdSplitPlaneType];
   
   // initialize split axis member function
   SplitFunction splitAxisFunctions[] = {
      &kdTreeAccel::_computeSplitAxisRoundRobin, 
      &kdTreeAccel::_computeSplitAxisLongestExtent, 
   };
   
   ASSERT(m_buildParams.kdSplitAxisType  < 2);
   m_splitAxisFunction = splitAxisFunctions[m_buildParams.kdSplitAxisType];
   
   // create the root node
   m_root = (kdNode*) malloc(sizeof(kdNode));
   
   // initialize working buffer
   workBuf.splitPlanes  = new kdSplitPlane[m_primitives->size() * 2];
   workBuf.aabb         = m_aabb;
   workBuf.splitAxis    = 3;
   
   IndexedIntersectableList *primitives = new IndexedIntersectableList(m_primitives->size());
   for(unsigned i = primitives->size(); i--;)
      (*primitives)[i] = i;
   
   // build the tree!
   _buildTreeHelper(&workBuf, m_root, primitives, 0, 0, 0);
   
   // optional post-processing (memory pooling)
   if (m_buildParams.kdPostCompress)
      _postCompressTree();
   
   const kdTreeAccelLog &log = workBuf.log;
   ASSERT(log.noLeaves > log.noInternal);
   
   // print out useful debugging info / stats
   if (m_primitives->size() > 1) {
      workBuf << "   " << "minDepth:      " << log.minDepth << endl;
      workBuf << "   " << "maxDepth:      " << log.maxDepth << endl;
      workBuf << "   " << "avgDepth:      " << 
         (log.avgDepth / log.noLeaves) << endl;
      workBuf << "   " << "noInternal:    " << log.noInternal << endl;
      workBuf << "   " << "noLeaves:      " << log.noLeaves << endl;
      workBuf << "   " << "minPrimitives: " << log.minPrimitives << endl;
      workBuf << "   " << "maxPrimitives: " << log.maxPrimitives << endl;
      workBuf << "   " << "avgPrimitives: " << 
         (log.avgPrimitives / log.noLeaves) << endl;
   }
}


// -------------------------------------------------------------------------
// Internal construction methods
// -------------------------------------------------------------------------


void kdTreeAccel::_reset() {
   if (m_root) {
      m_root->cleanup();
      free(m_root);
      m_root = NULL;
   }
}

void kdTreeAccel::_initProperties(kdWorkBuffer &workBuf) {
   const char *const splitPlaneTypes[] = {
      "splitPlaneMiddle", "splitPlaneMedian", "splitPlaneSAH", NULL
   };
   
   const char *const splitAxisTypes[] = {
      "splitAxisRoundRobin", "splitAxisLongestExtent", NULL
   };
   
   const std::string &param  = getValue<const std::string>(
      "kdSplitPlaneType", splitPlaneTypes[m_buildParams.kdSplitPlaneType]);
   
   if (m_primitives->size() > 1)
      workBuf << "   kdSplitPlaneType: " << param << endl;
   
   for(int i = 3; i--;) {
      if (splitPlaneTypes[i] == param) {
         m_buildParams.kdSplitPlaneType = (SplitPlaneType) i;
         break;
      }
   }
   
   const std::string &param2 = getValue<const std::string>(
      "kdSplitAxisType", splitAxisTypes[m_buildParams.kdSplitAxisType]);
   
   for(int i = 2; i--;) {
      if (splitAxisTypes[i] == param2) {
         m_buildParams.kdSplitAxisType = (SplitAxisType) i;
         break;
      }
   }
   
#define GET_PARAM(name, type) \
   m_buildParams.name = getValue<type>(#name, m_buildParams.name);
   
   GET_PARAM(kdMinPrimitives, unsigned);
   GET_PARAM(kdMaxDepth,      unsigned);
   GET_PARAM(kdNoThreads,     unsigned);
   GET_PARAM(kdPostCompress,  bool);
   GET_PARAM(kdCostTraversal, real_t);
   GET_PARAM(kdEmptyBias,     real_t);
   
#undef GET_PARAM
}

void kdTreeAccel::_buildTree(kdWorkBuffer *workBuf) {
   kdNode *node = workBuf->node;
   
   if (workBuf->depth > workBuf->log.maxDepth)
      workBuf->log.maxDepth = workBuf->depth;
   
   // Check base cases
   // -----------------------------------------------------
   if (workBuf->noPrimitives < m_buildParams.kdMinPrimitives || 
       workBuf->depth >= m_buildParams.kdMaxDepth)
   {
      KD_INIT_LEAF(node, workBuf->primitives, workBuf->noPrimitives);
      return;
   }
   
   // Select split axis and plane (position along axis)
   // -----------------------------------------------------
   ((this)->*(m_splitPlaneFunction))(workBuf);
   
   if (workBuf->forceLeaf)
      return;
   
   const unsigned splitAxis  = workBuf->splitAxis;
   const real_t   splitPos   = workBuf->splitPos;
   
   const real_t nearSplitPos = splitPos;// - EPSILON;
   const real_t farSplitPos  = splitPos;// + EPSILON;
   
   IndexedIntersectableList *leftPrims  = new IndexedIntersectableList();
   IndexedIntersectableList *rightPrims = new IndexedIntersectableList();
   
   // Partition primitives according to split
   // -----------------------------------------------------
   FOREACH(IndexedIntersectableListIter, *workBuf->primitives, iter) {
      const unsigned index = *iter;
      const Intersectable *const shape = (*m_primitives)[index];
      
      if (shape->getAABB().isPoint())
         continue;
      
      const real_t min = shape->getMin(splitAxis);
      const real_t max = shape->getMax(splitAxis);
      ASSERT(min <= max);
      
      if (max >  nearSplitPos)
         rightPrims->push_back(index);
      
      if (min <= farSplitPos)
         leftPrims->push_back(index);
   }
   
   // Stop recursion if splitting doesn't help
   // -----------------------------------------------------
   if ((workBuf->noPrimitives == leftPrims->size() || 
        workBuf->noPrimitives == rightPrims->size()) && 
       (m_splitPlaneFunction != &kdTreeAccel::_computeSplitPlaneSAH || 
        (workBuf->noPrimitives == leftPrims->size() && 
         workBuf->noPrimitives == rightPrims->size())))
   {
      safeDelete(leftPrims);
      safeDelete(rightPrims);
      
      KD_INIT_LEAF(node, workBuf->primitives, workBuf->noPrimitives);
      return;
   }
   
   safeDelete(workBuf->primitives);
   
   // Prepare children and finalize current node
   // -----------------------------------------------------
   kdNode *leftChild  = (kdNode*) malloc(sizeof(kdNode) * 2);
   kdNode *rightChild = leftChild + 1;
   
   KD_INIT_INTERNAL(node, leftChild, splitAxis, static_cast<float>(splitPos));
   const unsigned int depth = workBuf->depth + 1;
   
   // Recurse on left child
   // -----------------------------------------------------
   
   const real_t prevMax = workBuf->aabb.max[splitAxis];
   workBuf->aabb.max[splitAxis] = splitPos;
   ASSERT(prevMax >= splitPos);
   
   _buildTreeHelper(workBuf, leftChild, leftPrims, splitAxis, 
                    splitPos, depth);
   workBuf->aabb.max[splitAxis] = prevMax;
   
   // Recurse on right child
   // -----------------------------------------------------
   
   const real_t prevMin = workBuf->aabb.min[splitAxis];
   workBuf->aabb.min[splitAxis] = splitPos;
   ASSERT(prevMin <= splitPos);
   
   _buildTreeHelper(workBuf, rightChild, rightPrims, splitAxis, 
                    splitPos, depth);
   workBuf->aabb.min[splitAxis] = prevMin;
}

void kdTreeAccel::_buildTreeHelper(kdWorkBuffer *workBuf, kdNode *child, 
                                   IndexedIntersectableList *primitives, 
                                   const unsigned splitAxis, 
                                   const real_t splitPos, 
                                   const unsigned depth)
{
   // Recursively subdivide node
   // -----------------------------------------------------
   unsigned noPrimitives = primitives->size();
   
   if (noPrimitives > 0) {
      workBuf->node         = child;
      workBuf->primitives   = primitives;
      workBuf->noPrimitives = noPrimitives;
      
      workBuf->splitAxis    = splitAxis;
      workBuf->splitPos     = splitPos;
      
      workBuf->depth        = depth;
      workBuf->forceLeaf    = false;
      
      // TODO: multithreading spawn thread if depth < threshold
      // NOTE: new workBuf as well cause of concurrent access to 
      // workBuf's members and how to aggregate debugging info
      // (kdTreeAccelLog)
      
      _buildTree(workBuf);
   } else { // node is empty
      KD_INIT_LEAF(child, primitives, 0);
   }
}


// split axis functions
// --------------------

void kdTreeAccel::_computeSplitAxisRoundRobin(kdWorkBuffer *workBuf) const {
   unsigned maxTries = 2;
   
   do {
      workBuf->splitAxis = (workBuf->splitAxis + 1) % 3;
   } while(workBuf->aabb.min[workBuf->splitAxis] >= 
           workBuf->aabb.max[workBuf->splitAxis] && --maxTries);
   
   ASSERT(workBuf->splitAxis >= 0 && workBuf->splitAxis <= 2);
}

void kdTreeAccel::_computeSplitAxisLongestExtent(kdWorkBuffer *workBuf) const {
   const unsigned maxExtent = workBuf->aabb.getMaxExtent();
   
   workBuf->splitAxis = maxExtent;
   
   // TODO: test speed compared to alternative (below)
   //if (oldSplitAxis != maxExtent)
   //   workBuf->splitAxis = maxExtent;
   //else _computeSplitAxisRoundRobin(workBuf);
}


// split position functions
// ------------------------

void kdTreeAccel::_computeSplitPlaneMiddle(kdWorkBuffer *workBuf) const {
   ((this)->*(m_splitAxisFunction))(workBuf);
   const unsigned splitAxis = workBuf->splitAxis;
   
   // choosing a split plane in the middle of a principally-aligned axis 
   // produces a kd-Tree that is functionally equivalent to an Octree
   
   workBuf->splitPos = (workBuf->aabb.min[splitAxis] + 
                        workBuf->aabb.max[splitAxis]) / 2;
}

void kdTreeAccel::_computeSplitPlaneMedian(kdWorkBuffer *workBuf) const {
   ((this)->*(m_splitAxisFunction))(workBuf);
   
   // Sort candidate split planes along selected split axis
   // -------------------------------------------------------------
   kdSplitPlane *planes = workBuf->splitPlanes;
   unsigned int n = _computeSplitPlanesSAH(*workBuf->primitives, 
                                           workBuf->noPrimitives, 
                                           workBuf->splitAxis, 
                                           planes);
   
   // select median split plane
   if (n > 0)
      workBuf->splitPos = planes[n >> 1].splitPos;
   else workBuf->forceLeaf = true;
}

void kdTreeAccel::_computeSplitPlaneSAH(kdWorkBuffer *workBuf) const {
   const unsigned noPrimitives = workBuf->noPrimitives;
   kdSplitPlane *planes = workBuf->splitPlanes;
   kdSAHCost minSAHCost = { INFINITY, 0, 0 };
   
   // foreach split axis
   // ----------------------------------------------------------------
   for(unsigned splitAxis = 0; splitAxis < 3; ++splitAxis) {
      
      // note: the first constraint here produces a slightly less optimal tree
      // but trees which have long chains of splits along the same axis are 
      // generally really deep and empirically, this constraint can drastically 
      // reduce the number of overall nodes in the tree at the cost of slightly 
      // more primitives per leaf on average
      if (splitAxis == workBuf->splitAxis || 
          workBuf->aabb.min[splitAxis] >= workBuf->aabb.max[splitAxis])
      {
         continue;
      }
      
      // Sort candidate split planes along current split axis
      // -------------------------------------------------------------
      unsigned int n = _computeSplitPlanesSAH(*workBuf->primitives, 
                                              noPrimitives, 
                                              splitAxis, 
                                              planes);
      
      // Evaluate minimum SAH cost among sorted candidate split planes
      // -------------------------------------------------------------
      
      kdSAHCost curSAHCost;
      unsigned index   = 0;
      unsigned noLeft  = 0;
      unsigned noRight = noPrimitives;
      
      // foreach candidate split plane
      while(index < n - 1) {
         const real_t curSplitPlane = planes[index].splitPos;
         unsigned noPrims[3] = { 0, 0, 0 };
         
         // Skip past all equivalent split planes
         do {
            ++noPrims[planes[index].splitType];
         } while(index < n - 1 && planes[++index].splitPos == curSplitPlane);
         
         // Update running primitive counters
         const unsigned int noPlane = noPrims[kdSplitPlane::SPLIT_PLANE];
         noRight -= noPrims[kdSplitPlane::SPLIT_MAX] + noPlane;
         
         // Compute SAH
         if (curSplitPlane > workBuf->aabb.min[splitAxis] && 
             curSplitPlane < workBuf->aabb.max[splitAxis])
         {
            _computeSplitCostSAH(workBuf, curSplitPlane, splitAxis, noLeft, 
                                 noPlane, noRight, curSAHCost);
            
            if (curSAHCost.totalExpCost < minSAHCost.totalExpCost)
               minSAHCost = curSAHCost;
         }
         
         noLeft  += noPrims[kdSplitPlane::SPLIT_MIN] + noPlane;
      }
   }
   
   if (minSAHCost.totalExpCost == INFINITY) {
      KD_INIT_LEAF(workBuf->node, workBuf->primitives, noPrimitives);
      workBuf->forceLeaf = true;
      
      return;
   }
   
   const real_t leafCost = 
      (noPrimitives - m_buildParams.kdCostTraversal) * 
      workBuf->aabb.getSurfaceArea();
   
   // if estimated cost of subdivision is greater than the guaranteed cost of 
   // intersecting this node as-is, then create a leaf instead of splitting
   if (minSAHCost.totalExpCost > leafCost) {
      KD_INIT_LEAF(workBuf->node, workBuf->primitives, noPrimitives);
      workBuf->forceLeaf = true;
      
      return;
   }
   
   ASSERT(minSAHCost.totalExpCost < INFINITY);
   
   // record description of minimal-cost split
   workBuf->splitPos  = minSAHCost.splitPos;
   workBuf->splitAxis = minSAHCost.splitAxis;
}

unsigned kdTreeAccel::_computeSplitPlanesSAH(
   const IndexedIntersectableList &primitives, 
   const unsigned int noPrimitives, 
   const unsigned int splitAxis, 
   kdSplitPlane *outPlanes) const
{
   unsigned n = 0;
   
   // Aggregate candidate split planes (primitive extrema along split axis)
   // ---------------------------------------------------------------------
   for(unsigned int i = noPrimitives; i--;) {
      const Intersectable *const prim = (*m_primitives)[primitives[i]];
      
      if (prim->getAABB().isPoint())
         continue;
      
      const real_t min  = prim->getMin(splitAxis);
      const real_t max  = prim->getMax(splitAxis);
      
      if (min == max) {
         outPlanes[n].splitPos  = min;
         outPlanes[n].splitType = kdSplitPlane::SPLIT_PLANE;
         ++n;
      } else {
         outPlanes[n].splitPos  = min;
         outPlanes[n].splitType = kdSplitPlane::SPLIT_MIN;
         ++n;

         outPlanes[n].splitPos  = max;
         outPlanes[n].splitType = kdSplitPlane::SPLIT_MAX;
         ++n;
      }
   }
   
   // Sort candidate split planes along current split axis
   // ---------------------------------------------------------------------
   if (n > 0)
      std::sort(outPlanes, outPlanes + n);
   
   return n;
}

void kdTreeAccel::_computeSplitCostSAH(const kdWorkBuffer *workBuf, 
                                       real_t splitPos, 
                                       const unsigned splitAxis, 
                                       unsigned noLeft, 
                                       unsigned noPlane, 
                                       unsigned noRight, 
                                       kdSAHCost &outCost) const
{
   real_t leftSA, rightSA;
   
   workBuf->aabb.getMinMaxSurfaceArea(splitAxis, splitPos, 
                                      leftSA, rightSA);
   
   noLeft += noPlane;
   outCost.totalExpCost = (leftSA * noLeft + rightSA * noRight);
   
   // favor splits which produce empty cells
   if (noLeft + noPlane == 0 || noRight == 0)
      outCost.totalExpCost *= m_buildParams.kdEmptyBias;
   
   outCost.splitAxis    = splitAxis;
   outCost.splitPos     = splitPos;
}


void kdTreeAccel::_postCompressTree() {
   
   /// @TODO: compress memory
   
   /// @TODO: record stats
}


// -------------------------------------------------------------------------
// Public intersection tests
// -------------------------------------------------------------------------


real_t kdTreeAccel::getIntersection(const Ray &ray, SurfacePoint &pt) {
   real_t tMin, tMax;
   
   //m_intersected.clear();
   
   // check for intersection between ray and root node's bounding box
   if (!m_aabb.intersects(ray, tMin, tMax))
      return INFINITY;
   
   ASSERT(tMin <= tMax);
   
   /**
    * technical note: tMin and tMax are always conservatively offset by EPSILON
    * to reduce 99% of numerical robustness issues, the majority of which 
    * are due to inherent floating point arithmetic inprecision, compounded 
    * by the fact that kdNodes store their split plane positions using 
    * 32-bit floating point precision, whereas all other math in Milton uses 
    * 64-bit double floating point precision (default value of real_t)
    */
   tMin -= EPSILON;
   tMax += EPSILON;
   
   register kdNode *curNode = m_root;
   kdNode *farNode;
   kdStack stack;
   
   /**
    * Each Ray has a unique "ID" stored with it to help prevent multiple 
    * intersection tests against the same Shape during traversal of an 
    * acceleration data structure (e.g., kd-Tree --  since a Shape may 
    * appear in more than one leaf node)
    */
   // TODO: thread-safe rayIDs
   
   while(1) {
      // traverse until we reach a leaf
      while(KD_INTERNAL_NODE(curNode)) {
         const unsigned splitAxis = KD_SPLIT_AXIS(curNode);
         const real_t diff = (KD_SPLIT_POS(curNode) - ray.origin[splitAxis]);
         const real_t d    = diff * ray.invDir[splitAxis];
         
         // for visualizing the order in which nodes are traversed
         // note: this can't always be active because of multithreading!
         //m_intersected.push_back(curNode);
         
         // order near/far children relative to ray's origin and direction
         register const bool reverseOrder = 
            (diff <= 0 ? 
             (ray.direction[splitAxis] <= 0) : 
             (ray.direction[splitAxis] <  0));
            //((ray.direction[splitAxis] < 0) || 
            // (ray.direction[splitAxis] == 0 && diff <= 0));
         
         curNode = KD_CHILD(curNode, reverseOrder);
         
         // if far node not intersected or ray is perpendicular to split plane
         if (d > tMax)
            continue;     // cull far node
         
         farNode = curNode + 1 - 2 * reverseOrder;
         
         if (d < tMin) {  // cull near node
            curNode = farNode;
         } else {         // ray intersects both near and far children
            // traverse near node and place far node on stack
            stack.push(farNode, tMax); // { farNode, d, tMax }
            tMax = d + EPSILON;
         }
      }
      
      //m_intersected.push_back(curNode);
      
      // check for and record intersections 
      const IndexedIntersectableList &primitives = *curNode->primitives;
      real_t tMinCell = INFINITY;
      
      unsigned normalCase = pt.normalCase;
      unsigned index      = pt.index;
      Shape   *shape      = pt.shape;
      
      for(unsigned i = KD_NO_PRIMITIVES(curNode); i--;) {
         const unsigned primIndex = primitives[i];
         Intersectable *curIntersectable = (*m_primitives)[primIndex];
         
         // Only test intersection with any given shape once per ray 
         // (using a mailbox "ID")
         //if (curIntersectable->rayID != rayID) {
            pt.shape  = NULL;
            pt.index  = (unsigned)(-1);
            
            const real_t t = curIntersectable->getIntersection(ray, pt);
            
            /*if (t == INFINITY) {
               curIntersectable->rayID = rayID;
            } else */
            if (t > EPSILON && t < tMinCell) {
               tMinCell = t;
               
               shape      = (pt.shape ? pt.shape : static_cast<Shape*>(curIntersectable));
               index      = (pt.index == ((unsigned)(-1)) ? primIndex : pt.index);
               normalCase = pt.normalCase;
            }
         //}
      }
      
      // Early termination!
      if (tMinCell <= tMax) {
         pt.shape      = shape;
         pt.normalCase = normalCase;
         pt.index      = index;
         
         return tMinCell;
      }
      
      if (stack.isEmpty())
         return INFINITY;
      
      // no valid intersection found, so traverse back up tree and 
      // check an alternate path
      
      tMin = tMax - (2 * EPSILON);
      stack.pop(curNode, tMax);
   }
}

bool kdTreeAccel::intersects(const Ray &ray, real_t clipMax) {
   real_t tMin, tMax;
   
   // check for intersection between ray and root node's bounding box
   if (!m_aabb.intersects(ray, tMin, tMax) || clipMax < tMin)
      return false;
   
   if (clipMax < tMax)
      tMax = clipMax;
   
   ASSERT(tMin <= tMax);
   tMin -= EPSILON;
   tMax += EPSILON;
   
   //const unsigned rayID = QThread::currentThreadId() ^ Random::sampleInt(0, 99999);
   register kdNode *curNode = m_root;
   kdNode *farNode;
   kdStack stack;
   
   while(1) {
      // traverse until we reach a leaf
      while(KD_INTERNAL_NODE(curNode)) {
         const unsigned splitAxis = KD_SPLIT_AXIS(curNode);
         const real_t diff = (KD_SPLIT_POS(curNode) - ray.origin[splitAxis]);
         const real_t d    = diff * ray.invDir[splitAxis];
         
         // order near/far children relative to ray's origin and direction
         register const bool reverseOrder = 
            (diff <= 0 ? 
             (ray.direction[splitAxis] <= 0) : 
             (ray.direction[splitAxis] <  0));
         
         curNode = KD_CHILD(curNode, reverseOrder);
         
         // if far node not intersected or ray is perpendicular to split plane
         if (d > tMax)
            continue;     // cull far node
         
         farNode = curNode + 1 - 2 * reverseOrder;
         
         if (d < tMin) { // cull near node
            curNode = farNode;
         } else {         // ray intersects both near and far children
            // traverse near node and place far node on stack
            stack.push(farNode, tMax); // { farNode, d, tMax }
            tMax = d + EPSILON;
         }
      }
      
      // check for and record intersections 
      const IndexedIntersectableList &primitives = *curNode->primitives;
      
      for(unsigned i = KD_NO_PRIMITIVES(curNode); i--;) {
         const unsigned primIndex = primitives[i];
         Intersectable *curIntersectable = (*m_primitives)[primIndex];
         
         // Only test intersection with any given shape once per ray 
         // (using a mailbox "ID")
         //if (curIntersectable->rayID != rayID) {
            // Early termination!
            if (curIntersectable->intersects(ray, clipMax))
               return true;
            
         //   curIntersectable->rayID = rayID;
         //}
      }
      
      if (stack.isEmpty())
         return false;
      
      // no valid intersection found, so traverse back up tree and 
      // check an alternate path
      
      tMin = tMax;
      stack.pop(curNode, tMax);
   }
}

void kdTreeAccel::preview() {
   // save state
   glPushAttrib(GL_ENABLE_BIT);
   
   { // enable blending / transparency
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   }
   
   { // disable back-face culling and turn on back-facing shading
      glDisable(GL_CULL_FACE);
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
   }
   
   // recursively draw each node in the kd-Tree
   if (m_root && m_primitives->size() > 1)
      m_root->preview(m_aabb, this);
   
   // restore state
   glPopAttrib(); // GL_BLEND
                  // GL_CULL_FACE
   
   // draw AABB last, overlaid atop any other transparent fragments
   glColor3f(1, 1, 1);
   m_aabb.preview();
}

}

