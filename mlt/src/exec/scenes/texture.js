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
         "type" : "pinhole", 
         "eye"   : [ 0, 5, 0 ], 
         "up"    : [ 1, 0, 0 ], 
      }, 
      
      /**
       * Scene (materials and shapes) 
       */
      "scene" : {
         "material": {
            "emitter" : {
               "type" : "omni", 
               "power" : 60, 
            }, 
            
            "transform" : {
               "translate" : [ 0, 1, 0 ], 
               "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
               "scale"  : [ .75, 1, .75 ], 
               "point"  : { },
            },
         }, 
         
         "material" : {
            /*"bumpMap" : "images/bark/bark_disp.jpg", 
            "bumpIntensity" : 5, */
            
            "repeatU" : 2, 
            "repeatV" : 2, 
            
            "bsdf" : {
               "type" : "diffuse", 
               "kd"   : "images/checkerboard.png", 
               /*"kd"   : [ .8, .1, .1 ], "/course/cs123/data/image/lena.png", */
               /*"kd"   : "images/bark/bark_kd.jpg", */
            },
            
            "transform" : {
               "scale" : [ 3, 1, 3 ], 
               "plane" : { },
            }, 
         },
      }, 
   },
}

