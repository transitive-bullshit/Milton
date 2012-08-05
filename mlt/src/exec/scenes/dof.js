{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "pathTracer", 
         "noRenderThreads" : 8, 
         "noDirectSamples" : 1, 
         
         "noSuperSamples"  : 1000, 
         "generator" : "super", 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "thinlens", 
         "eye"   : [ 30, 18, 0 ], 
         "up"    : [ 0, 1, 0 ], 
         "heightAngle" : 45, 
         "aspectRatio" : 1.0, 
         /*"focalPoint" : [ .5, .5 ], */ /* focus on center ball */
         "focalDistance" : 24,  /* distance to closest ball */
         /*"focalDistance" : 40, */ /* distance to farthest ball */
         
         "aperture" : 35, 
         "fstop" : 8, 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material": {
            "emitter" : {
               "type" : "oriented", 
               "power" : [ 15000, 15000, 15000 ], 
               /*"power" : [ 1500000, 1500000, 1500000 ], */
            }, 
            
            /*"transform" : {
               "translate" : [ 0, 100, 0 ], 
               "point"  : { },
            }, */
            
            "transform" : {
               "translate" : [ 0, 20, 0 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ 5, 1, 5 ], 
               "plane"  : { },
            }, 
         }, 
         
         "material": {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ .8, .1, .1 ], 
            }, 
            
            "transform" : {
               "scale"  : [ 100, 1, 100 ], 
               "plane"  : { },
            }, 
         }, 
         
         "material": {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ .5, .5, .5 ], 
            }, 
            
            "transform" : {
               "translate" : [ 10, 2.5, 0 ], 
               "scale"  : [ 5, 5, 5 ], 
               
               "mesh" : { "path" : "data/meshes/topMod/OBJ2/doug1.obj", "normalize" : true, }, 
               /*"sphere"  : { }, */
            },
         }, 
         
         "material": {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ .5, .5, .5 ], 
            }, 
            
            "transform" : {
               "translate" : [ 0, 2.5, 0 ], 
               "scale"  : [ 5, 5, 5 ], 
               
               "mesh" : { "path" : "data/meshes/topMod/OBJ2/doug2.obj", "normalize" : true, }, 
               /*"sphere"  : { }, */
            },
         }, 

         "material": {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ .5, .5, .5 ], 
            }, 
            
            "transform" : {
               "translate" : [ -10, 2.5, 0 ], 
               "scale"  : [ 5, 5, 5 ], 
               
               "mesh" : { "path" : "data/meshes/topMod/OBJ2/doug3.obj", "normalize" : true, }, 
               /*"sphere"  : { }, */
            },
         }, 
      }, 
   },
}

