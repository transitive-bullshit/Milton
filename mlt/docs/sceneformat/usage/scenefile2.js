{
   // typical scenefile example
   "scenefile" : {
      "renderer" : {
         "type" : "pathTracer", 
         "noRenderThreads" : 10, 
         "noSuperSamples"  : 1000, 
      }, 
      
      "camera" : {
         "type" : "thinlens", 
         "eye"  : [ 0, 10, 0 ], 
         
         "aperture" : 50, 
         "fstop" : 18, 
      }, 
      
      "scene" : {
         // declare a planar, area light source
         "material" : {
            "emitter" : {
               "type" : "oriented", 
               "power" : [ 80, 80, 80 ], 
            }, 
            
            "plane" : { }, 
         }, 
      }, 
   },
}

