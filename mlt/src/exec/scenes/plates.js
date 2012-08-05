{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         "type" : "rayCaster", 
         "noRenderThreads" : 1, 
         "noDirectSamples" : 1, 
         "maxDepth" : 1, 
         /*"ambient" : [ .5, .5, .5 ], */
      }, 
      
      /**
       * Camera 
       */
      "camera" : {
         "type"  : "pinhole", 
         "eye"   : [ 0, 2, 15 ], 
         "focus" : [ 0, -2, 2.5 ], 
         "up"    : [ 0, 1, 0 ], 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "spatialAccel" : { "type" : "naive", }, 
         
         /* Emitters */
         "material" : {
            "emitter" : {
               "type" : "oriented", 
               "power" : 7.4, 
            }, 
            
            /*"transform" : { "translate" : [ -3.75, 0, 0 ], "point" : { } }, */
            "transform" : { "translate" : [ -3.75, 0, 0 ], 
            "rotate" : [ 0,0,0,1,0,0,180 ], "scale" : [0.0666,1,0.0666], "plane" : { } }, 
            
            /*"sphere" : {
               "position" : [ -3.75, 0, 0 ], 
               "radius"   : 0.0333, 
            },*/
         }, 
         
         "material": {
            "emitter" : {
               "type" : "oriented", 
               "power" : 22.2222, 
            }, 
            
            "sphere" : {
               "position" : [ -1.25, 0, 0 ], 
               "radius"   : 0.1, 
            },
         },
         
         "material": {
            "emitter" : {
               "type" : "oriented", 
               "power" : 66.6666, 
            }, 
            
            "sphere" : {
               "position" : [ 1.25, 0, 0 ], 
               "radius"   : 0.3, 
            },
         },
         
         "material": {
            "emitter" : {
               "type" : "oriented", 
               "power" : 200, 
            }, 
            
            "transform" : { "translate" : [ 3.75, 0, 0 ], 
               "rotate" : [ 0,0,0,1,0,0,180 ], "scale" : [1.8,1,1.8], "plane" : { } }, 
            
            /*"sphere" : {
               "position" : [ 3.75, 0, 0 ], 
               "radius"   : 0.9, 
            },*/ 
         }, 
         
         /* overarching emitter providing background illumination */
         /*"material": {
            "emitter" : {
               "type" : "oriented", 
               "power" : 500, 
            }, 
            
            "transform" : {
               "translate" : [ 2, 10, 4 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ 2, 1, 2 ], 
               "plane"  : { }, 
            }, 
            "sphere" : {
               "position" : [ 10, 10, 4 ], 
               "radius"   : 0.5, 
            }, 
         }, */
         
         "material": { /* ufacet0 */
            "bsdf" : {
               "type" : "phong", 
               "kd"   : [ 0.07, 0.09, 0.13 ], 
               "ks"   : 0.87, 
               "n"    : 300, 
            }, 
            
            "triangle" : {
               "vertices" : [  4, -2.70651, 0.25609, 
                               4, -2.08375, -0.526323, 
                              -4, -2.08375, -0.526323 ], 
            },
            "triangle" : {
               "vertices" : [ -4, -2.08375, -0.526323, 
                              -4, -2.70651, 0.25609, 
                               4, -2.70651, 0.25609 ], 
            },
         }, 
         "material": {
            "bsdf" : {
               "type" : "phong", 
               "kd"   : [ 0.07, 0.09, 0.13 ], 
               "ks"   : 0.7, 
               "n"    : 200, 
            }, 
            
            "triangle" : {
               "vertices" : [  4, -3.28825, 1.36972, 
                               4, -2.83856, 0.476536, 
                              -4, -2.83856, 0.476536 ], 
            },
            "triangle" : {
               "vertices" : [ -4, -2.83856, 0.476536, 
                              -4, -3.28825, 1.36972, 
                               4, -3.28825, 1.36972 ], 
            },
         },
         "material": {
            "bsdf" : {
               "type" : "phong", 
               "kd"   : [ 0.07, 0.09, 0.13 ], 
               "ks"   : 0.5, 
               "n"    : 100, 
            }, 
            
            "triangle" : {
               "vertices" : [  4, -3.73096, 2.70046, 
                               4, -3.43378, 1.74564, 
                              -4, -3.43378, 1.74564 ], 
            },
            "triangle" : {
               "vertices" : [ -4, -3.43378, 1.74564, 
                              -4, -3.73096, 2.70046, 
                               4, -3.73096, 2.70046 ], 
            },
         },
         "material": {
            "bsdf" : {
               "type" : "phong", 
               "kd"   : [ 0.07, 0.09, 0.13 ], 
               "ks"   : 0.1, 
               "n"    : 50, 
            }, 
            
            "triangle" : {
               "vertices" : [  4, -3.99615, 4.0667, 
                               4, -3.82069, 3.08221, 
                              -4, -3.82069, 3.08221 ], 
            },
            "triangle" : {
               "vertices" : [ -4, -3.82069, 3.08221, 
                              -4, -3.99615, 4.0667, 
                               4, -3.99615, 4.0667 ], 
            },
         },
         "material": {
            "bsdf" : {
               "type" : "phong", 
               "kd"   : 0.4, 
            }, 
            
            "triangle" : {
               "vertices" : [ -100, -4.14615, -100, 
                              -100, -4.14615,  100, 
                               100, -4.14615,  100 ], 
            },
            "triangle" : {
               "vertices" : [  100, -4.14615,  100, 
                               100, -4.14615, -100, 
                              -100, -4.14615, -100 ], 
            },
            "triangle" : {
               "vertices" : [ -100, -100, -2,  
                               100, -100, -2,  
                               100,  100, -2 ], 
            },
            "triangle" : {
               "vertices" : [  100,  100, -2,  
                              -100,  100, -2,  
                              -100, -100, -2 ], 
            },
         },
      }, 
   },
}

