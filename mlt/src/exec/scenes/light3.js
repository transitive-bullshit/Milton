{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "bidirectionalPathTracer", 
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
         "eye"   : [ 278, 273, -650 ], 
         "look"  : [ 0, 0, 1 ], 
         "up"    : [ 0, 1, 0 ], 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material" : { /* light */
            "emitter" : {
               "type" : "oriented", 
               /* light source should have approximately the following 
                * emittance proportions: B = 1, G = 2, R = 2.25 */
               "power" : [ 2000000, 2000000, 1600000 ], /* slightly yellow */
            }, 
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : 1, 
            }, 
            "transform" : {
               "translate" : [ 278, 547, 279.5 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ 140, 1, 125 ], 
               "plane"  : { },
            },
         }, 
      },
   },
}

