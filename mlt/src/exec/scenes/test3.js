{
   "scenefile" : {
      "version" : 0.1, 
      
      /**
       * Renderer
       */
      "renderer" : {
         /*"type" : "photonMapper", */
         "type" : "pathTracer", 
         /*"type" : "bidirectionalPathTracer", */
         "noRenderThreads" : 10, 
         "noDirectSamples" : 1, 
         
         "noSuperSamples"  : 4096, 
         "generator" : "super", 
         
         "diffuseGatherRadius" : 50, 
         "causticGatherRadius" : 20, 
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
            
            /*"plane" : {
               "vertices" : [
                  343.0, 547, 227.0, 
                  343.0, 547, 332.0,
                  213.0, 547, 332.0,
                  213.0, 547, 227.0,
               ], 
            },*/
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
         /*}, "material" : { 
            "bumpMap" : "images/bark/bark_disp.jpg", 
            "bumpIntensity" : 10,
            
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : "images/bark/bark_kd.jpg", 
            }, */
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
            "medium" : { "ior" : [ 1.46, 1.49, 1.52 ] }, 
            
            "transform" : {
               "translate" : [ 260, 230, 270 ], 
               "scale" : [ 400, 400, 400 ], 
               "rotate" : [ 0, 0, 0, 0, 1, 0, 180 ], 
               /*"sphere" : { }, */
               "mesh" : { "path" : "data/meshes/dragon.obj", }, 
            }, 
            /*"sphere" : {
               "position" : [ 186, 82.5, 169 ], 
               "radius"   : 80, 
            }, */
         },
         /*"material" : {
            "bsdf" : { 
               "type" : "dielectric", 
               "transparency" : 0, 
            }, 
            "medium" : { "ior" : 1.49 }, 
            
            "sphere" : {
               "position" : [ 368, 80, 351 ], 
               "radius"   : 80, 
            }, 
         }, */
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

