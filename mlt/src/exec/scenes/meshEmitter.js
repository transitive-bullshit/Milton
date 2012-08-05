{
   /* @file   meshEmitter.js
    * @author Travis Fischer (fisch0920@gmail.com)
    * @date   Fall 2008
    * 
    * @brief
    *    Standard Cornell Box with mesh emitter
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
         "noRenderThreads" : 10, 
         "noDirectSamples" : 1, 
         
         "noSuperSamples"  : 10000, 
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
         "near"  : 10, 
         "far"   : 2000,
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "spatialAccel" : {
            "type" : "kdTree", 
            "kdSplitPlaneType": "splitPlaneSAH", 
            /*"kdMaxDepth" : 3, */
         },
         
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
               /*"translate" : [ 200, 517, 279.5 ], */
               "translate" : [ 190, 165, 149.5 ], 
               /*"rotate" : [ 0, 0, 0, 1, 0, 0, 90 ], 
               "scale"  : [ 140, 140, 140 ], */
               "rotate" : [ 0, 0, 0, 0, 1, 0, 90 ], 
               "scale"  : [ 220, 220, 220 ], 
               "mesh"   : { "path" : "data/meshes/lion.obj", },
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
            
            /* short block */
            "plane" : {
               "vertices" : [
                  130.0, 165.0,  65.0, 
                   82.0, 165.0, 225.0,
                  240.0, 165.0, 272.0,
                  290.0, 165.0, 114.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  290.0,   0.0, 114.0,
                  290.0, 165.0, 114.0,
                  240.0, 165.0, 272.0,
                  240.0,   0.0, 272.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  130.0,   0.0,  65.0,
                  130.0, 165.0,  65.0,
                  290.0, 165.0, 114.0,
                  290.0,   0.0, 114.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                   82.0,   0.0, 225.0,
                   82.0, 165.0, 225.0,
                  130.0, 165.0,  65.0,
                  130.0,   0.0,  65.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  240.0,   0.0, 272.0,
                  240.0, 165.0, 272.0,
                   82.0, 165.0, 225.0,
                   82.0,   0.0, 225.0,
               ], 
            }, 
            /* end short block */
         },
         "material" : {
            "bsdf" : { 
               "type" : "dielectric", 
               "transparency" : 0, 
            }, 
            "medium" : { "ior" : 1.49 }, 
            
            "sphere" : {
               "position" : [ 368, 80, 351 ], 
               "radius"   : 80, 
            }, 
         }, 
         /* tall block */
         /*"material" : {
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : .75, 
            }, 
            "plane" : {
               "vertices" : [
                  423.0, 330.0, 247.0,
                  265.0, 330.0, 296.0,
                  314.0, 330.0, 456.0,
                  472.0, 330.0, 406.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  423.0,   0.0, 247.0,
                  423.0, 330.0, 247.0,
                  472.0, 330.0, 406.0,
                  472.0,   0.0, 406.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  472.0,   0.0, 406.0,
                  472.0, 330.0, 406.0,
                  314.0, 330.0, 456.0,
                  314.0,   0.0, 456.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  314.0,   0.0, 456.0,
                  314.0, 330.0, 456.0,
                  265.0, 330.0, 296.0,
                  265.0,   0.0, 296.0,
               ], 
            }, 
            "plane" : {
               "vertices" : [
                  265.0,   0.0, 296.0,
                  265.0, 330.0, 296.0,
                  423.0, 330.0, 247.0,
                  423.0,   0.0, 247.0,
               ], 
            }, 
         }, */
         /* end tall block */
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

