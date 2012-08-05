"blob" : {
   // declare a blobby metaobject composed of three metaballs
   "metaObjects" : [
      { "type" : "ball", "position" : [ .6, 0, 0 ], }, 
      { "type" : "ball", "position" : [ 0, .6, 0 ], }, 
      { "type" : "ball", "position" : [ 0, 0, .6 ], }, 
   ], 
   
   // optionally specify hints for resulting mesh's acceleration data structure
   "spatialAccel" : "kdTree", 
   "kdMaxDepth" : 16, 
   
   // threshold may range from [0, 0.2)
   "threshold" : .18, 
},

