"material": {
   // this example material declares a planar area light source
   // declare reflectivity (optional)
   "bsdf" : {
      "type" : "diffuse", 
      "kd"   : 0.5, 
   }, 
   // declare emittance (optional)
   "emitter" : {
      "type" : "oriented", 
      "power" : [ 80, 80, 80 ], 
   }, 
   
   // subnodes to which this material is applied
   "transform" : { 
      "translate" : [ 0, 1, 0 ], 
      // rotate plane to face downwards towards origin
      "rotate" : [ 0, 0, 0, 1, 0, 0, 180 ], 
      "scale"  : [ .75, 1, .75 ], 
      "plane"  : { },
   },
},

