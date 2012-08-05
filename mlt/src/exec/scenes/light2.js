{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         /*"type" : "pathTracer", */
         "type" : "bidirectionalPathTracer", 
         "noRenderThreads" : 8, 
         "noDirectSamples" : 1, 
         
         "noSuperSamples"  : 16, 
         "generator" : "super", 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "pinhole", 
         "eye"   : [ 0, 1.5, 0 ], 
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
            
            "transform" : {
               "translate" : [ 0, 1.6, 0 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ 1, 1, 1 ], 
               "plane"  : { },
            }, 
         }, 
         
         "material" : {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ 1, 0, 0 ], 
            }, 
            "plane"  : { },
         }, 
      },
   },
}

