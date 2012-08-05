"material" : {
   "_desc"  : "Declares a material which will be applied to all subshapes", 
   "_usage" : "usage/material.js", 
   "_class" : "Material", 
   "_info"  : { "type" : "material", "occurrences" : "zero or more", "default" : "default material applied to any object will be constant gray, and diffuse" },
}, 
"transform" : {
   "_desc"  : "Declares a transformation block which affects all subnodes", 
   "_usage" : "usage/transform.js", 
   "_info"  : { "type" : "transform", "occurrences" : "zero or more", "default" : "inherits transform from parent element (which can, itself, be a transform block); at top-level scene, default transform is the identity" }, 
}, 
"shape" : {
   "_brief" : "Declares a shape node", 
   "_note"  : "Aggregate shapes such as shapeSet may contain child materials and transforms.", 
   "_usage1" : "usage/shape.js", 
   "_info"  : { "type" : "shape", "occurrences" : "zero or more" },
}, 

