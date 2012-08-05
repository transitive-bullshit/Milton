{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         /*"ambient" : [ .7, .7, .7 ], */
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "pinhole", 
         /* optional parameters */
         "eye"   : [ 2, 2, 2 ], 
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
         
         "material" : {
            "name" : "material0", 
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : .75, 
            }, 
         }, 
         
         "material" : {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ 0, 0, 1], 
            }, 
            "transform" : {
               "translate" : [ -1, 0, 0 ], 
               "scale" : [ 1.5, 1.5, 1.5 ], 
               "mesh" : {
                  "name" : "s0", 
                  "path" : "data/meshes/lion.obj", 
               }, 
            }, 
         }, 
         "material" : {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ 1, 0, 0 ], 
            }, 
            "transform" : {
               "translate" : [ 1, 0, 0 ], 
               "scale" : [ .8, .8, .8 ], 
               "shape" : { "instance" : "s0", }, 
            }, 
         }, 
      },
   },
}

