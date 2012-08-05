{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         "noDirectSamples" : 16, 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "pinhole", 
         "eye"   : [ 0, 3, 0 ], 
         "up"    : [ 1, 0, 0 ], 
         "heightAngle" : 45, 
         "aspectRatio" : 1.0, 
         "near"  : 1, 
         "far"   : 100, 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material": {
            "emitter" : {
               "type" : "omni", 
               "power" : [ 40, 40, 40 ], 
            }, 
            
            "transform" : {
               "translate" : [ 0, 1, 0 ], 
               "transform" : {
                  "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
                  "scale"  : [ .75, 1, .75 ], 
                  "point"  : { },
               }, 
            },
         }, 
         
         "spatialAccel" : {
            "type" : "naive", 
            "kdMaxDepth" : 2, 
         },
         
         "transform" : {
            "scale" : [ 2, 1, 2 ], 
            "material" : {
               "bsdf" : { "type" : "diffuse", "kd" : [ 1, 0, 0 ] }, 
               "transform" : { "translate" : [  .5, 0,  0.5 ], "transform" : { "rotate" : [ 0, 0, 0, 0, 1, 0, 180 ], "triangle" : { }, }, }, 
            },
            "material" : {
               "bsdf" : { "type" : "diffuse", "kd" : [ 0, 1, 0 ] }, 
               "transform" : { "translate" : [ 0, 0, 0 ], "triangle" : { }, }, 
            },
            "material" : {
               "bsdf" : { "type" : "diffuse", "kd" : [ 0, 0, 1 ] }, 
               "transform" : { "translate" : [  0, 0, -.5 ], "triangle" : { }, }, 
            },
            "material" : {
               "bsdf" : { "type" : "diffuse", "kd" : [ 1, 1, 1 ] }, 
               "transform" : { "translate" : [  .5, 0,  0 ], "transform" : { "rotate" : [ 0, 0, 0, 0, 1, 0, 180 ], "triangle" : { }, }, }, 
            },
         },
      }, 
   },
}

