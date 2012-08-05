{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         "noDirectSamples" : 9, 
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type" : "pinhole", 
         /*"eye"   : [ 2, .5, 2 ], */
         "eye"   : [ 0, 5, 0 ], 
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
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : 0.5, 
            }, 
            "emitter" : {
               "type" : "oriented", 
               "power" : [ 80, 80, 80 ], 
            }, 
            
            "transform" : { 
               "translate" : [ 0, 1, 0 ], 
               /* rotate plane to face downwards towards origin */
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ .75, 1, .75 ], 
               "plane"  : { },
            },
         }, 
         
         "material" : {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : 0.5, /*"images/checkerboard.png",*/
            },
            
            "transform" : {
               "scale" : [ 3, 1, 3 ], 
               "plane" : { },
            }, 
         },
      }, 
   },
}

