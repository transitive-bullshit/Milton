{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayTracer", 
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
         "eye"   : [ 3, .8, 0 ], 
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
         "background" : {
            "emitter" : {
               "type" : "environment", 
               "coords" : "spherical", 
               "path" : "data/env/grace_probe.hdr", 
            }, 
         }, 
         
         /*"background" : {
            "emitter" : {
               "type" : "blackbody", 
               "temp" : 5000, 
            }, 
         }, */
         
         "material" : {
            "emitter" : {
               "type" : "omni", 
               "power" : [ 15, 15, 15 ], 
            }, 
            
            "transform" : {
               "translate" : [ 1, 1, 0 ], 
               "point"  : { },
            },
         }, 
         
         "material": {
            "bsdf" : {
               "type" : "specular", 
            }, 
            
            "transform" : {
               "translate" : [ 0, .25, 1 ], 
               "scale"  : [ .5, .5, .5 ], 
               "sphere"  : { },
            },
         }, 
         
         "material": {
            "bsdf" : {
               "type" : "specular", 
            }, 
            
            "transform" : {
               "translate" : [ 0, .25, 0 ], 
               "scale"  : [ .5, .5, .5 ], 
               "sphere"  : { },
            },
         }, 

         "material": {
            "bsdf" : {
               "type" : "specular", 
            }, 
            
            "transform" : {
               "translate" : [ 0, .25, -1 ], 
               "scale"  : [ .5, .5, .5 ], 
               "sphere"  : { },
            },
         }, 
      }, 
   },
}

