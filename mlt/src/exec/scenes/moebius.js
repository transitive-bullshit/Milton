{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      /*"renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         "ambient" : [ .7, .7, .7 ], 
      }, */
      
      /**
       * Camera 
       */
      "camera" : {
         "type" : "pinhole", 
         /* optional parameters */
         "eye"   : [ 7, 7, 7 ], 
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
         "mesh" : {
            "path" : "data/meshes/moebiusStrip.obj", 
         },
      }, 
   },
}

