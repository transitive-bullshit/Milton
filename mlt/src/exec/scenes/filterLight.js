{
   /* @file   spec.js
    * @author Travis Fischer (fisch0920@gmail.com)
    * @date   Fall 2008
    * 
    * @brief
    *    Standard Cornell Box w/ specular balls
    * 
    * for more information, including concrete measurements, see
    *    http://www.graphics.cornell.edu/online/box/data.html
    */
   
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         /*"type" : "bidirectionalPathTracer", */
         "type" : "pathTracer", 
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
         "material" : { /* filter */
            "bsdf" : {
               "type" : "dielectric", 
               "transparency" : 1, 
               "ks"   : [ 1, 1, 1 ], 
            }, 
            /*"bsdf" : { "type" : "diffuse", }, */
            "transform" : {
               "translate" : [ 278, 520, 279.5 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ 200, 1, 200 ], 
               "plane"  : { },
            },
         }, 
         "material" : { /* white */
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : .75, /*[ 0.75, 0.75, 0.5 ], */
            }, 
            "plane" : { /* floor */
               "vertices" : [
                  552.8, 0.0,   0.0, 
                    0.0, 0.0,   0.0, 
                    0.0, 0.0, 559.2, 
                  549.6, 0.0, 559.2, 
               ],
            },
            "plane" : { /* ceiling */
               "vertices" : [
                  556.0, 548.8, 0.0,   
                  556.0, 548.8, 559.2,
                  0.0, 548.8,   559.2,
                  0.0, 548.8,   0.0,
               ], 
            },
            "plane" : { /* back wall */
               "vertices" : [
                  549.6,   0.0, 559.2, 
                    0.0,   0.0, 559.2,
                    0.0, 548.8, 559.2,
                  556.0, 548.8, 559.2,
               ], 
            },
         },
         "material" : {
            "bsdf" : { 
               "type" : "dielectric", 
               "transparency" : 1, 
            }, 
            "medium" : { "ior" : 1.49 }, 
            
            /* short block */
            "sphere" : {
               "position" : [ 186, 82.5, 169 ], 
               "radius"   : 80, 
            }, 
            /* end short block */
         },
         "material" : {
            "bsdf" : { 
               "type" : "dielectric", 
               "transparency" : 0, 
            }, 
            "medium" : { "ior" : 1.49 }, 
            
            /* tall block */
            "sphere" : {
               "position" : [ 368, 80, 351 ], 
               "radius"   : 80, 
            }, 
            /* end tall block */
         }, 
         "material" : { /* green */
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ 0.15, 0.45, 0.1 ], 
            }, 
            "plane" : { /* right wall */
               "vertices" : [
                  0.0,   0.0, 559.2,   
                  0.0,   0.0,   0.0,
                  0.0, 548.8,   0.0,
                  0.0, 548.8, 559.2,
               ],
            },
         },
         "material" : { /* red */
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : [ 0.65, 0.05, 0.06 ], 
            }, 
            "plane" : { /* left wall */
               "vertices" : [
                  552.8,   0.0,   0.0, 
                  549.6,   0.0, 559.2,
                  556.0, 548.8, 559.2,
                  556.0, 548.8,   0.0,
               ],
            },
         },
      },
   },
}

