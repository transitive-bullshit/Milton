{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         "ambient" : [ .7, .7, .7 ], 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type" : "pinhole", 
         /* optional parameters */
         "eye"   : [ 1, 1, 1 ], 
         "up"    : [ 0, 1, 0 ], 
         "heightAngle" : 45, 
         "aspectRatio" : 1.0, 
         "near"  : 1, 
         "far"   : 100, 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "spatialAccel" : {
            "type" : "kdTree", 
            "kdSplitPlaneType" : "splitPlaneSAH", 
            /*"kdMaxDepth" : 3, */
         },
         
         "material" : { /* light */
            "emitter" : {
               "type" : "oriented", 
               "power" : 80000000, 
            }, 
            "transform" : {
               "translate" : [ 0, 1, 0 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "plane"  : { },
            },
         }, 
         
         "mesh" : {
            "path" : "data/meshes/lion.obj", 
            "kdSplitPlaneType" : "splitPlaneSAH", 
            /*"kdMaxDepth" : 3, */
         },
      }, 
   },
}

