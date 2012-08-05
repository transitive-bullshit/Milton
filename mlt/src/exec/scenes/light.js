{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         /*"type" : "bidirectionalPathTracer", */
         "type" : "mlt", 
         /*"type" : "pathTracer", */
         "noRenderThreads" : 1, 
         "noDirectSamples" : 1, 
         
         "noSuperSamples"  : 16, 
         "generator" : "super", 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "pinhole", 
         "eye"   : [ 0, 1, 0 ], 
         "focus" : [ 0, 0, 0 ], 
         "up"    : [ 1, 0, 0 ], 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material" : { /* light */
            "emitter" : {
               "type" : "oriented", 
               "power" : 10
            }, 
            
            "plane"  : { },
         }, 
      },
   },
}

