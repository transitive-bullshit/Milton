"scene" : {
   // declare the same sphere centered at the origin two different ways
   "shape" : { "type" : "sphere" }, 
   "sphere" : { }, 
   
   // declare a mesh
   "mesh" : {
      "path" : "path/to/mesh.obj", // ply or obj mesh
      "name" : "myMesh", // optionally specify a name
   }, 
   
   // declare a translated mesh instance with a different material
   "material" : {
      "bsdf" : { "type" : "diffuse", "kd" : [ 1, 0, 0 ], }, 
      "medium" : { "ior" : 1.49 }, 
      
      "transform" : {
         "translate" : [ 0, 5, 0 ], 
         "shape" : { "instance" : "myMesh" }, 
      }, 
   }, 
}, 

