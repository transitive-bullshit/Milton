{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 8, 
         "noDirectSamples" : 1, 
         /*"ambient" : [ .5, .5, .5 ], */
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type" : "pinhole", 
         /* optional parameters */
         "eye"   : [ 2.5, 2.1, 2.5 ], 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material": {
            "emitter" : {
               "type" : "omni", 
               "power" : 100, 
            }, 
            
            "transform" : {
               "translate" : [ 2.5, 2.1, 2.5 ], 
               "point" : { },
            }, 
         }, 
         
         "blob" : {
            "metaObjects" : [
               { "type" : "ball", "position" : [ .6, 0, 0 ], }, 
               { "type" : "ball", "position" : [ 0, .6, 0 ], }, 
               { "type" : "ball", "position" : [ 0, 0, .6 ], }, 
            ], 
            
            "spatialAccel" : "kdTree", 
            "kdMaxDepth" : 16, 
            /* threshold may range from [0, 0.2) */
            "threshold" : .18, 
         }, 
      }, 
   },
}
