"spatialAccel" : {
   "_brief" : "Acceleration data structure for accelerating ray-object intersection queries and visibility tests that are prominent in many rendering algorithms", 
   "_desc"  : "Available spatialAccel variants include:  %naive (no acceleration) and kdTree (default)", 
   "_class" : "SpatialAccel", 
   "_note"  : "'naive' spatialAccel corresponds to linear traversal through all primitives when calculating the closest positive intersection between a ray and a set of primitives", 
   "_info"  : { "type" : "string", "optional" : true, "default" : "kdTree" }
}, 
"kdSplitPlaneType" : {
   "_brief" : "Split %plane generation method", 
   "_desc"  : "Available variants include:  splitPlaneMiddle, splitPlaneMedian, and splitPlaneSAH.", 
   "_note"  : "A kd-Tree constructed with splitPlaneMiddle and splitAxisRoundRobin is equivalent to a standard octree, but for ray tracing purposes, the surface area heuristic (splitPlaneSAH, the default) is generally preferred for its ability to concentrate efforts in dense regions of the scene.", 
   "_info"  : { "type" : "string", "optional" : true, "default" : "splitPlaneSAH" }
}, 
"kdSplitAxisType" : {
   "_brief" : "Split axis selection method", 
   "_desc"  : "Available variants include:  splitAxisRoundRobin and splitAxisLongestExtent", 
   "_note"  : "If splitPlaneSAH is selected, kdSplitAxisType will be ignored, and the split axis at each node will be selected according to the minimum expected SAH cost (with a few optimization-oriented exceptions..)", 
   "_info"  : { "type" : "string", "optional" : true, "default" : "none" }
}, 
"kdMinPrimitives" : {
   "_brief" : "Minimum number of primitives s.t. a leaf cell will automatically be created (below this number, further subdivision either won't help or requires too much memory", 
   "_info"  : { "type" : "uint", "optional" : true, "default" : "3" }
}, 
"kdMaxDepth" : {
   "_brief" : "Maximum depth of tree", 
   "_info"  : { "type" : "uint", "optional" : true, "default" : "24" }
}, 
"kdNoThreads" : {
   "_brief" : "number of threads to spawn during tree construction", 
   "_note"  : "TODO; currently single-threaded construction", 
   "_info"  : { "type" : "uint", "optional" : true, "default" : "1" }
}, 
"kdCostTraversal" : {
   "_brief" : "SAH relative cost of traversal", 
   "_info"  : { "type" : "real", "optional" : true, "default" : "1" }
}, 
"kdEmptyBias" : {
   "_brief" : "SAH empty cell bias factor", 
   "_info"  : { "type" : "real", "optional" : true, "default" : "0.9" }
}, 
